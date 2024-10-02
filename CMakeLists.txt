cmake_minimum_required(VERSION 3.1)

# trick is a C/C++ project, but we have some macOS
# configuration to do before CMake searches for compilers
project(trick NONE)
set(TRICK_MAJOR 19)
set(TRICK_MINOR 8)
set(TRICK_TINY 0)
# set TRICK_PRERELEASE TO EMPTY STRING "" ON RELEASE
set(TRICK_PRERELEASE "-beta")

# On macOS Mojave and Catalina, the compilers in /usr/bin 
# are the ones that include the correct C standard library system headers
if(CMAKE_SYSTEM_NAME MATCHES Darwin)
    if ( (NOT DEFINED CMAKE_C_COMPILER) AND (NOT DEFINED ENV{CC}) AND (EXISTS /usr/bin/cc) )
        set(CMAKE_C_COMPILER /usr/bin/cc)
    endif()
    if ( (NOT DEFINED CMAKE_CXX_COMPILER) AND (NOT DEFINED ENV{CXX}) AND (EXISTS /usr/bin/c++) )
        set(CMAKE_CXX_COMPILER /usr/bin/c++)
    endif()
endif()


enable_language(C)
enable_language(CXX)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TRICK_FORCE_32BIT OFF CACHE BOOL "Set Trick to compile in 32bit mode")
#set(USE_JAVA ON CACHE BOOL "Use java")
set(USE_ER7_UTILS ON CACHE BOOL "Use er7_utils")
set(UDUNITS2_ROOT "" CACHE STRING "UDUNITS home directory")
set(TRICK_MONGOOSE "0" CACHE STRING "Enable webserver")

