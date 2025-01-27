# Under Construction!

I just want to know how to do things in the input file:

## Simulation Controls
```python
# Set a simulation stop time. default 9x10e18
trick.stop(<time>)
trick.exec_set_terminate_time(<time>)
# Set a initial simulation time. default 0.0 Rarely used.
trick.exec_set_time(<time>)

# Start simulation in freeze mode. default False
trick.exec_set_freeze_command(True|False)
# Freeze only when simulation reaches end of software frame. default False
trick.exec_set_freeze_on_frame_boundary(True|False)
# Set <CTRL-C> to toggle sim to freeze/run. default False
trick.exec_set_enable_freeze(True|False)

# Set the number of time divisions (tics) in a second. default 1000000
trick.exec_set_time_tic_value(<num_tics>)
# Set the software frame. default 1.0
trick.exec_set_software_frame(<time>)
# Set the software frame while in freeze mode. default 1.0
trick.exec_set_freeze_frame(<time>)
# Release the processor for an instant while waiting on events. default True
trick.exec_set_rt_nap(True|False)

# Process a string of commands at a later simulation time
trick.add_read(<time>,"<command string>")
```

## SimObject and Job control
```python
# Several calls use an instance_num parameter.  If jobs have the same name in a sim_object they will be numbered with an instance_num.  instance_num starts at 1.  Unique job names will have instance_num = 1

# Turn a simobject on/off
trick.exec_set_sim_object_onoff(<sim_object_name>, True|False)
<sim_object>.enable()
<sim_object>.disable()

# Finding a job by name
trick.exec_get_job(<job_name>, <instance_num>)
# Turn a job on/off.
trick.exec_set_job_onoff(<job_name>, <instance_num>, True|False)
trick.exec_get_job(<job_name>, <instance_num>).enable()
trick.exec_get_job(<job_name>, <instance_num>).disable()
# Set a job cycle
trick.exec_set_job_cycle(<job_name>, <instance_num>, <cycle>)
```

## Debugging
```python
# Turn on/off echoing job names as they are executed.
trick.echo_jobs_on()
trick.echo_jobs_off()

# Turn on/off pausing execution before each job is run
trick.debug_pause_on()
trick.debug_pause_off()
trick.debug_pause_signal() # signal sim to advance to next job while paused.

# Print stack trace if signal is caught. default True
trick.exec_set_stack_trace(True|False)
# Attach a debugger to the process if a signal is caught. default False
trick.exec_set_attach_debugger(True|False)
# Debugger command to use when attaching. default "gdb"
trick.exec_set_debugger_command(<command_string>)
# Trap SIGBUS signals. default True
trick.exec_set_trap_sigbus(True|False)
# Trap SIGFPE signals. default False
trick.exec_set_trap_sigfpe(True|False)
# Trap SIGSEGV signals. default True
trick.exec_set_trap_sigsegv(True|False)
```

## Checkpointing
```python
# Save a checkpoint before the initialization jobs execute. default False
trick.checkpoint_pre_init(True|False)
# Save a checkpoint after the initialization jobs execute. default False
trick.checkpoint_post_init(True|False)
# Save a checkpoint when the simulation ends. default False
trick.checkpoint_end(True|False)
# Save a checkpoint at a time in the future
trick.checkpoint(<time>)
# Save a checkpoint at a time in the future to a specified file
trick.checkpoint(<time>, <file_name>)
# Save a checkpoint now
trick.checkpoint()
# Save a checkpoint now to a specified file
trick.checkpoint(<file_name>)

# Set the CPU to use for checkpoints
trick.checkpoint_cpu(<cpu_num>)

# Save a checkpoint periodically during simulation execution. default False
trick.checkpoint_safestore_set_enabled(True|False)
# Set the safestore checkpoint period. default 9x10e18
trick.checkpoint_safestore(<period>)
```

## Data Recording
```python
# create a new data recording group
drg = trick.DRAscii(<group_name>)
drg = trick.DRBinary(<group_name>)
drg = trick.DRHDF5(<group_name>)
# add parameter to data recording group
drg.add_variable(<var_name>
# Set cycle rate of recording. default 0.1
drg.set_cycle(<period>)
# Set recording frequency
# <freq> = trick.DR_Always | trick.DR_Changes | trick.DR_Changes_Step
drg.freq = <freq>
# Set the job class of recording job. default "logging"
drg.set_job_class(<job_class_name>)
# Set the execution phase of the recording job. default 60000
drg.set_phase(<phase_num>)
# Set execution thread to record on. default 0
drg.set_thread(<thread_id>)
# Set floating point recording to single or double position. default False
drg.set_single_prec_only(True|False)

# Enable (dynamically) the data recording group
drg.enable()
# Disable (dynamically) the data recording group
drg.disable()


# Add recording group to simulation
# <buffering> = trick.DR_Buffer | trick.DR_No_Buffer | trick.Ring_Buffer | trick.Thread_Buffer
trick.add_data_record_group( drg , <buffering>)

```

## Memory Allocation
```python
# return the address of a variable by name
trick.get_address(<var_name>)
```

## Realtime
```python
# Enable real time
trick.real_time_enable()
# Disable real time
trick.real_time_disable()
# Lock all process memory into RAM.
trick.exec_set_lock_memory(True|False)
```

## Thread Control
```python
# Sets the thread rendezvous cycle time. default 1.0
trick.exec_set_thread_amf_cycle_time(<thread_id>, <cycle_time>)
# Sets the thread rendezvous cycle time (same as exec_set_thread_amf_cycle_time). default 1.0
trick.exec_set_thread_async_cycle_time(<thread_id>, <cycle_time>)
# Wait for asynchronous thread to complete before terminating simulation. default False
trick.exec_set_thread_async_wait(<thread_id>, True|False)
# Assign thread to run on specific processor. default None
trick.exec_set_thread_cpu_affinity(<thread_id>, <cpu_num>)
# Set thread execution priority. default 0
trick.exec_set_thread_priority(<thread_id>, <priority>)
# Set thread rendezvous type. default trick.PROCESS_TYPE_SCHEDULED
# <process_type> = trick.PROCESS_TYPE_SCHEDULED | trick.PROCESS_TYPE_AMF_CHILD | trick.PROCESS_TYPE_ASYNC_CHILD
trick.exec_set_thread_process_type(<thread_id>, <process_type>)

# Return a thread object
trick.exec_get_thread(<thread_id>)
# Set thread trigger mechanism. default trick.TT_MUTEX
# mechanism = trick.TT_MUTEX | trick.TT_FLAG | trick.TT_EVENTFD | trick.TT_FUTEX
trick.exec_get_thread(<thread_id>).trigger_container.setThreadTrigger(<mechanism>)

# enable/disable a thread.  Not widely used.
trick.exec_set_thread_enabled(<thread_id>, True|False)

# Deprecated
trick.exec_set_thread_rt_semaphores(<thread_id>, True|False) 
```

