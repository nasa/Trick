
%module sim_services

%{
#include <cstddef>
#include <random>
%}

#include "trick/swig/trick_swig.i"
#include "trick/swig/swig_class_typedef.i"
#include "trick/swig/cast_as.i"
#include "units_attach.i"
#include "extra_functions.i"

// Special typemap for collectee in add_collect and delete_collect.
%typemap(in) void * collectee {
    // VOID *
    void * argp2 ;

    if ( SWIG_IsOK(SWIG_ConvertPtr($input, &argp2,SWIG_TypeQuery("void *"), 0)) ) {
        // We have an address coming in, we don't have to do any translation
        $1 = reinterpret_cast< $1_basetype * >(argp2) ;
    } else if ( SWIG_IsOK(SWIG_ConvertPtr($input, &argp2,SWIG_TypeQuery("swig_ref *"), 0)) ) {
        // Array to pointer assignment
        swig_ref * temp_swig_ref = reinterpret_cast< swig_ref * >(argp2);
        if ( temp_swig_ref != NULL ) {
            $1 = ($1_basetype *)temp_swig_ref->ref.address ;
        }
        // package the array address as a void
    }
}

%inline %{
#include "trick/swig/swig_global_vars.hh"

#include "trick/reference_frame.h"
#include "trick/rand_generator.h"
#include "trick/wave_form.h"
#include "trick/integrator_c_intf.h"
#include "trick/units_conv.h"

#include "trick/GetTimeOfDayClock.hh"
#include "trick/clock_proto.h"
#include "trick/CommandLineArguments.hh"
#include "trick/command_line_protos.h"
#include "trick/DRAscii.hh"
#include "trick/DRBinary.hh"
#ifdef HDF5
#include "trick/DRHDF5.hh"
#endif
#include "trick/DataRecordDispatcher.hh"
#include "trick/data_record_proto.h"
#include "trick/EchoJobs.hh"
#include "trick/echojobs_proto.h"
#include "trick/Environment.hh"
#include "trick/env_proto.h"
#include "trick/Executive.hh"
#include "trick/ExecutiveException.hh"
#include "trick/exec_proto.h"
#include "trick/exec_proto.hh"
#include "trick/MalfunctionsTrickView.hh"
#include "trick/MonteMonitor.hh"
#include "trick/SimControlPanel.hh"
#include "trick/StripChart.hh"
#include "trick/TrickView.hh"
#include "trick/ExternalApplicationManager.hh"
#include "trick/external_application_c_intf.h"
#include "trick/FrameDataRecordGroup.hh"
#include "trick/FrameLog.hh"
#include "trick/framelog_proto.h"
#include "trick/IPPython.hh"
#include "trick/input_processor_proto.h"
#include "trick/MTV.hh"
#include "trick/EventManager.hh"
#include "trick/EventProcessor.hh"
#include "trick/EventManager_c_intf.hh"
#include "trick/JITInputFile.hh"
#include "trick/jit_input_file_proto.hh"
#include "trick/JSONVariableServer.hh"
#include "trick/IntegLoopScheduler.hh"
#include "trick/IntegLoopManager.hh"
#include "trick/IntegLoopSimObject.hh"
#include "trick/Integrator.hh"
#include "trick/regula_falsi.h"

#include "trick/ABM_Integrator.hh"
#include "trick/Euler_Cromer_Integrator.hh"
#include "trick/Euler_Integrator.hh"
#include "trick/MM4_Integrator.hh"
#include "trick/NL2_Integrator.hh"
#include "trick/RK2_Integrator.hh"
#include "trick/RK4_Integrator.hh"
#include "trick/RKF45_Integrator.hh"
#include "trick/RKF78_Integrator.hh"
#include "trick/RKG4_Integrator.hh"

#include "trick/SimTime.hh"
#include "trick/simtime_proto.h"

#include "trick/MSSocket.hh"
#include "trick/MSSharedMem.hh"
#include "trick/Master.hh"
#include "trick/mc_master.hh"
#include "trick/mc_python_code.hh"
#include "trick/mc_variable_file.hh"
#include "trick/mc_variable_fixed.hh"
#include "trick/mc_variable.hh"
#include "trick/mc_variable_random_bool.hh"
#include "trick/mc_variable_random.hh"
#include "trick/mc_variable_random_normal.hh"
#include "trick/mc_variable_random_string.hh"
#include "trick/mc_variable_random_uniform.hh"
#include "trick/mc_variable_semi_fixed.hh"
#include "trick/Slave.hh"
#include "trick/master_proto.h"
#include "trick/MemoryManager.hh"
#include "trick/memorymanager_c_intf.h"
#include "trick/CheckPointRestart.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/collect_proto.hh"
#include "trick/AttributesMap.hh"
#include "trick/sie_c_intf.h"
#include "trick/MessageCout.hh"
#include "trick/MessageThreadedCout.hh"
#include "trick/MessageFile.hh"
#include "trick/MessageHSFile.hh"
#include "trick/MessageCustomFile.hh"
#include "trick/MessageCustomManager.hh"
#include "trick/MessageLCout.hh"
#include "trick/MessagePublisher.hh"
#include "trick/MessageSubscriber.hh"
#include "trick/MessageTCDevice.hh"
#include "trick/Message_proto.hh"
#include "trick/message_proto.h"
#include "trick/MonteCarlo.hh"
#include "trick/montecarlo_c_intf.h"
#include "trick/MonteVarCalculated.hh"
#include "trick/MonteVarFile.hh"
#include "trick/MonteVarFixed.hh"
#include "trick/MonteVarRandom.hh"
#include "trick/RealtimeSync.hh"
#include "trick/realtimesync_proto.h"
#include "trick/RtiExec.hh"
#include "trick/RtiStager.hh"
#include "trick/ITimer.hh"
#include "trick/Unit.hh"
#include "trick/UnitTest.hh"
#include "trick/trick_tests.h"
#include "trick/VariableServer.hh"
#include "trick/variable_server_proto.h"
#include "trick/vval.h"
#include "trick/Flag.h"
#include "trick/UdUnits.hh"
#include "trick/MyCivetServer.hh"

#ifdef USE_ER7_UTILS_INTEGRATORS
#include "er7_utils/integration/core/include/integrator_constructor_factory.hh"
#include "er7_utils/integration/core/include/integrable_object.hh"
#include "er7_utils/integration/core/include/base_integration_group.hh"
#include "er7_utils/trick/integration/include/trick_first_order_ode_integrator.hh"
#include "er7_utils/trick/integration/include/trick_second_order_ode_integrator.hh"
#endif

%}

%pythoncode %{
import os
import linecache
import re

curr_dir = os.getcwd()
exclude_dir = os.getcwd() + "/trick"

def traceit(frame, event, arg):
    if event == "line":
        lineno = frame.f_lineno
        filename = frame.f_code.co_filename
        if ( not filename.startswith(exclude_dir) and not filename.startswith("/usr") and not filename.startswith("/opt") and not filename.startswith("<") and not filename.startswith(".trick/") ):
            if (filename.endswith(".pyc") or
                filename.endswith(".pyo")):
                filename = filename[:-1]
            line = linecache.getline(filename, lineno)
            print (filename,":",lineno,": ",line.rstrip())
    return traceit
%}