#message("UDUNITS2_ROOT = ${UDUNITS2_ROOT}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(EXISTS "/etc/redhat-release")
    if(TRICK_FORCE_32BIT)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    else()
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib64)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib64)
    endif()
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
include(UseJava)
find_package(Java REQUIRED)
find_package(Maven REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(LLVM 3.4 REQUIRED)
find_package(Perl REQUIRED)
find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(SWIG REQUIRED)
find_package(Tee REQUIRED)
find_package(Threads REQUIRED)
find_package(UDUNITS2 REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(HDF5)
find_package(GSL)

find_package(X11)
find_package(Motif)

add_definitions( -DTRICK_VER=${TRICK_MAJOR} )

if(USE_ER7_UTILS)
    add_definitions( -DUSE_ER7_UTILS_INTEGRATORS)
endif()

if(GSL_FOUND)
    add_definitions( -D_HAVE_GSL)
endif()

if(USE_MONGOOSE)
    add_definitions(-DUSE_MONGOOSE)
endif()

include_directories( ${CMAKE_BINARY_DIR}/include)
include_directories( ${CMAKE_BINARY_DIR}/include/trick/compat)

file(COPY bin DESTINATION ${CMAKE_BINARY_DIR})
file(COPY include DESTINATION ${CMAKE_BINARY_DIR})
file(COPY libexec DESTINATION ${CMAKE_BINARY_DIR})
file(COPY share DESTINATION ${CMAKE_BINARY_DIR})
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/mongoose)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/temp_src/io_src)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/temp_src/lex_yacc)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/temp_src/mongoose)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/temp_src/swig)
# copy er7_util header files to build directory
file(GLOB_RECURSE ER7_UTIL_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/er7_utils ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/er7_utils/*.hh)
foreach ( infile ${ER7_UTIL_HEADERS} )
    get_filename_component(dir ${infile} DIRECTORY)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/er7_utils/${infile} DESTINATION "${CMAKE_BINARY_DIR}/include/er7_utils/${dir}")
endforeach(infile)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/share/trick/makefiles/config_user_cmake.mk.in ${CMAKE_BINARY_DIR}/share/trick/makefiles/config_user.mk)

###############################################################
# mongoose lib
###############################################################
if(USE_MONGOOSE)
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/include/mongoose/mongoose.h
  COMMAND curl --retry 4 -O https://raw.githubusercontent.com/cesanta/mongoose/6.16/mongoose.h
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/include/mongoose
)

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/temp_src/mongoose/mongoose.c
  COMMAND curl --retry 4 -O https://raw.githubusercontent.com/cesanta/mongoose/6.16/mongoose.c
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/temp_src/mongoose
  DEPENDS ${CMAKE_BINARY_DIR}/include/mongoose/mongoose.h
)

add_library(mongoose STATIC ${CMAKE_BINARY_DIR}/temp_src/mongoose/mongoose.c)
target_include_directories( mongoose PUBLIC ${CMAKE_BINARY_DIR}/include/mongoose )
endif()

###############################################################
# io_src files
###############################################################

set( IO_SRC
  ${CMAKE_BINARY_DIR}/temp_src/io_src/class_map.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_ABM_Integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_AttributesMap.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_BC635Clock.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_CheckPointAgent.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_CheckPointRestart.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Clock.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_CommandLineArguments.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_DRAscii.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_DRBinary.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_DRHDF5.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_DataRecordDispatcher.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_DataRecordGroup.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_EchoJobs.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_EnumAttributesMap.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Environment.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Euler_Cromer_Integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Euler_Integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Event.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_EventInstrument.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_EventManager.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_EventProcessor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Executive.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_ExecutiveException.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_ExternalApplication.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Flag.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_FrameDataRecordGroup.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_FrameLog.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_GetTimeOfDayClock.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_IPPython.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_IPPythonEvent.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_ITimer.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_InputProcessor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_InstrumentBase.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_IntegLoopManager.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_IntegLoopScheduler.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_IntegLoopSimObject.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_JITEvent.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_JITInputFile.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_JSONVariableServer.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_JSONVariableServerThread.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_JobData.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MM4_Integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MSConnect.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MSSharedMem.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MSSocket.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MTV.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MalfunctionsTrickView.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Master.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MemoryManager.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MessageCout.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MessageFile.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MessageLCout.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MessagePublisher.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MessageSubscriber.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MessageTCDevice.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MessageThreadedCout.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MonteCarlo.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MonteMonitor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_MonteVar.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_NL2_Integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_PlaybackFile.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_RK2_Integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_RK4_Integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_RKF45_Integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_RKF78_Integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_RKG4_Integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_RealtimeSync.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_RemoteShell.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_RtiEvent.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_RtiExec.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_RtiList.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_RtiStager.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_STLInterface.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_ScheduledJobQueue.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Scheduler.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Sie.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_SimControlPanel.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_SimObject.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_SimTime.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Slave.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_StripChart.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_TPROCTEClock.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_ThreadBase.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_ThreadTrigger.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Threads.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Timer.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_TrickView.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_UCFn.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_UdUnits.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Unit.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_UnitTest.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_UnitsMap.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_VariableServer.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_VariableServerListenThread.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_VariableServerReference.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_VariableServerThread.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_Zeroconf.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_attributes.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_dllist.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_io_alloc.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_lqueue.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_lstack.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_message_type.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_mm_error.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_ms_sim_mode.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_n_choose_m.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_parameter_types.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rand_generator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_reference.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_reference_frame.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_regula_falsi.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_sim_mode.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_tc.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_time_offset.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_trick_error_hndlr.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_tsm.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_units_conv.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_value.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_var.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_variable_server_sync_types.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_wave_form.cpp
)

set( ER7_UTILS_IO_SRC
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_abm4_first_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_abm4_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_abm4_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_base_integration_group.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_beeman_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_beeman_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_bogus_integration_controls.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_deletable.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_euler_first_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_euler_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_euler_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_first_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_generalized_position_derivative.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_integrable_object.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_integration_controls.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_integration_messages.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_integration_technique.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_integrator_constructor_factory.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_integrator_interface.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_integrator_result.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_integrator_result_merger.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_integrator_result_merger_container.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_left_quaternion_functions.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_mm4_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_mm4_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_nl2_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_nl2_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_position_verlet_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_position_verlet_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_priming_first_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_priming_integration_controls.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_priming_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_priming_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_ratio128.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rk2_heun_first_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rk2_heun_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rk2_heun_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rk2_midpoint_first_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rk2_midpoint_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rk2_midpoint_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rk4_first_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rk4_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rk4_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rk4_second_order_ode_integrator_base.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rkf45_first_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rkf45_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rkf45_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rkf78_first_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rkf78_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rkf78_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rkg4_first_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rkg4_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_rkg4_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_single_cycle_integration_controls.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_standard_integration_controls.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_state_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_symplectic_euler_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_symplectic_euler_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_time_interface.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_trick_first_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_trick_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_trick_second_order_ode_integrator.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_uint128.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_velocity_verlet_integrator_constructor.cpp
  ${CMAKE_BINARY_DIR}/temp_src/io_src/io_velocity_verlet_second_order_ode_integrator.cpp
)

set(ENV{TRICK_ICG_EXCLUDE} ${CMAKE_BINARY_DIR}/include/mongoose)
set(ICG_FLAGS -I${CMAKE_BINARY_DIR}/include -I${CMAKE_BINARY_DIR}/include/trick/compat -I${UDUNITS2_INCLUDES} -DTRICK_VER=${TRICK_MAJOR} -DUSE_ER7_UTILS_INTEGRATORS)
add_custom_command(OUTPUT ${IO_SRC} ${ER7_UTILS_IO_SRC}
  COMMAND TRICK_HOME=${CMAKE_BINARY_DIR} TRICK_ICG_EXCLUDE=${CMAKE_BINARY_DIR}/include/mongoose ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/trick-ICG -force -sim_services -m -n -o ${CMAKE_BINARY_DIR}/temp_src/io_src ${ICG_FLAGS} ${CMAKE_BINARY_DIR}/include/trick/files_to_ICG.hh
  DEPENDS trick-ICG
)

add_library( trick STATIC $<TARGET_OBJECTS:sim_services_objs> $<TARGET_OBJECTS:trick_utils_objs> ${IO_SRC})
target_include_directories( trick PUBLIC ${UDUNITS2_INCLUDES} )

add_library( er7_utils STATIC $<TARGET_OBJECTS:er7_utils_objs> ${ER7_UTILS_IO_SRC})

# fake dependency to avoid double ICG
add_dependencies(er7_utils trick)

###############################################################
# libtrick_pyip.a
###############################################################

set( TRICK_SWIG_SRC
  trick_source/trick_swig/PrimitiveAttributesMap
  trick_source/trick_swig/swig_convert_units
  trick_source/trick_swig/swig_global_vars
)

# Generated SWIG files
set( SWIG_SRC
  ${CMAKE_BINARY_DIR}/temp_src/swig/sim_services_wrap
  ${CMAKE_BINARY_DIR}/temp_src/swig/swig_double_wrap
  ${CMAKE_BINARY_DIR}/temp_src/swig/swig_int_wrap
  ${CMAKE_BINARY_DIR}/temp_src/swig//swig_ref_wrap
)

set( SWIG_SRC_BASENAME
  sim_services
  swig_double
  swig_int
  swig_ref
)

set(SWIG_FLAGS -DUSE_ER7_UTILS_INTEGRATORS)
if(GSL_FOUND)
    list( APPEND SWIG_FLAGS -D_HAVE_GSL )
endif()

if(USE_MONGOOSE)
  list( APPEND SWIG_FLAGS -DUSE_MONGOOSE)
  list( APPEND IO_SRC   
    ${CMAKE_BINARY_DIR}/temp_src/io_src/io_WebServer.cpp
    ${CMAKE_BINARY_DIR}/temp_src/io_src/io_WebSocketSession.cpp
  )
endif()

foreach ( infile ${SWIG_SRC_BASENAME} )
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/temp_src/swig/${infile}_wrap.cpp
      COMMAND ${SWIG_EXECUTABLE} ${SWIG_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR}/trick_source -I${CMAKE_CURRENT_SOURCE_DIR}/include -I${CMAKE_CURRENT_SOURCE_DIR} -c++ -python -includeall -ignoremissing -w201,362,389,451 -o $@ -outdir ${CMAKE_BINARY_DIR}/share/trick/swig ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_swig/${infile}.i
    )
endforeach(infile)

add_library( trick_pyip STATIC $<TARGET_OBJECTS:input_processor_objs> ${TRICK_SWIG_SRC} ${SWIG_SRC})
target_include_directories( trick_pyip PUBLIC ${PYTHON_INCLUDE_DIRS} )
target_include_directories( trick_pyip PUBLIC ${UDUNITS2_INCLUDES} )
if(GSL_FOUND)
    target_include_directories( trick_pyip PUBLIC ${GSL_INCLUDE_DIRS} )
endif()
target_include_directories( trick_pyip PUBLIC trick_source )

###############################################################
# libtrickHTTP.a
###############################################################
if(USE_MONGOOSE)
set( TRICKHTTP_SRC
  trick_source/web/HttpServer/src/VariableServerSession
  trick_source/web/HttpServer/src/VariableServerVariable
  trick_source/web/HttpServer/src/WebServer
  trick_source/web/HttpServer/src/http_GET_handlers
  trick_source/web/HttpServer/src/simpleJSON
)

add_library( trickHTTP STATIC ${TRICKHTTP_SRC})
add_dependencies( trickHTTP mongoose)
endif()
###############################################################
# Other Trick libraries
###############################################################

add_subdirectory(trick_source/codegen/Interface_Code_Gen)
add_subdirectory(trick_source/er7_utils)
add_subdirectory(trick_source/sim_services)
add_subdirectory(trick_source/trick_utils)
add_subdirectory(trick_source/java)
add_subdirectory(trick_source/data_products)
