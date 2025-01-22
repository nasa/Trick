import sys
import os

thisdir = os.path.abspath(os.path.dirname(os.path.realpath(__file__)))
sys.path.append(os.path.join(thisdir,"share/trick/trickops"))

from TrickWorkflow import *
from WorkflowCommon import Job

max_retries = 5

class SimTestWorkflow(TrickWorkflow):
    def __init__( self, quiet, trick_top_level, cpus, config_file):
        self.cpus = cpus
        # Create the trick_test directory if it doesn't already exist
        if not os.path.exists(trick_top_level + "/trick_test"):
          os.makedirs(trick_top_level + "/trick_test")

        # Base Class initialize, this creates internal management structures
        TrickWorkflow.__init__(self, project_top_level=(trick_top_level), log_dir=(trick_top_level +'/trickops_logs/'),
            trick_dir=trick_top_level, config_file=(trick_top_level + "/" + config_file), cpus=self.cpus, quiet=quiet)
    def run( self ):
      build_jobs      = self.get_jobs(kind='build')

      # This is awful but I can't think of another way around it
      # SIM_test_varserver has 2 tests that should return the code for SIGUSR1, the number is different on Mac vs Linux
      # so it can't be hardcoded in the input yml file. Maybe this is a case having a label on a run would be cleaner?
      import signal
      run_names = ["Run test/SIM_test_varserv RUN_test/err1_test.py", "Run test/SIM_test_varserv RUN_test/err2_test.py"]
      for job in [job for job in self.get_jobs(kind='run') if job.name in run_names]:
        job._expected_exit_status = signal.SIGUSR1.value

      # Several test sims have runs that require ordering via phases:
      #  - SIM_stls dumps a checkpoint that is then read in and checked by a subsequent run
      #  - SIM_checkpoint_data_recording dumps checkpoints that are read by subsequent runs
      #  - SIM_test_varserver has 3 runs that cannot be concurrent
      #  - SIM_mc_generation generates runs and then runs them
      phases = [-1, 0, 1, 2, 3]

      analysis_jobs   = self.get_jobs(kind='analyze')
      if platform == "darwin":
        for job in build_jobs:
          if job.name == "Build test/SIM_trickified_shared" :
            print("REMOVING JOB: " + job.name)
            build_jobs.remove(job)
      builds_status = self.execute_jobs(build_jobs, max_concurrent=self.cpus, header='Executing all sim builds.')

      jobs = build_jobs

      run_status = 0
      for phase in phases:
        run_jobs = self.get_jobs(kind='run', phase=phase)
        if platform == "darwin":
          for job in run_jobs:
            if job.name == "Run test/SIM_trickified_shared RUN_test/unit_test.py" :
              print("REMOVING JOB: " + job.name)
              run_jobs.remove(job)
        this_status = self.execute_jobs(run_jobs, max_concurrent=self.cpus, header="Executing phase " + str(phase) + " runs.", job_timeout=1000)
        run_status = run_status or this_status
        jobs += run_jobs

      comparison_result = self.compare()
      analysis_status = self.execute_jobs(analysis_jobs, max_concurrent=self.cpus, header='Executing all analysis.')

      self.report()           # Print Verbose report
      self.status_summary()   # Print a Succinct summary

      # Dump failing logs
      for job in jobs:
        if job.get_status() == Job.Status.FAILED or job.get_status() == Job.Status.TIMEOUT:
          print ("*"*120)
          if job.get_status() == Job.Status.FAILED:
            header = "Failing job: " + job.name
          else:
            header = "Timed out job: " + job.name

          numspaces = int((120 - 20 - len(header))/2 -2)
          print("*"*10, " "*numspaces, header, " "*numspaces, "*"*10,)
          print ("*"*120)
          print(open(job.log_file, "r").read())
          print ("*"*120, "\n\n\n")

      return (builds_status or run_status or len(self.config_errors) > 0 or comparison_result or analysis_status)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Build, run, and compare all test sims for Trick',
      formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument( "--trick_top_level", type=str, help="Path to TRICK_HOME", default=thisdir)
    parser.add_argument( "--quiet", action="store_true", help="Suppress progress bars (automatically set to True if environment variable CI is present).")
    parser.add_argument( "--cpus", type=int, default=(os.cpu_count() if os.cpu_count() is not None else 8),
      help="Number of cpus to use for testing. For builds this number is used for MAKEFLAGS *and* number of "
        "concurrent builds (cpus^2). For sim runs this controls the maximum number of simultaneous runs.")
    parser.add_argument( "--config_file", type=str, help="Run configuration file to use, relative to trick_top_level", default="test_sims.yml")

    myargs = parser.parse_args()
    should_be_quiet = myargs.quiet or os.getenv('CI') is not None
    sys.exit(SimTestWorkflow(quiet=should_be_quiet, trick_top_level=myargs.trick_top_level, cpus=myargs.cpus, config_file=myargs.config_file).run())
