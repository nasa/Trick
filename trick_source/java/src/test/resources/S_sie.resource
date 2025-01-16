<?xml version="1.0"?>

<sie>

  <class name="ManipUtils">
  </class>

  <class name="ManipKinemat">
    <member
      name="joint_u"
      type="double"
      io_attributes="15"
      units="--"
      description="axis of rotation for each joint">
      <dimension>0</dimension>
    </member>
    <member
      name="joint_q"
      type="double"
      io_attributes="15"
      units="rad"
      description="joint angle of each joint">
      <dimension>0</dimension>
    </member>
    <member
      name="joint_w"
      type="double"
      io_attributes="15"
      units="rad/s"
      description="joint rate of each joint">
      <dimension>0</dimension>
    </member>
    <member
      name="joint_l"
      type="double"
      io_attributes="15"
      units="m"
      description="link lengths of each joint">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="joint_ee"
      type="double"
      io_attributes="15"
      units="m"
      description="length from last joint to ee">
      <dimension>0</dimension>
    </member>
    <member
      name="P_task_base"
      type="double"
      io_attributes="15"
      units="m"
      description="vector task->base expressed in task">
      <dimension>0</dimension>
    </member>
    <member
      name="R_task_base"
      type="double"
      io_attributes="15"
      units="--"
      description="rotation matrix task->base">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="R_base_task"
      type="double"
      io_attributes="15"
      units="--"
      description="rotation matrix base->task">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="P_joint_joint"
      type="double"
      io_attributes="15"
      units="m"
      description="link_i+1->link_i in frame i">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="P_joint_ee"
      type="double"
      io_attributes="15"
      units="m"
      description="link_n-1 -> ee in frame n-1">
      <dimension>0</dimension>
    </member>
    <member
      name="P_task_joint"
      type="double"
      io_attributes="15"
      units="m"
      description="task->link_i in task frame">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="P_task_ee"
      type="double"
      io_attributes="15"
      units="m"
      description="task->ee in task frame">
      <dimension>0</dimension>
    </member>
    <member
      name="R_joint"
      type="double"
      io_attributes="15"
      units="--"
      description="rotation matrix frames i+1->i">
      <dimension>0</dimension>
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="R_joint_task"
      type="double"
      io_attributes="15"
      units="--"
      description="rotation matrix frame i+1-task">
      <dimension>0</dimension>
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="R_ee_joint"
      type="double"
      io_attributes="15"
      units="--"
      description="rotation matrix frame ee->n">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="R_ee_task"
      type="double"
      io_attributes="15"
      units="--"
      description="rotation matrix frame ee->task">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="V_joint_joint"
      type="double"
      io_attributes="15"
      units="m/s"
      description="velocity of frame i in i-1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="V_ee"
      type="double"
      io_attributes="15"
      units="m/s"
      description="vel of EE in task">
      <dimension>0</dimension>
    </member>
    <member
      name="V_joint_task"
      type="double"
      io_attributes="15"
      units="m/s"
      description="vel of joint i in task frame">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="jacobian"
      type="double"
      io_attributes="15"
      units="--"
      description="transform from angular to linear vel">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="jacInv"
      type="double"
      io_attributes="15"
      units="--"
      description="Inverse Jacobian">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="jacDet"
      type="double"
      io_attributes="15"
      units="--"
      description="Determinant of the Jacobian">
    </member>
    <member
      name="checkSingularities"
      type="bool"
      io_attributes="15"
      units="--"
      description="whether to check for kinematic singularities">
    </member>
    <member
      name="singularityTolerance"
      type="double"
      io_attributes="15"
      units="rad"
      description="tolerance for singularity approach">
    </member>
    <member
      name="utils"
      type="ManipUtils"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="ndof"
      type="int"
      io_attributes="15"
      units="--"
      description="number of degrees of freedom">
    </member>
  </class>

  <enumeration name="ControlFrame">
    <pair label ="Task" value="0"/>
    <pair label ="EE" value="1"/>
  </enumeration>

  <class name="ManipControl">
    <member
      name="manualFrame"
      type="ControlFrame"
      io_attributes="15"
      units="--"
      description="Frame for Manual Control">
    </member>
    <member
      name="singleJointNum"
      type="int"
      io_attributes="15"
      units="--"
      description="SingleJoint control joint selection">
    </member>
    <member
      name="singleJointRate"
      type="double"
      io_attributes="15"
      units="rad/s"
      description="SingleJoint control joint rate for selected joint">
    </member>
    <member
      name="manualCommandRate"
      type="double"
      io_attributes="15"
      units="m/s"
      description="command rate of EE in manual mode">
      <dimension>2</dimension>
    </member>
    <member
      name="desiredPos"
      type="double"
      io_attributes="15"
      units="m"
      description="Desired EE position in TaskFrame of EE for EEPos control mode">
      <dimension>2</dimension>
    </member>
    <member
      name="Kp"
      type="double"
      io_attributes="15"
      units="--"
      description="Proportional gain for EEPos tracking">
    </member>
    <member
      name="Kd"
      type="double"
      io_attributes="15"
      units="--"
      description="Derivative gain for EEPos tracking">
    </member>
    <member
      name="utils"
      type="ManipUtils"
      io_attributes="15"
      units="--"
      description="math utilities">
    </member>
    <member
      name="commandedJointRate"
      type="double"
      io_attributes="15"
      units="rad/s"
      description="calculated required joint rates">
      <dimension>2</dimension>
    </member>
    <member
      name="jacInv"
      type="double"
      io_attributes="15"
      units="--"
      description="inverse Jacobian as calculated by kinematics">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="R_ee_task"
      type="double"
      io_attributes="15"
      units="--"
      description="rot matrix from EE to task frame">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="ndof"
      type="int"
      io_attributes="15"
      units="--"
      description="number of degrees of freedom">
    </member>
    <member
      name="commandedEERate_Task"
      type="double"
      io_attributes="15"
      units="m/s"
      description="command rate of EE in Task frame">
      <dimension>2</dimension>
    </member>
    <member
      name="posEps"
      type="double"
      io_attributes="15"
      units="m"
      description="error tolerance for EEPosAuto convergence">
    </member>
  </class>

  <enumeration name="ControlMode">
    <pair label ="Nope" value="0"/>
    <pair label ="Cancel" value="1"/>
    <pair label ="SingleJoint" value="2"/>
    <pair label ="Manual" value="3"/>
    <pair label ="EEPos" value="4"/>
  </enumeration>

  <class name="PlanarManip">
    <member
      name="ndof"
      type="int"
      io_attributes="15"
      units="--"
      description="number of degrees of freedom">
    </member>
    <member
      name="mode"
      type="ControlMode"
      io_attributes="15"
      units="--"
      description="Control mode to use">
    </member>
    <member
      name="kinemat"
      type="ManipKinemat"
      io_attributes="15"
      units="--"
      description="class the calculates for/inv kinematics">
    </member>
    <member
      name="controller"
      type="ManipControl"
      io_attributes="15"
      units="--"
      description="controller class">
    </member>
  </class>

  <class name="SimEnvironment">
  </class>

  <class name="SysSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sched"
      type="Trick__Executive"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="MonteCarloSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="mc"
      type="Trick__MonteCarlo"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="MemoryManagerSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="mm"
      type="Trick__MemoryManager"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="CheckPointRestartSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="cpr"
      type="Trick__CheckPointRestart"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="SieSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sie"
      type="Trick__Sie"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="CommandLineArgumentsSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="cmd_args"
      type="Trick__CommandLineArguments"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="MessageSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="mpublisher"
      type="Trick__MessagePublisher"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="mcout"
      type="Trick__MessageCout"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="mtcout"
      type="Trick__MessageThreadedCout"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="mfile"
      type="Trick__MessageHSFile"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="mdevice"
      type="Trick__MessageTCDevice"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="pfile"
      type="Trick__PlaybackFile"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="message_file_manager"
      type="Trick__MessageCustomManager"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="JITSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="jit"
      type="Trick__JITInputFile"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="InputProcessorSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="ip"
      type="Trick__IPPython"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="mtv"
      type="Trick__MTV"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="ThreadProcessEventSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="ep"
      type="Trick__EventProcessor"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="EventManagerSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="em"
      type="Trick__EventManager"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="ep"
      type="Trick__EventProcessor"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="VariableServerSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="vs"
      type="Trick__VariableServer"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="sim_control_panel"
      type="Trick__SimControlPanel"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="trick_view"
      type="Trick__TrickView"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="malfunctions_trick_view"
      type="Trick__MalfunctionsTrickView"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="stripchart"
      type="Trick__StripChart"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="monte_monitor"
      type="Trick__MonteMonitor"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="DataRecordDispatcherSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="drd"
      type="Trick__DataRecordDispatcher"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="RTSyncSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="gtod_clock"
      type="Trick__GetTimeOfDayClock"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="itimer"
      type="Trick__ITimer"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="rt_sync"
      type="Trick__RealtimeSync"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="FrameLogSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="frame_log"
      type="Trick__FrameLog"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="MasterSlaveSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="master"
      type="Trick__Master"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="slave"
      type="Trick__Slave"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="InstrumentationSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="echo_jobs"
      type="Trick__EchoJobs"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="debug_pause"
      type="Trick__DebugPause"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="InjectorExecSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rtie"
      type="Trick__RtiExec"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="InjectorSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rtis"
      type="Trick__RtiStager"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="ZeroconfSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="zc"
      type="Trick__Zeroconf"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="UnitTestSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="unit_tests"
      type="Trick__UnitTest"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="UdUnitsSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="udunits"
      type="Trick__UdUnits"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="ManipulatorSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="robot"
      type="PlanarManip"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <top_level_object
    name="trick_sys"
    type="SysSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_mc"
    type="MonteCarloSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_mm"
    type="MemoryManagerSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_cpr"
    type="CheckPointRestartSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_sie"
    type="SieSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_cmd_args"
    type="CommandLineArgumentsSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_message"
    type="MessageSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_jit"
    type="JITSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_ip"
    type="InputProcessorSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_em"
    type="EventManagerSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_vs"
    type="VariableServerSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_data_record"
    type="DataRecordDispatcherSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_real_time"
    type="RTSyncSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_frame_log"
    type="FrameLogSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_master_slave"
    type="MasterSlaveSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_instruments"
    type="InstrumentationSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_inject"
    type="InjectorSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_zero_conf"
    type="ZeroconfSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_utest"
    type="UnitTestSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="trick_udunits"
    type="UdUnitsSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="Manip2D"
    type="ManipulatorSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <top_level_object
    name="armIntegLoop"
    type="IntegLoopSimObject"
    alloc_memory_init="1">
  </top_level_object>

  <class name="TRANSFORM">
    <member
      name="euler_sequence"
      type="Euler_Seq"
      io_attributes="15"
      units="--"
      description="Euler rotation sequence">
    </member>
    <member
      name="euler_angles"
      type="double"
      io_attributes="15"
      units="rad"
      description="Euler rot angles from Frame 1 to Frame 2">
      <dimension>3</dimension>
    </member>
    <member
      name="location"
      type="double"
      io_attributes="15"
      units="m"
      description="Frame 1 origin to Frame 2 origin vector, referenced to Frame 1">
      <dimension>3</dimension>
    </member>
  </class>

  <class name="Trick__Clock">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="clock_tics_per_sec"
      type="unsigned long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_clock_ratio"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sim_tic_ratio"
      type="double"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__GetTimeOfDayClock">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="clock_tics_per_sec"
      type="unsigned long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_clock_ratio"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sim_tic_ratio"
      type="double"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__CommandLineArguments">
    <member
      name="output_dir_timestamped"
      type="bool"
      io_attributes="15"
      units="--"
      description="record data into a separate timestamped directory">
    </member>
    <member
      name="default_dir"
      type="std__string"
      io_attributes="15"
      units="--"
      description="Directory of executable">
    </member>
    <member
      name="cmdline_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="Name of sim from the cmdline">
    </member>
    <member
      name="run_dir"
      type="std__string"
      io_attributes="15"
      units="--"
      description="Run directory name &quot;RUN_&lt;unique_tag>&quot;">
    </member>
    <member
      name="user_output_dir"
      type="std__string"
      io_attributes="15"
      units="--"
      description="user specified output dir">
    </member>
    <member
      name="time_stamp_dir"
      type="std__string"
      io_attributes="15"
      units="--"
      description="time stamp sub directory">
    </member>
    <member
      name="output_dir"
      type="std__string"
      io_attributes="15"
      units="--"
      description="Directory name to hols S_job_execution, chkpnt_* files, and the rt logging files">
    </member>
    <member
      name="input_file"
      type="std__string"
      io_attributes="15"
      units="--"
      description="Simulation input data file name">
    </member>
  </class>

  <class name="Trick__InstrumentBase">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="phase"
      type="unsigned short"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="target_job"
      type="Trick__JobData"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__JobData">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="complete"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="handled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="cycle"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="start"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="stop"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="job_class"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="job_class_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="system_job_class"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="phase"
      type="unsigned short"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="tags"
      type="std__set&lt;std__string>"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="thread"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sim_object_id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="cycle_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="start_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="stop_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="next_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__ScheduledJobQueue">
    <member
      name="list_size"
      type="unsigned int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="curr_index"
      type="unsigned int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="next_job_time"
      type="long long"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__SimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__Scheduler">
  </class>

  <class name="Trick__ThreadBase">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_priority"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__ThreadTriggerBase">
    <member
      name="tt_type"
      type="Trick__ThreadTriggerType"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__ThreadTriggerMutex">
    <member
      name="tt_type"
      type="Trick__ThreadTriggerType"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__ThreadTriggerFlag">
    <member
      name="tt_type"
      type="Trick__ThreadTriggerType"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__ThreadTriggerEventFD">
    <member
      name="tt_type"
      type="Trick__ThreadTriggerType"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__ThreadTriggerFutex">
    <member
      name="tt_type"
      type="Trick__ThreadTriggerType"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__ThreadTriggerContainer">
    <member
      name="trigger_type"
      type="Trick__ThreadTriggerType"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="ttMutex"
      type="Trick__ThreadTriggerMutex"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="ttFlag"
      type="Trick__ThreadTriggerFlag"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="ttEventFD"
      type="Trick__ThreadTriggerEventFD"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="ttFutex"
      type="Trick__ThreadTriggerFutex"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="ttBase"
      type="Trick__ThreadTriggerBase"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__Threads">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_priority"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="thread_id"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="curr_time_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="amf_cycle"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="amf_cycle_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="amf_next_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="running"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="trigger_container"
      type="Trick__ThreadTriggerContainer"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="shutdown_wait_async"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__Executive">
    <member
      name="attach_debugger"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="debugger_command"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="enable_freeze"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="stack_trace"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="trap_sigbus"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="trap_sigfpe"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="trap_sigsegv"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="trap_sigabrt"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="trap_sigchld"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="initialization_complete"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_nap"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="software_frame"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="software_frame_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="frame_count"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="freeze_frame_count"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="freeze_times"
      type="std__priority_queue&lt;long long, std__vector&lt;long long>, std__greater&lt;long long> >"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="freeze_frame"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="freeze_frame_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="next_frame_check_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="mode"
      type="SIM_MODE"
      io_attributes="5"
      units="--">
    </member>
    <member
      name="terminate_time"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="time_tic_value"
      type="int"
      io_attributes="15"
      units="1/s">
    </member>
    <member
      name="old_time_tic_value"
      type="int"
      io_attributes="15"
      units="1/s">
    </member>
    <member
      name="time_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="freeze_time_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="next_freeze_frame_check_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="freeze_command"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="freeze_on_frame_boundary"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sim_start"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="time_last_pass_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="job_call_time_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="num_classes"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="num_all_jobs"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="all_jobs_for_checkpoint"
      type="Trick__JobData"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
    </member>
    <member
      name="num_sim_objects"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sim_objects"
      type="std__vector&lt;Trick__SimObject *>"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="version_date_tag"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="build_date"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="current_version"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__ExecutiveException">
  </class>

  <class name="Trick__Environment">
  </class>

  <class name="ENUM_ATTR">
    <member
      name="label"
      type="char"
      io_attributes="15"
      units="--"
      description="Enumeration label">
      <dimension>0</dimension>
    </member>
    <member
      name="value"
      type="int"
      io_attributes="15"
      units="--"
      description="Enumeration value">
    </member>
    <member
      name="mods"
      type="int"
      io_attributes="15"
      units="--"
      description="bit 32 = deprecated var">
    </member>
  </class>

  <class name="INDEX">
    <member
      name="size"
      type="int"
      io_attributes="15"
      units="--"
      description="If ATTRIBUTES.type == TRICK_BITFIELD or ATTRIBUTES.type == TRICK_UNSIGNED_BITFIELD, then ATTRIBUTES.index.size[0] represents the number of bits in the bitfield. Otherwise: 0 indicates that the array is dynamic, i.e., may have any natural number of array elements. > 0 indicates that the array is fixed (constrained). The value specifies the maximum number of elements in the array.">
    </member>
    <member
      name="start"
      type="int"
      io_attributes="15"
      units="--"
      description="If ATTRIBUTES.type == TRICK_BITFIELD or ATTRIBUTES.type == TRICK_UNSIGNED_BITFIELD, then start represents the bit position (in the range 0:31) of the most significant bit of the bit field.">
    </member>
  </class>

  <class name="ATTRIBUTES_tag">
    <member
      name="name"
      type="char"
      io_attributes="15"
      units="--"
      description="Name of the variable of variable element of a structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="type_name"
      type="char"
      io_attributes="15"
      units="--"
      description="Character string representation of the C/C++ data type or user-defined type.">
      <dimension>0</dimension>
    </member>
    <member
      name="units"
      type="char"
      io_attributes="15"
      units="--"
      description="Units of measurement. Units string specification as described WHERE?">
      <dimension>0</dimension>
    </member>
    <member
      name="alias"
      type="char"
      io_attributes="15"
      units="--"
      description="OBSOLETE.">
      <dimension>0</dimension>
    </member>
    <member
      name="user_defined"
      type="char"
      io_attributes="15"
      units="--"
      description="OBSOLETE.">
      <dimension>0</dimension>
    </member>
    <member
      name="des"
      type="char"
      io_attributes="15"
      units="--"
      description="OBSOLETE. Parameter description">
      <dimension>0</dimension>
    </member>
    <member
      name="io"
      type="int"
      io_attributes="15"
      units="--"
      description="Input / Output processing restrictions, Each bit controls different I/O access. Bit 31 TRICK_VAR_OUTPUT - Allows output through attribute based capabilities Bit 30 TRICK_VAR_INPUT - Allows input through attribute based capabilities Bit 29 TRICK_CHKPNT_OUTPUT - Allows checkpoint output Bit 28 TRICK_CHKPNT_OUTPUT - Allows checkpoint input">
    </member>
    <member
      name="type"
      type="TRICK_TYPE"
      io_attributes="15"
      units="--"
      description="TRICK data type.">
    </member>
    <member
      name="size"
      type="int"
      io_attributes="15"
      units="--"
      description="Data type size in bytes">
    </member>
    <member
      name="range_min"
      type="double"
      io_attributes="15"
      units="--"
      description="Minimum allowable value for parameter">
    </member>
    <member
      name="range_max"
      type="double"
      io_attributes="15"
      units="--"
      description="Minimum allowable value for parameter">
    </member>
    <member
      name="language"
      type="Language"
      io_attributes="15"
      units="--"
      description="Native language of parameter.">
    </member>
    <member
      name="mods"
      type="int"
      io_attributes="15"
      units="--"
      description="Modification bits. bit 0 = c++ reference var bit 1 = c++ static var bit 2 = &quot;--&quot; units bit 3-8: if c++ reference bit is set, width of reference attribute. else not set. Why: because `size` is the size of the type, not the reference reference is the size of a pointer because of compiler implementations, but is not enforced by C++ standard (could change). bit 31 = deprecated var">
    </member>
    <member
      name="offset"
      type="long"
      io_attributes="15"
      units="--"
      description="Offset in bytes of this parameter from beginning of data structure">
    </member>
    <member
      name="num_index"
      type="int"
      io_attributes="15"
      units="--"
      description="Number of array indices for the parameter. May have values in the range 0..8. 0 = not arrayed. 1..8 = number of array dimensions. NOTE: TRICK_BITFIELD and TRICK_UNSIGNED_BITFIELD are not considered to be arrays so num_index is 0 for these types. Even so, for these two types index[0] contains bitfield information.">
    </member>
    <member
      name="index"
      type="INDEX"
      io_attributes="15"
      units="--"
      description="An array of array-index information or bit-field information. Is only meaningful if num_index > 0 or if type is a bit field type.">
      <dimension>8</dimension>
    </member>
  </class>

  <class name="ATTRIBUTES">
    <member
      name="name"
      type="char"
      io_attributes="15"
      units="--"
      description="Name of the variable of variable element of a structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="type_name"
      type="char"
      io_attributes="15"
      units="--"
      description="Character string representation of the C/C++ data type or user-defined type.">
      <dimension>0</dimension>
    </member>
    <member
      name="units"
      type="char"
      io_attributes="15"
      units="--"
      description="Units of measurement. Units string specification as described WHERE?">
      <dimension>0</dimension>
    </member>
    <member
      name="alias"
      type="char"
      io_attributes="15"
      units="--"
      description="OBSOLETE.">
      <dimension>0</dimension>
    </member>
    <member
      name="user_defined"
      type="char"
      io_attributes="15"
      units="--"
      description="OBSOLETE.">
      <dimension>0</dimension>
    </member>
    <member
      name="des"
      type="char"
      io_attributes="15"
      units="--"
      description="OBSOLETE. Parameter description">
      <dimension>0</dimension>
    </member>
    <member
      name="io"
      type="int"
      io_attributes="15"
      units="--"
      description="Input / Output processing restrictions, Each bit controls different I/O access. Bit 31 TRICK_VAR_OUTPUT - Allows output through attribute based capabilities Bit 30 TRICK_VAR_INPUT - Allows input through attribute based capabilities Bit 29 TRICK_CHKPNT_OUTPUT - Allows checkpoint output Bit 28 TRICK_CHKPNT_OUTPUT - Allows checkpoint input">
    </member>
    <member
      name="type"
      type="TRICK_TYPE"
      io_attributes="15"
      units="--"
      description="TRICK data type.">
    </member>
    <member
      name="size"
      type="int"
      io_attributes="15"
      units="--"
      description="Data type size in bytes">
    </member>
    <member
      name="range_min"
      type="double"
      io_attributes="15"
      units="--"
      description="Minimum allowable value for parameter">
    </member>
    <member
      name="range_max"
      type="double"
      io_attributes="15"
      units="--"
      description="Minimum allowable value for parameter">
    </member>
    <member
      name="language"
      type="Language"
      io_attributes="15"
      units="--"
      description="Native language of parameter.">
    </member>
    <member
      name="mods"
      type="int"
      io_attributes="15"
      units="--"
      description="Modification bits. bit 0 = c++ reference var bit 1 = c++ static var bit 2 = &quot;--&quot; units bit 3-8: if c++ reference bit is set, width of reference attribute. else not set. Why: because `size` is the size of the type, not the reference reference is the size of a pointer because of compiler implementations, but is not enforced by C++ standard (could change). bit 31 = deprecated var">
    </member>
    <member
      name="offset"
      type="long"
      io_attributes="15"
      units="--"
      description="Offset in bytes of this parameter from beginning of data structure">
    </member>
    <member
      name="num_index"
      type="int"
      io_attributes="15"
      units="--"
      description="Number of array indices for the parameter. May have values in the range 0..8. 0 = not arrayed. 1..8 = number of array dimensions. NOTE: TRICK_BITFIELD and TRICK_UNSIGNED_BITFIELD are not considered to be arrays so num_index is 0 for these types. Even so, for these two types index[0] contains bitfield information.">
    </member>
    <member
      name="index"
      type="INDEX"
      io_attributes="15"
      units="--"
      description="An array of array-index information or bit-field information. Is only meaningful if num_index > 0 or if type is a bit field type.">
      <dimension>8</dimension>
    </member>
  </class>

  <class name="ENUM_LIST">
    <member
      name="type_name"
      type="char"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="E"
      type="ENUM_ATTR"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="NAME_TO_ATTR">
    <member
      name="name"
      type="char"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="attr"
      type="ATTRIBUTES_tag"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="size"
      type="int"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="EQUIV_ATTRIBUTES">
  </class>

  <class name="VALUE">
    <member
      name="ll"
      type="long long"
      io_attributes="15"
      units="--"
      description="long long">
    </member>
  </class>

  <class name="V_DATA">
    <member
      name="type"
      type="TRICK_TYPE"
      io_attributes="15"
      units="--"
      description="value type">
    </member>
    <member
      name="value"
      type="VALUE"
      io_attributes="15"
      units="--"
      description="value">
    </member>
  </class>

  <class name="VALUE_LIST">
    <member
      name="num_val"
      type="int"
      io_attributes="15"
      units="--"
      description="Number of values">
    </member>
    <member
      name="v_data"
      type="V_DATA"
      io_attributes="15"
      units="1">
      <dimension>512</dimension>
    </member>
  </class>

  <class name="v_tree">
    <member
      name="last"
      type="v_tree"
      io_attributes="15"
      units="--"
      description="last element in the current list">
      <dimension>0</dimension>
    </member>
    <member
      name="next"
      type="v_tree"
      io_attributes="15"
      units="--"
      description="next element in the current list">
      <dimension>0</dimension>
    </member>
    <member
      name="down"
      type="v_tree"
      io_attributes="15"
      units="--"
      description="subordinate list of the current element. If non-NULL then &quot;v_data&quot; should be NULL.">
      <dimension>0</dimension>
    </member>
    <member
      name="v_data"
      type="V_DATA"
      io_attributes="15"
      units="--"
      description="data of the current element. If non-NULL then &quot;down&quot; should be NULL.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="V_TREE">
    <member
      name="last"
      type="v_tree"
      io_attributes="15"
      units="--"
      description="last element in the current list">
      <dimension>0</dimension>
    </member>
    <member
      name="next"
      type="v_tree"
      io_attributes="15"
      units="--"
      description="next element in the current list">
      <dimension>0</dimension>
    </member>
    <member
      name="down"
      type="v_tree"
      io_attributes="15"
      units="--"
      description="subordinate list of the current element. If non-NULL then &quot;v_data&quot; should be NULL.">
      <dimension>0</dimension>
    </member>
    <member
      name="v_data"
      type="V_DATA"
      io_attributes="15"
      units="--"
      description="data of the current element. If non-NULL then &quot;down&quot; should be NULL.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="_DLLNODE">
    <member
      name="prev"
      type="_DLLNODE"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="next"
      type="_DLLNODE"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="DLLNODE">
    <member
      name="prev"
      type="_DLLNODE"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="next"
      type="_DLLNODE"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="_DLLIST">
    <member
      name="count"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="head"
      type="_DLLNODE"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="tail"
      type="_DLLNODE"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="compare"
      type="int"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="DLLIST">
    <member
      name="count"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="head"
      type="_DLLNODE"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="tail"
      type="_DLLNODE"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="compare"
      type="int"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="ADDRESS_OPERAND">
    <member
      name="offset"
      type="long"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="ADDRESS_NODE">
    <member
      name="operator_"
      type="ADDRESS_OPERATOR"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="operand"
      type="ADDRESS_OPERAND"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="REF2">
    <member
      name="reference"
      type="char"
      io_attributes="15"
      units="--"
      description="Main buffer for variable reference">
      <dimension>0</dimension>
    </member>
    <member
      name="num_index"
      type="int"
      io_attributes="15"
      units="--"
      description="num index for last level of reference">
    </member>
    <member
      name="units"
      type="char"
      io_attributes="15"
      units="--"
      description="Units as specified in input runstream">
      <dimension>0</dimension>
    </member>
    <member
      name="num_index_left"
      type="int"
      io_attributes="15"
      units="--"
      description="FIXME OBSOLETE Number of remaining indicies to specify">
    </member>
    <member
      name="pointer_present"
      type="int"
      io_attributes="15"
      units="--"
      description="0 = no , 1 = yes (address could change)">
    </member>
    <member
      name="ref_type"
      type="int"
      io_attributes="15"
      units="--"
      description="0 = address, 1 = value.">
    </member>
    <member
      name="attr"
      type="ATTRIBUTES_tag"
      io_attributes="15"
      units="--"
      description="Parameter attributes">
      <dimension>0</dimension>
    </member>
    <member
      name="ref_attr"
      type="ATTRIBUTES_tag"
      io_attributes="15"
      units="--"
      description="Dynamically allocated reference attribute.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="VAR_DECLARE">
  </class>

  <class name="VAR_LIST">
  </class>

  <class name="ALLOC_INFO">
  </class>

  <class name="Trick__VarNameElement">
    <member
      name="type"
      type="Trick__VarNameElementType"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="index"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__CheckPointAgent">
  </class>

  <class name="Trick__ALLOC_INFO_MAP">
  </class>

  <class name="Trick__ALLOC_INFO_MAP_ITER">
  </class>

  <class name="Trick__VARIABLE_MAP">
  </class>

  <class name="Trick__VARIABLE_MAP_ITER">
  </class>

  <class name="Trick__ENUMERATION_MAP">
  </class>

  <class name="Trick__MemoryManager">
    <member
      name="debug_level"
      type="int"
      io_attributes="15"
      units="--"
      description="Debug level">
    </member>
    <member
      name="restore_stls_default"
      type="bool"
      io_attributes="15"
      units="--"
      description="true = restore STL variables on checkpoint restore if user does not specify option. false = don't">
    </member>
    <member
      name="local_anon_var_prefix"
      type="char"
      io_attributes="15"
      units="--"
      description="Temporary-variable-name prefix.">
      <dimension>0</dimension>
    </member>
    <member
      name="extern_anon_var_prefix"
      type="char"
      io_attributes="15"
      units="--"
      description="Temporary-variable-name prefix.">
      <dimension>0</dimension>
    </member>
    <member
      name="reduced_checkpoint"
      type="bool"
      io_attributes="15"
      units="--"
      description="true = Don't write zero valued variables in the checkpoint. false= Write all values.">
    </member>
    <member
      name="hexfloat_checkpoint"
      type="bool"
      io_attributes="15"
      units="--"
      description="true = Represent floating point values as hexidecimal to preserve precision. false= Normal.">
    </member>
    <member
      name="expanded_arrays"
      type="bool"
      io_attributes="15"
      units="--"
      description="true = array element values are set in separate assignments.">
    </member>
    <member
      name="resetting_memory"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__Event">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="active"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="thread_id"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="before_after"
      type="Trick__EventBeforeAfter"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="cycle"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="cycle_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="next_tics"
      type="long long"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="free_on_removal"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="target_name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="target_inst"
      type="unsigned int"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__CompareEventPtrs">
  </class>

  <class name="Trick__EventProcessor">
  </class>

  <class name="Trick__EventInstrument">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="phase"
      type="unsigned short"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="target_job"
      type="Trick__JobData"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="event"
      type="Trick__Event"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__EventManager">
    <member
      name="active_events"
      type="Trick__Event"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="num_active_events"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="num_allocated"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__InputProcessor">
    <member
      name="verify_input"
      type="int"
      io_attributes="15"
      units="--"
      description="verify input">
    </member>
    <member
      name="input_file"
      type="std__string"
      io_attributes="15"
      units="--"
      description="Simulation input data file name">
    </member>
  </class>

  <class name="Trick__IPPython">
    <member
      name="verify_input"
      type="int"
      io_attributes="15"
      units="--"
      description="verify input">
    </member>
    <member
      name="input_file"
      type="std__string"
      io_attributes="15"
      units="--"
      description="Simulation input data file name">
    </member>
    <member
      name="units_conversion_msgs"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__condition_t">
    <member
      name="enabled"
      type="char"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="hold"
      type="char"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="fired"
      type="char"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="fired_count"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="fired_time"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="str"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="comment"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="cond_type"
      type="int"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__action_t">
    <member
      name="enabled"
      type="char"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="ran"
      type="char"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="ran_count"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="ran_time"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="str"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="comment"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="act_type"
      type="int"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__IPPythonEvent">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="active"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="thread_id"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="before_after"
      type="Trick__EventBeforeAfter"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="cycle"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="cycle_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="next_tics"
      type="long long"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="free_on_removal"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="target_name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="target_inst"
      type="unsigned int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="added"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="is_user_event"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="manual"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="manual_fired"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="cond_all"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="fired"
      type="char"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="hold"
      type="char"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="ran"
      type="char"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="condition_count"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="fired_count"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="fired_time"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="action_count"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="ran_count"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="ran_time"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="condition_list"
      type="Trick__condition_t"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="action_list"
      type="Trick__action_t"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__MTV">
    <member
      name="dummy_event"
      type="Trick__IPPythonEvent"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="mtv_list"
      type="Trick__IPPythonEvent"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="mtv_count"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="mtv_update_ticker"
      type="int"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__JITEvent">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="active"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="thread_id"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="before_after"
      type="Trick__EventBeforeAfter"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="cycle"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="cycle_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="next_tics"
      type="long long"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="free_on_removal"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="target_name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="target_inst"
      type="unsigned int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="func_name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__JITLibInfo">
    <member
      name="library_name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__JITInputFile">
    <member
      name="input_file"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="file_to_libinfo_map"
      type="std__map&lt;std__string, JITLibInfo>"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="_TrickErrorHndlr">
    <member
      name="report_level"
      type="TrickErrorLevel"
      io_attributes="15"
      units="--"
      description="Err reporting level">
    </member>
    <member
      name="use_send_hs"
      type="int"
      io_attributes="15"
      units="--"
      description="1=send_hs; else fprintf">
      <dimension>9</dimension>
    </member>
  </class>

  <class name="TrickErrorHndlr">
    <member
      name="report_level"
      type="TrickErrorLevel"
      io_attributes="15"
      units="--"
      description="Err reporting level">
    </member>
    <member
      name="use_send_hs"
      type="int"
      io_attributes="15"
      units="--"
      description="1=send_hs; else fprintf">
      <dimension>9</dimension>
    </member>
  </class>

  <class name="TCDevice">
    <member
      name="disabled"
      type="TCCommFlag"
      io_attributes="15"
      units="--"
      description="Connection enabled flag">
    </member>
    <member
      name="disable_handshaking"
      type="TCCommFlag"
      io_attributes="15"
      units="--"
      description="Set this to skip handshaking in tc_connect and tc_accept.">
    </member>
    <member
      name="blockio_limit"
      type="double"
      io_attributes="15"
      units="s"
      description="Block I/O timeout limit">
    </member>
    <member
      name="blockio_type"
      type="TCCommBlocking"
      io_attributes="15"
      units="--"
      description="E.g. spinloops">
    </member>
    <member
      name="client_id"
      type="int"
      io_attributes="15"
      units="--"
      description="client id, other side. Set by tc_accept.">
    </member>
    <member
      name="client_tag"
      type="char"
      io_attributes="15"
      units="--"
      description="client tag, other side. User specified. Defaults if unspecified.">
      <dimension>80</dimension>
    </member>
    <member
      name="hostname"
      type="char"
      io_attributes="15"
      units="--"
      description="Hostname of server/client. User specified.">
      <dimension>0</dimension>
    </member>
    <member
      name="port"
      type="int"
      io_attributes="15"
      units="--"
      description="TCP/IP Port number. User specified.">
    </member>
    <member
      name="mcast_group"
      type="char"
      io_attributes="15"
      units="--"
      description="Multicast group address. User specified.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="SEND_ME">
  </class>

  <class name="BC_INFO">
  </class>

  <class name="Trick__JSONVariableServer">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_priority"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="source_address"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="port"
      type="unsigned short"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="user_port_requested"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__JSONVariableServerThread">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_priority"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__MSConnect">
    <member
      name="sync_wait_limit"
      type="double"
      io_attributes="15"
      units="s">
    </member>
  </class>

  <class name="Trick__SlaveInfo">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="activated"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="slave_type"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="remote_shell"
      type="Trick__RemoteShell"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="user_remote_shell"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="remote_shell_args"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="machine_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="machine_display"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sim_path"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="S_main_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="chkpnt_dump_auto"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="chkpnt_load_auto"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="chkpnt_name"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>256</dimension>
    </member>
    <member
      name="run_input_file"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="other_args"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sync_error_terminate"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sync_wait_limit"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="reconnect_wait_limit"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="connection"
      type="Trick__MSConnect"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__Master">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="num_slaves"
      type="int"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="MonteCarloVariable">
    <member
      name="units"
      type="std__string"
      io_attributes="15"
      units="--"
      description="optional setting in the case where the specified values are in units different from the native units of the variable. These are the units associated with the specified-value.">
    </member>
    <member
      name="include_in_summary"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag telling MonteCarloMaster whether to include this variable in the dispersion summary file. The default depends on the type of variable but is generally true.">
    </member>
    <member
      name="variable_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the sim-variable being assigned by this instance.">
    </member>
    <member
      name="assignment"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The value assigned to the variable. Used in MonteCarloMaster to generate the dispersion summary file.">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="--"
      description="the command that gets pushed to the monte_input input file.">
    </member>
    <member
      name="type"
      type="MonteCarloVariable__MonteCarloVariableType"
      io_attributes="15"
      units="--"
      description="Broad categorization of types of MonteCarloVariable. This is set in the constructor of the specific classes derived from MonteCarloVariable and provides information to the MonteCarloMaster about what general type of variable it is dealing with.">
    </member>
  </class>

  <class name="MonteCarloVariableFile">
    <member
      name="units"
      type="std__string"
      io_attributes="15"
      units="--"
      description="optional setting in the case where the specified values are in units different from the native units of the variable. These are the units associated with the specified-value.">
    </member>
    <member
      name="include_in_summary"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag telling MonteCarloMaster whether to include this variable in the dispersion summary file. The default depends on the type of variable but is generally true.">
    </member>
    <member
      name="variable_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the sim-variable being assigned by this instance.">
    </member>
    <member
      name="assignment"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The value assigned to the variable. Used in MonteCarloMaster to generate the dispersion summary file.">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="--"
      description="the command that gets pushed to the monte_input input file.">
    </member>
    <member
      name="type"
      type="MonteCarloVariable__MonteCarloVariableType"
      io_attributes="15"
      units="--"
      description="Broad categorization of types of MonteCarloVariable. This is set in the constructor of the specific classes derived from MonteCarloVariable and provides information to the MonteCarloMaster about what general type of variable it is dealing with.">
    </member>
    <member
      name="max_skip"
      type="unsigned long"
      io_attributes="15"
      units="--"
      description="The maximum number of lines to skip in a file. This defaults to 0, indicating that the file will be read sequentially.">
    </member>
    <member
      name="is_dependent"
      type="bool"
      io_attributes="15"
      units="--"
      description="A flag indicating that this instance is dependent upon another instance for reading the file &quot;filename&quot;. Only one instance will read each filename. Default: false.">
    </member>
    <member
      name="filename"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the file containing the data for this variable. Assigned at construction.">
    </member>
    <member
      name="column_number"
      type="unsigned long"
      io_attributes="15"
      units="--"
      description="The column number indicating from where in the data file the data for this variable should be extracted.">
    </member>
    <member
      name="first_column_number"
      type="unsigned long"
      io_attributes="15"
      units="--"
      description="Usually used to distinguish between whether the first column should be identified with a 0 or 1, but extensible to other integers as well. Default: 1.">
    </member>
    <member
      name="dependents"
      type="std__list&lt;MonteCarloVariableFile *>"
      io_attributes="15"
      units="--"
      description="A list of MonteVarVariableFile instances that use the same file as this one. This list is only populated if this instance was the first registered to use this file.">
    </member>
  </class>

  <class name="MonteCarloPythonLineExec">
    <member
      name="units"
      type="std__string"
      io_attributes="15"
      units="--"
      description="optional setting in the case where the specified values are in units different from the native units of the variable. These are the units associated with the specified-value.">
    </member>
    <member
      name="include_in_summary"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag telling MonteCarloMaster whether to include this variable in the dispersion summary file. The default depends on the type of variable but is generally true.">
    </member>
    <member
      name="variable_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the sim-variable being assigned by this instance.">
    </member>
    <member
      name="assignment"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The value assigned to the variable. Used in MonteCarloMaster to generate the dispersion summary file.">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="--"
      description="the command that gets pushed to the monte_input input file.">
    </member>
    <member
      name="type"
      type="MonteCarloVariable__MonteCarloVariableType"
      io_attributes="15"
      units="--"
      description="Broad categorization of types of MonteCarloVariable. This is set in the constructor of the specific classes derived from MonteCarloVariable and provides information to the MonteCarloMaster about what general type of variable it is dealing with.">
    </member>
    <member
      name="instruction_set"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The right-hand-side of an equation that gets inserted into the monte-input file and looks like: &lt;variable_name> = &lt;instruction-set>">
    </member>
    <member
      name="instruction_is_command"
      type="bool"
      io_attributes="15"
      units="--"
      description="Indicates whether to implement a command that looks like: - variable=instruction vs variable representing the standalone command, in which case variable_name and instruction_set are identical.">
    </member>
  </class>

  <class name="MonteCarloPythonFileExec">
    <member
      name="units"
      type="std__string"
      io_attributes="15"
      units="--"
      description="optional setting in the case where the specified values are in units different from the native units of the variable. These are the units associated with the specified-value.">
    </member>
    <member
      name="include_in_summary"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag telling MonteCarloMaster whether to include this variable in the dispersion summary file. The default depends on the type of variable but is generally true.">
    </member>
    <member
      name="variable_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the sim-variable being assigned by this instance.">
    </member>
    <member
      name="assignment"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The value assigned to the variable. Used in MonteCarloMaster to generate the dispersion summary file.">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="--"
      description="the command that gets pushed to the monte_input input file.">
    </member>
    <member
      name="type"
      type="MonteCarloVariable__MonteCarloVariableType"
      io_attributes="15"
      units="--"
      description="Broad categorization of types of MonteCarloVariable. This is set in the constructor of the specific classes derived from MonteCarloVariable and provides information to the MonteCarloMaster about what general type of variable it is dealing with.">
    </member>
  </class>

  <class name="MonteCarloVariableRandom">
    <member
      name="units"
      type="std__string"
      io_attributes="15"
      units="--"
      description="optional setting in the case where the specified values are in units different from the native units of the variable. These are the units associated with the specified-value.">
    </member>
    <member
      name="include_in_summary"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag telling MonteCarloMaster whether to include this variable in the dispersion summary file. The default depends on the type of variable but is generally true.">
    </member>
    <member
      name="variable_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the sim-variable being assigned by this instance.">
    </member>
    <member
      name="assignment"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The value assigned to the variable. Used in MonteCarloMaster to generate the dispersion summary file.">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="--"
      description="the command that gets pushed to the monte_input input file.">
    </member>
    <member
      name="type"
      type="MonteCarloVariable__MonteCarloVariableType"
      io_attributes="15"
      units="--"
      description="Broad categorization of types of MonteCarloVariable. This is set in the constructor of the specific classes derived from MonteCarloVariable and provides information to the MonteCarloMaster about what general type of variable it is dealing with.">
    </member>
    <member
      name="seed_m"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="the value used to seed the generator.">
    </member>
  </class>

  <class name="MonteCarloVariableFixed">
    <member
      name="units"
      type="std__string"
      io_attributes="15"
      units="--"
      description="optional setting in the case where the specified values are in units different from the native units of the variable. These are the units associated with the specified-value.">
    </member>
    <member
      name="include_in_summary"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag telling MonteCarloMaster whether to include this variable in the dispersion summary file. The default depends on the type of variable but is generally true.">
    </member>
    <member
      name="variable_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the sim-variable being assigned by this instance.">
    </member>
    <member
      name="assignment"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The value assigned to the variable. Used in MonteCarloMaster to generate the dispersion summary file.">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="--"
      description="the command that gets pushed to the monte_input input file.">
    </member>
    <member
      name="type"
      type="MonteCarloVariable__MonteCarloVariableType"
      io_attributes="15"
      units="--"
      description="Broad categorization of types of MonteCarloVariable. This is set in the constructor of the specific classes derived from MonteCarloVariable and provides information to the MonteCarloMaster about what general type of variable it is dealing with.">
    </member>
    <member
      name="var_type"
      type="int"
      io_attributes="15"
      units="--"
      description="0: double 1: integer 2: string;">
    </member>
  </class>

  <class name="MonteCarloVariableSemiFixed">
    <member
      name="units"
      type="std__string"
      io_attributes="15"
      units="--"
      description="optional setting in the case where the specified values are in units different from the native units of the variable. These are the units associated with the specified-value.">
    </member>
    <member
      name="include_in_summary"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag telling MonteCarloMaster whether to include this variable in the dispersion summary file. The default depends on the type of variable but is generally true.">
    </member>
    <member
      name="variable_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the sim-variable being assigned by this instance.">
    </member>
    <member
      name="assignment"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The value assigned to the variable. Used in MonteCarloMaster to generate the dispersion summary file.">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="--"
      description="the command that gets pushed to the monte_input input file.">
    </member>
    <member
      name="type"
      type="MonteCarloVariable__MonteCarloVariableType"
      io_attributes="15"
      units="--"
      description="Broad categorization of types of MonteCarloVariable. This is set in the constructor of the specific classes derived from MonteCarloVariable and provides information to the MonteCarloMaster about what general type of variable it is dealing with.">
    </member>
    <member
      name="seed_variable"
      type="MonteCarloVariable"
      io_attributes="3"
      units="--"
      description="A reference to another MonteCarloVariable; the value of *this* variable is taken from the value of this seed-variable during preparation of the first monte-input file.">
    </member>
    <member
      name="command_generated"
      type="bool"
      io_attributes="15"
      units="--"
      description="flag indicating the fixed command has been generated.">
    </member>
  </class>

  <class name="MonteCarloMaster">
    <member
      name="active"
      type="bool"
      io_attributes="15"
      units="--"
      description="The main active flag determining whether an input file should be processed for a monte-carlo run. This flag is used to manage the configuration of the scenario, including things like which variables to log.">
    </member>
    <member
      name="generate_dispersions"
      type="bool"
      io_attributes="15"
      units="--"
      description="This flag controls whether the variables should be loaded and dispersed. It has no effect if the active flag is off. False: Configure the run for MC; this configuration typically uses a previously-generated monte_input.py file; it does not read in MC variables and does not generate new monte_input files. True: Use this execution of the sim to read in the MC variables and generating the monte-input files. After doing so, the execution will terminate. The sim can then be re-run using one of the new monte_input files. Default: true. This is set to false in the monte_input.py files to allow the base input file to be processed from within the monte-input file without regenerting the monte-input files.">
    </member>
    <member
      name="run_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the scenario, used in generating the &quot;MONTE_&lt;run_name>&quot; directory, which contains all of the runs.">
    </member>
    <member
      name="monte_dir"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the MONTE&lt;RUN...> directory relative to the SIM directory where runs will be generated">
    </member>
    <member
      name="input_file_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the original file used as the main input file. Default: input.py.">
    </member>
    <member
      name="generate_meta_data"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag indicating whether to generate the meta-data output file.">
    </member>
    <member
      name="generate_summary"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag indicating whether to generate the dispersion summary file.">
    </member>
    <member
      name="minimum_padding"
      type="int"
      io_attributes="15"
      units="--"
      description="The minimum width of the run-number field, e.g. RUN_1 vs RUN_00001; The run-number field will be sized to the minimum of this value or the width necessary to accommodate the highest number. Defaults to 0.">
    </member>
    <member
      name="monte_run_number"
      type="unsigned long"
      io_attributes="15"
      units="--"
      description="A unique identifying number for each run.">
    </member>
    <member
      name="input_files_prepared"
      type="bool"
      io_attributes="15"
      units="--"
      description="Internal flag indicating that the input files have been generated and waiting for execution. Effectively blocks further modifications to variables after this flag has been set.">
    </member>
    <member
      name="location"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The location in the main sim by which this instance may be addressed in an input file.">
    </member>
    <member
      name="variables"
      type="std__list&lt;MonteCarloVariable *>"
      io_attributes="15"
      units="--"
      description="A STL-list of pointers to instances of all the base-class MonteVarVariable instances. Note that this has to be a list of pointers rather than instances because the actual instances are polymorphic; making a copy would restrict them to be actual MonteCarloVariable instances and we need the polymorphic capabilities for generating the monte_input command.">
    </member>
    <member
      name="num_runs"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="The number of runs to execute for this scenario.">
    </member>
    <member
      name="file_list"
      type="std__list&lt;std__pair&lt;std__string, MonteCarloVariableFile *> >"
      io_attributes="15"
      units="--"
      description="A list of filenames being read as part of the MonteVarFile variables being managed by this class.">
    </member>
  </class>

  <class name="MonteCarloVariableRandomUniform">
    <member
      name="units"
      type="std__string"
      io_attributes="15"
      units="--"
      description="optional setting in the case where the specified values are in units different from the native units of the variable. These are the units associated with the specified-value.">
    </member>
    <member
      name="include_in_summary"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag telling MonteCarloMaster whether to include this variable in the dispersion summary file. The default depends on the type of variable but is generally true.">
    </member>
    <member
      name="variable_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the sim-variable being assigned by this instance.">
    </member>
    <member
      name="assignment"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The value assigned to the variable. Used in MonteCarloMaster to generate the dispersion summary file.">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="--"
      description="the command that gets pushed to the monte_input input file.">
    </member>
    <member
      name="type"
      type="MonteCarloVariable__MonteCarloVariableType"
      io_attributes="15"
      units="--"
      description="Broad categorization of types of MonteCarloVariable. This is set in the constructor of the specific classes derived from MonteCarloVariable and provides information to the MonteCarloMaster about what general type of variable it is dealing with.">
    </member>
    <member
      name="seed_m"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="the value used to seed the generator.">
    </member>
  </class>

  <class name="MonteCarloVariableRandomUniformInt">
    <member
      name="units"
      type="std__string"
      io_attributes="15"
      units="--"
      description="optional setting in the case where the specified values are in units different from the native units of the variable. These are the units associated with the specified-value.">
    </member>
    <member
      name="include_in_summary"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag telling MonteCarloMaster whether to include this variable in the dispersion summary file. The default depends on the type of variable but is generally true.">
    </member>
    <member
      name="variable_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the sim-variable being assigned by this instance.">
    </member>
    <member
      name="assignment"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The value assigned to the variable. Used in MonteCarloMaster to generate the dispersion summary file.">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="--"
      description="the command that gets pushed to the monte_input input file.">
    </member>
    <member
      name="type"
      type="MonteCarloVariable__MonteCarloVariableType"
      io_attributes="15"
      units="--"
      description="Broad categorization of types of MonteCarloVariable. This is set in the constructor of the specific classes derived from MonteCarloVariable and provides information to the MonteCarloMaster about what general type of variable it is dealing with.">
    </member>
    <member
      name="seed_m"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="the value used to seed the generator.">
    </member>
  </class>

  <class name="MonteCarloVariableRandomStringSet">
    <member
      name="units"
      type="std__string"
      io_attributes="15"
      units="--"
      description="optional setting in the case where the specified values are in units different from the native units of the variable. These are the units associated with the specified-value.">
    </member>
    <member
      name="include_in_summary"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag telling MonteCarloMaster whether to include this variable in the dispersion summary file. The default depends on the type of variable but is generally true.">
    </member>
    <member
      name="variable_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the sim-variable being assigned by this instance.">
    </member>
    <member
      name="assignment"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The value assigned to the variable. Used in MonteCarloMaster to generate the dispersion summary file.">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="--"
      description="the command that gets pushed to the monte_input input file.">
    </member>
    <member
      name="type"
      type="MonteCarloVariable__MonteCarloVariableType"
      io_attributes="15"
      units="--"
      description="Broad categorization of types of MonteCarloVariable. This is set in the constructor of the specific classes derived from MonteCarloVariable and provides information to the MonteCarloMaster about what general type of variable it is dealing with.">
    </member>
    <member
      name="seed_m"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="the value used to seed the generator.">
    </member>
    <member
      name="values"
      type="std__vector&lt;std__string>"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="MonteCarloVariableRandomBool">
    <member
      name="units"
      type="std__string"
      io_attributes="15"
      units="--"
      description="optional setting in the case where the specified values are in units different from the native units of the variable. These are the units associated with the specified-value.">
    </member>
    <member
      name="include_in_summary"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag telling MonteCarloMaster whether to include this variable in the dispersion summary file. The default depends on the type of variable but is generally true.">
    </member>
    <member
      name="variable_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the sim-variable being assigned by this instance.">
    </member>
    <member
      name="assignment"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The value assigned to the variable. Used in MonteCarloMaster to generate the dispersion summary file.">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="--"
      description="the command that gets pushed to the monte_input input file.">
    </member>
    <member
      name="type"
      type="MonteCarloVariable__MonteCarloVariableType"
      io_attributes="15"
      units="--"
      description="Broad categorization of types of MonteCarloVariable. This is set in the constructor of the specific classes derived from MonteCarloVariable and provides information to the MonteCarloMaster about what general type of variable it is dealing with.">
    </member>
    <member
      name="seed_m"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="the value used to seed the generator.">
    </member>
    <member
      name="values"
      type="std__vector&lt;std__string>"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="MonteCarloVariableRandomNormal">
    <member
      name="units"
      type="std__string"
      io_attributes="15"
      units="--"
      description="optional setting in the case where the specified values are in units different from the native units of the variable. These are the units associated with the specified-value.">
    </member>
    <member
      name="include_in_summary"
      type="bool"
      io_attributes="15"
      units="--"
      description="Flag telling MonteCarloMaster whether to include this variable in the dispersion summary file. The default depends on the type of variable but is generally true.">
    </member>
    <member
      name="variable_name"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The name of the sim-variable being assigned by this instance.">
    </member>
    <member
      name="assignment"
      type="std__string"
      io_attributes="15"
      units="--"
      description="The value assigned to the variable. Used in MonteCarloMaster to generate the dispersion summary file.">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="--"
      description="the command that gets pushed to the monte_input input file.">
    </member>
    <member
      name="type"
      type="MonteCarloVariable__MonteCarloVariableType"
      io_attributes="15"
      units="--"
      description="Broad categorization of types of MonteCarloVariable. This is set in the constructor of the specific classes derived from MonteCarloVariable and provides information to the MonteCarloMaster about what general type of variable it is dealing with.">
    </member>
    <member
      name="seed_m"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="the value used to seed the generator.">
    </member>
    <member
      name="max_num_tries"
      type="unsigned long"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="min_value"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="max_value"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="truncated_low"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="truncated_high"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__Slave">
    <member
      name="activated"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="reconnected"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sent_reconnect_cmd"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="msg_published"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="chkpnt_name"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>256</dimension>
    </member>
    <member
      name="sync_error_terminate"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sync_wait_limit"
      type="double"
      io_attributes="15"
      units="s">
    </member>
  </class>

  <class name="Trick__MSSocket">
    <member
      name="sync_wait_limit"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="tc_dev"
      type="TCDevice"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sync_port_tag"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="TSMDevice">
    <member
      name="size"
      type="int"
      io_attributes="15"
      units="--"
      description="Size of shared memory segment">
    </member>
    <member
      name="key_file"
      type="char"
      io_attributes="15"
      units="--"
      description="An existing file name to use in generating key (ftok) for shared memory">
      <dimension>256</dimension>
    </member>
    <member
      name="timeout_limit"
      type="double"
      io_attributes="15"
      units="--"
      description="s Optional: how long do we wait for shared memory data before we time out">
    </member>
    <member
      name="default_val"
      type="int"
      io_attributes="15"
      units="--"
      description="Optional: value to set shared memory data to after it's been read">
    </member>
    <member
      name="use_lock"
      type="char"
      io_attributes="15"
      units="--"
      description="Optional: if true then tsm_init creates pthread read/write lock">
    </member>
  </class>

  <class name="Trick__MSSharedMemData">
    <member
      name="master_pid"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="master_time"
      type="long long"
      io_attributes="15"
      units="1">
      <dimension>5</dimension>
    </member>
    <member
      name="master_time_front"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="master_time_back"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="master_command"
      type="MS_SIM_COMMAND"
      io_attributes="15"
      units="1">
      <dimension>5</dimension>
    </member>
    <member
      name="master_command_front"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="master_command_back"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="slave_command"
      type="MS_SIM_COMMAND"
      io_attributes="15"
      units="1">
      <dimension>5</dimension>
    </member>
    <member
      name="slave_command_front"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="slave_command_back"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="slave_port"
      type="int"
      io_attributes="15"
      units="--"
      description="slave's checkpoint port">
    </member>
    <member
      name="chkpnt_name"
      type="char"
      io_attributes="15"
      units="--"
      description="checkpoint dir/filename">
      <dimension>256</dimension>
    </member>
  </class>

  <class name="Trick__MSSharedMem">
    <member
      name="sync_wait_limit"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="tsm_dev"
      type="TSMDevice"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="shm_addr"
      type="Trick__MSSharedMemData"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__MessageSubscriber">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="color"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__MessageCout">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="color"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__SysThread">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_priority"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__MessageThreadedCout">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="color"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="MessageSubscriber::name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="SysThread::ThreadBase::name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_priority"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="max_buffer_items"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="max_buffer_size"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="color_code"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="print_immediate"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__MessageFile">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="color"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="file_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__MessageHSFile">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="color"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="file_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__MessageCustomFile">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="color"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="file_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="_this_level"
      type="int"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__MessageCustomManager">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="color"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="_custom_message_files"
      type="std__vector&lt;MessageCustomFile *>"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__MessageLCout">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="color"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="subs"
      type="std__map&lt;char, std__string>"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="seed"
      type="unsigned int"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__MessagePublisher">
    <member
      name="subscribers"
      type="std__list&lt;MessageSubscriber *>"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="tics_per_sec"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="num_digits"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="print_format"
      type="char"
      io_attributes="15"
      units="1">
      <dimension>64</dimension>
    </member>
    <member
      name="sim_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__MessageTCDeviceListenThread">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_priority"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__MessageTCDevice">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="color"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="port"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="listen_thread"
      type="Trick__MessageTCDeviceListenThread"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__PlaybackFile">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="color"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="file_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__MonteVar">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="unit"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="value"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__MonteRun">
    <member
      name="id"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="num_tries"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="start_time"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="end_time"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="variables"
      type="std__vector&lt;std__string>"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="exit_status"
      type="Trick__MonteRun__ExitStatus"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__MonteSlave">
    <member
      name="id"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="Trick__MonteSlave__State"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="machine_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="port"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="current_run"
      type="Trick__MonteRun"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
    </member>
    <member
      name="num_dispatches"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="num_results"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="cpu_time"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="remote_shell"
      type="Trick__RemoteShell"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="user_remote_shell"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="remote_shell_args"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="multiplier"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sim_path"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="S_main_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__MonteRange">
    <member
      name="start"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="end"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__MonteCarlo">
    <member
      name="slave_sim_options"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="slave_output_directory"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dry_run"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="localhost_as_remote"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="custom_slave_dispatch"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="timeout"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="max_tries"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="user_cmd_string"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="custom_pre_text"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="custom_post_text"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="verbosity"
      type="Trick__MonteCarlo__Verbosity"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="listen_device"
      type="TCDevice"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="connection_device"
      type="TCDevice"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="num_slaves"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="slaves_head"
      type="Trick__MonteSlave"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="current_run"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="num_runs"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="actual_num_runs"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="num_results"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="start_time"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="end_time"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="master_port"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="slave_id"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="machine_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="run_directory"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="master_init_queue"
      type="Trick__ScheduledJobQueue"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="master_pre_queue"
      type="Trick__ScheduledJobQueue"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="master_post_queue"
      type="Trick__ScheduledJobQueue"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="master_shutdown_queue"
      type="Trick__ScheduledJobQueue"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="slave_init_queue"
      type="Trick__ScheduledJobQueue"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="slave_pre_queue"
      type="Trick__ScheduledJobQueue"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="slave_post_queue"
      type="Trick__ScheduledJobQueue"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="slave_shutdown_queue"
      type="Trick__ScheduledJobQueue"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__Timer">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="active"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__RealtimeSync">
    <member
      name="enable_flag"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="disable_flag"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="active"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="freeze_frame"
      type="long long"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="freeze_time_tics"
      type="long long"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="frame_overrun_cnt"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="total_overrun"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_max_overrun_cnt"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_max_overrun_time"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="rt_max_overrun_time_tics"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_overrun_freeze"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="frame_sched_time"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="frame_overrun_time"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="last_clock_time"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="tics_per_sec"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="align_sim_to_wall_clock"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="align_tic_mult"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_clock"
      type="Trick__Clock"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
    </member>
    <member
      name="default_clock"
      type="Trick__Clock"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
    </member>
    <member
      name="sleep_timer"
      type="Trick__Timer"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
    </member>
    <member
      name="freeze_shutdown"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sim_start_time"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sim_end_init_time"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="sim_end_time"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__ITimer">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="active"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__VariableReference">
    <member
      name="_var_info"
      type="REF2"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="_size"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="_deref"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="_trick_type"
      type="TRICK_TYPE"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="_staged"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_write_ready"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_base_units"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_requested_units"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__ClientConnection">
    <member
      name="_connection_type"
      type="Trick__ClientConnection__ConnectionType"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_client_tag"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__VariableServerSession">
    <member
      name="_time"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="_info_msg"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_session_log_msg_stream"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_send_stdio"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_validate_address"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_instance_num"
      type="int"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="SystemInterface">
  </class>

  <class name="Trick__TCPConnection">
    <member
      name="_connection_type"
      type="Trick__ClientConnection__ConnectionType"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_client_tag"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_socket"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_connected"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_listen_socket"
      type="int"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__TCPClientListener">
    <member
      name="_listen_socket"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_hostname"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_port"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_client_tag"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_initialized"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__VariableServerSessionThread">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_priority"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="_saved_pause_cmd"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__UDPConnection">
    <member
      name="_connection_type"
      type="Trick__ClientConnection__ConnectionType"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_client_tag"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_initialized"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_started"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_socket"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_port"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_hostname"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_remote_serv_addr"
      type="sockaddr_in"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__MulticastGroup">
    <member
      name="_connection_type"
      type="Trick__ClientConnection__ConnectionType"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_client_tag"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="MulticastGroup::UDPConnection::_initialized"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_started"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_socket"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_port"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_hostname"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_remote_serv_addr"
      type="sockaddr_in"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="_system_interface"
      type="SystemInterface"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__VariableServerListenThread">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_priority"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="_requested_source_address"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="_requested_port"
      type="unsigned short"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="_user_requested_address"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="_user_tag"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="_broadcast"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__VariableServer">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="info_msg"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="log"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="session_log"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="listen_thread"
      type="Trick__VariableServerListenThread"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="REGULA_FALSI">
    <member
      name="lower_set"
      type="int"
      io_attributes="15"
      units="--"
      description="Yes = Lower bounds has been set">
    </member>
    <member
      name="upper_set"
      type="int"
      io_attributes="15"
      units="--"
      description="Yes = Upper bounds has been set">
    </member>
    <member
      name="iterations"
      type="int"
      io_attributes="15"
      units="--"
      description="Total num of iterations used to fire event">
    </member>
    <member
      name="fires"
      type="int"
      io_attributes="15"
      units="--"
      description="Total num of times event fired">
    </member>
    <member
      name="x_lower"
      type="double"
      io_attributes="15"
      units="--"
      description="Lower value from error func">
    </member>
    <member
      name="t_lower"
      type="double"
      io_attributes="15"
      units="s"
      description="Time at lower val of err func">
    </member>
    <member
      name="x_upper"
      type="double"
      io_attributes="15"
      units="--"
      description="Upper value from err function">
    </member>
    <member
      name="t_upper"
      type="double"
      io_attributes="15"
      units="s"
      description="Time at upper value of err func">
    </member>
    <member
      name="delta_time"
      type="double"
      io_attributes="15"
      units="s"
      description="Time to go">
    </member>
    <member
      name="error"
      type="double"
      io_attributes="15"
      units="--"
      description="New Error value from err func">
    </member>
    <member
      name="last_error"
      type="double"
      io_attributes="15"
      units="--"
      description="Last pass error value">
    </member>
    <member
      name="last_tgo"
      type="double"
      io_attributes="15"
      units="s"
      description="Last pass Time to go value">
    </member>
    <member
      name="error_tol"
      type="double"
      io_attributes="15"
      units="--"
      description="Tolerance on error below which tgo=0">
    </member>
    <member
      name="mode"
      type="Regula_Falsi"
      io_attributes="15"
      units="--"
      description="Operating mode for regula falsi">
    </member>
    <member
      name="function_slope"
      type="Regula_Falsi"
      io_attributes="15"
      units="--"
      description="Operating mode for regula falsi">
    </member>
  </class>

  <class name="Trick__Integrator">
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="er7_utils__Er7UtilsDeletable">
  </class>

  <class name="er7_utils__GeneralizedPositionDerivativeFunctions">
  </class>

  <class name="er7_utils__GeneralizedPositionStepFunctions">
  </class>

  <class name="er7_utils__Integration">
  </class>

  <class name="er7_utils__IntegratorConstructor">
  </class>

  <class name="er7_utils__PrimingIntegratorConstructor">
    <member
      name="primer_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n Constructor used to create the primers.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RK4IntegratorConstructor">
  </class>

  <class name="er7_utils__IntegratorInterface">
  </class>

  <class name="er7_utils__IntegrationControls">
    <member
      name="time_scale_factor"
      type="double"
      io_attributes="15"
      units="--"
      description="n Ratio of dynamic delta time to simulation engine delta time.">
    </member>
    <member
      name="integ_starttime"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation time of the start of the current integration loop as requested by the simulation engine.">
    </member>
    <member
      name="integ_simdt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation time span of the current integration loop as requested by the simulation engine.">
    </member>
    <member
      name="integ_dyndt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The dynamic time span corresponding to integ_simdt.">
    </member>
    <member
      name="integ_time_scale"
      type="double"
      io_attributes="15"
      units="--"
      description="n The time scale factor, with 0.0 representing the start of the integration interval and 1.0 representing the end of the interval.">
    </member>
    <member
      name="integ_simtime"
      type="double"
      io_attributes="15"
      units="--"
      description="n The current simulation time.">
    </member>
    <member
      name="transition_table"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n A number_stages array that directs the transitions from one integration stage to the next. The constructor for this class populates this array with values 1 (element 0) to number_stages (element number_stages-1).">
      <dimension>0</dimension>
    </member>
    <member
      name="step_number"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Starts at zero at the start of a tour, increments by one per stage, and resets to zero at the end of a tour.">
    </member>
    <member
      name="cycle_stage"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The technique-specific finite state machine state number of the current stage. The cycle_stage starts at zero, nominally incrementing by one for each cycle, until it attains a value equal to the final_stage. At this point the cycle begins anew with a cycle_stage of zero.">
    </member>
    <member
      name="final_stage"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The stage number at which the cycle_stage resets to zero.">
    </member>
    <member
      name="number_stages"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the transition table, nominally final_stage.">
    </member>
    <member
      name="reset_needed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Wen set, indicates that a reset needs to be performed at the start of the next integration cycle. The flag will be cleared when the reset is performed.">
    </member>
  </class>

  <class name="er7_utils__SingleCycleIntegrationControls">
    <member
      name="time_scale_factor"
      type="double"
      io_attributes="15"
      units="--"
      description="n Ratio of dynamic delta time to simulation engine delta time.">
    </member>
    <member
      name="integ_starttime"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation time of the start of the current integration loop as requested by the simulation engine.">
    </member>
    <member
      name="integ_simdt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation time span of the current integration loop as requested by the simulation engine.">
    </member>
    <member
      name="integ_dyndt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The dynamic time span corresponding to integ_simdt.">
    </member>
    <member
      name="integ_time_scale"
      type="double"
      io_attributes="15"
      units="--"
      description="n The time scale factor, with 0.0 representing the start of the integration interval and 1.0 representing the end of the interval.">
    </member>
    <member
      name="integ_simtime"
      type="double"
      io_attributes="15"
      units="--"
      description="n The current simulation time.">
    </member>
    <member
      name="transition_table"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n A number_stages array that directs the transitions from one integration stage to the next. The constructor for this class populates this array with values 1 (element 0) to number_stages (element number_stages-1).">
      <dimension>0</dimension>
    </member>
    <member
      name="step_number"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Starts at zero at the start of a tour, increments by one per stage, and resets to zero at the end of a tour.">
    </member>
    <member
      name="cycle_stage"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The technique-specific finite state machine state number of the current stage. The cycle_stage starts at zero, nominally incrementing by one for each cycle, until it attains a value equal to the final_stage. At this point the cycle begins anew with a cycle_stage of zero.">
    </member>
    <member
      name="final_stage"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The stage number at which the cycle_stage resets to zero.">
    </member>
    <member
      name="number_stages"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the transition table, nominally final_stage.">
    </member>
    <member
      name="reset_needed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Wen set, indicates that a reset needs to be performed at the start of the next integration cycle. The flag will be cleared when the reset is performed.">
    </member>
  </class>

  <class name="er7_utils__StateIntegratorInterface">
  </class>

  <class name="er7_utils__IntegratorResult">
    <member
      name="time_scale"
      type="double"
      io_attributes="15"
      units="--"
      description="n Fraction of the integration interval time step by which the time should be advanced. A value of 0.0 means time should be set to the start of the integration interval; a value of 1.0 means time should be advanced to the end of the interval.">
    </member>
    <member
      name="failure_mode"
      type="int"
      io_attributes="15"
      units="--"
      description="n Zero indicates the integrator successfully reached the target stage. A non-zero value indicates some technique-dependent failure.">
    </member>
    <member
      name="merge_count"
      type="int"
      io_attributes="15"
      units="--"
      description="n The number of times another IntegratorResult has been merged into this IntegratorResult. A value of zero means this is a virgin result.">
    </member>
  </class>

  <class name="er7_utils__FirstOrderODEIntegrator">
    <member
      name="state_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the state.">
    </member>
  </class>

  <class name="er7_utils__RK4FirstOrderODEIntegrator">
    <member
      name="state_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the state.">
    </member>
    <member
      name="init_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n State derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__SecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
  </class>

  <class name="er7_utils__RK4SecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="saved_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position after stage 1.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RK4SimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="saved_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position after stage 1.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RK4GeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="saved_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position after stage 1.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RK4GeneralizedStepSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="saved_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position after stage 1.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__ABM4IntegratorConstructor">
    <member
      name="primer_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n Constructor used to create the primers.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__StandardIntegrationControls">
    <member
      name="time_scale_factor"
      type="double"
      io_attributes="15"
      units="--"
      description="n Ratio of dynamic delta time to simulation engine delta time.">
    </member>
    <member
      name="integ_starttime"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation time of the start of the current integration loop as requested by the simulation engine.">
    </member>
    <member
      name="integ_simdt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation time span of the current integration loop as requested by the simulation engine.">
    </member>
    <member
      name="integ_dyndt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The dynamic time span corresponding to integ_simdt.">
    </member>
    <member
      name="integ_time_scale"
      type="double"
      io_attributes="15"
      units="--"
      description="n The time scale factor, with 0.0 representing the start of the integration interval and 1.0 representing the end of the interval.">
    </member>
    <member
      name="integ_simtime"
      type="double"
      io_attributes="15"
      units="--"
      description="n The current simulation time.">
    </member>
    <member
      name="transition_table"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n A number_stages array that directs the transitions from one integration stage to the next. The constructor for this class populates this array with values 1 (element 0) to number_stages (element number_stages-1).">
      <dimension>0</dimension>
    </member>
    <member
      name="step_number"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Starts at zero at the start of a tour, increments by one per stage, and resets to zero at the end of a tour.">
    </member>
    <member
      name="cycle_stage"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The technique-specific finite state machine state number of the current stage. The cycle_stage starts at zero, nominally incrementing by one for each cycle, until it attains a value equal to the final_stage. At this point the cycle begins anew with a cycle_stage of zero.">
    </member>
    <member
      name="final_stage"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The stage number at which the cycle_stage resets to zero.">
    </member>
    <member
      name="number_stages"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the transition table, nominally final_stage.">
    </member>
    <member
      name="reset_needed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Wen set, indicates that a reset needs to be performed at the start of the next integration cycle. The flag will be cleared when the reset is performed.">
    </member>
    <member
      name="cycle_starttime"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation engine time of the start of the current integration cycle as determined by start_integration_tour or end_integration_cycle. For single cycle integrators such as RK4, the cycle_starttime and cycle_simdt are equal to the integ_starttime and integ_simdt. Multi-cycle integrators such as Gauss-Jackson compute the cycle_starttime and cycle_simdt.">
    </member>
    <member
      name="cycle_simdt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation engine time span of the current integration cycle as determined by start_integration_tour or end_integration_cycle.">
    </member>
    <member
      name="cycle_dyndt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The JEOD dynamic time span corresponding to cycle_simdt.">
    </member>
    <member
      name="multi_cycle"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Always false for single-cycle integrators. Multi-cycle integrators must set this to true to operate in multi-cycle mode.">
    </member>
    <member
      name="alt_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n An alternate controls, typically for priming, that acts as a stand-in for this integration controller.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__PrimingIntegrationControls">
    <member
      name="time_scale_factor"
      type="double"
      io_attributes="15"
      units="--"
      description="n Ratio of dynamic delta time to simulation engine delta time.">
    </member>
    <member
      name="integ_starttime"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation time of the start of the current integration loop as requested by the simulation engine.">
    </member>
    <member
      name="integ_simdt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation time span of the current integration loop as requested by the simulation engine.">
    </member>
    <member
      name="integ_dyndt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The dynamic time span corresponding to integ_simdt.">
    </member>
    <member
      name="integ_time_scale"
      type="double"
      io_attributes="15"
      units="--"
      description="n The time scale factor, with 0.0 representing the start of the integration interval and 1.0 representing the end of the interval.">
    </member>
    <member
      name="integ_simtime"
      type="double"
      io_attributes="15"
      units="--"
      description="n The current simulation time.">
    </member>
    <member
      name="transition_table"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n A number_stages array that directs the transitions from one integration stage to the next. The constructor for this class populates this array with values 1 (element 0) to number_stages (element number_stages-1).">
      <dimension>0</dimension>
    </member>
    <member
      name="step_number"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Starts at zero at the start of a tour, increments by one per stage, and resets to zero at the end of a tour.">
    </member>
    <member
      name="cycle_stage"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The technique-specific finite state machine state number of the current stage. The cycle_stage starts at zero, nominally incrementing by one for each cycle, until it attains a value equal to the final_stage. At this point the cycle begins anew with a cycle_stage of zero.">
    </member>
    <member
      name="final_stage"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The stage number at which the cycle_stage resets to zero.">
    </member>
    <member
      name="number_stages"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the transition table, nominally final_stage.">
    </member>
    <member
      name="reset_needed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Wen set, indicates that a reset needs to be performed at the start of the next integration cycle. The flag will be cleared when the reset is performed.">
    </member>
    <member
      name="cycle_starttime"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation engine time of the start of the current integration cycle as determined by start_integration_tour or end_integration_cycle. For single cycle integrators such as RK4, the cycle_starttime and cycle_simdt are equal to the integ_starttime and integ_simdt. Multi-cycle integrators such as Gauss-Jackson compute the cycle_starttime and cycle_simdt.">
    </member>
    <member
      name="cycle_simdt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation engine time span of the current integration cycle as determined by start_integration_tour or end_integration_cycle.">
    </member>
    <member
      name="cycle_dyndt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The JEOD dynamic time span corresponding to cycle_simdt.">
    </member>
    <member
      name="multi_cycle"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Always false for single-cycle integrators. Multi-cycle integrators must set this to true to operate in multi-cycle mode.">
    </member>
    <member
      name="alt_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n An alternate controls, typically for priming, that acts as a stand-in for this integration controller.">
      <dimension>0</dimension>
    </member>
    <member
      name="priming_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object used to prime the state integrators.">
      <dimension>0</dimension>
    </member>
    <member
      name="priming_count"
      type="int"
      io_attributes="15"
      units="--"
      description="n The number of priming integration cycles the integrator to go. This is typically reset to history_length-1.">
    </member>
    <member
      name="history_length"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The number of items in the state integrator's history buffer.">
    </member>
  </class>

  <class name="er7_utils__PrimingFirstOrderODEIntegrator">
    <member
      name="state_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the state.">
    </member>
    <member
      name="controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that controls this state integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="primer"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The state integrator used to prime this integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="priming_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The number of start of cycle derivatives needed by the technique.">
    </member>
    <member
      name="prime_counter"
      type="int"
      io_attributes="15"
      units="--"
      description="n Priming phase countdown. This is set to priming_state_size initially and upon reset. Priming is complete when the counter reaches zero.">
    </member>
    <member
      name="primed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Indicates whether the object has been primed. This is false initially and upon reset; it is set to true when priming is complete.">
    </member>
  </class>

  <class name="er7_utils__ABM4FirstOrderODEIntegrator">
    <member
      name="state_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the state.">
    </member>
    <member
      name="controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that controls this state integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="primer"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The state integrator used to prime this integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="priming_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The number of start of cycle derivatives needed by the technique.">
    </member>
    <member
      name="prime_counter"
      type="int"
      io_attributes="15"
      units="--"
      description="n Priming phase countdown. This is set to priming_state_size initially and upon reset. Priming is complete when the counter reaches zero.">
    </member>
    <member
      name="primed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Indicates whether the object has been primed. This is false initially and upon reset; it is set to true when priming is complete.">
    </member>
    <member
      name="init_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n The state at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n The state derivatives at the start of current and at the starts of the three previous integration cycles. This array is rotated so that deriv_hist[0] contains the state derivatives at the start of the current cycle, deriv_hist[1] the derivatives at the start of the previous cycle, and so on.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__PrimingSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that controls this state integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="primer"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The state integrator used to prime this integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="priming_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The number of start of cycle derivatives needed by the technique.">
    </member>
    <member
      name="prime_counter"
      type="int"
      io_attributes="15"
      units="--"
      description="n Priming phase countdown. This is set to priming_state_size initially and upon reset. Priming is complete when the counter reaches zero.">
    </member>
    <member
      name="primed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Indicates whether the object has been primed. This is false initially and upon reset; it is set to true when priming is complete.">
    </member>
  </class>

  <class name="er7_utils__ABM4SecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that controls this state integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="primer"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The state integrator used to prime this integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="priming_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The number of start of cycle derivatives needed by the technique.">
    </member>
    <member
      name="prime_counter"
      type="int"
      io_attributes="15"
      units="--"
      description="n Priming phase countdown. This is set to priming_state_size initially and upon reset. Priming is complete when the counter reaches zero.">
    </member>
    <member
      name="primed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Indicates whether the object has been primed. This is false initially and upon reset; it is set to true when priming is complete.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the current time step.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist_anchor"
      type="double"
      io_attributes="15"
      units="--"
      description="n Cached pointer to the data needed for deletion after PDH is rotated">
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist_anchor"
      type="double"
      io_attributes="15"
      units="--"
      description="n Cached pointer to the data needed for deletion after VDH is rotated">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__ABM4SimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that controls this state integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="primer"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The state integrator used to prime this integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="priming_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The number of start of cycle derivatives needed by the technique.">
    </member>
    <member
      name="prime_counter"
      type="int"
      io_attributes="15"
      units="--"
      description="n Priming phase countdown. This is set to priming_state_size initially and upon reset. Priming is complete when the counter reaches zero.">
    </member>
    <member
      name="primed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Indicates whether the object has been primed. This is false initially and upon reset; it is set to true when priming is complete.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the current time step.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist_anchor"
      type="double"
      io_attributes="15"
      units="--"
      description="n Cached pointer to the data needed for deletion after PDH is rotated">
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist_anchor"
      type="double"
      io_attributes="15"
      units="--"
      description="n Cached pointer to the data needed for deletion after VDH is rotated">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__ABM4GeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that controls this state integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="primer"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The state integrator used to prime this integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="priming_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The number of start of cycle derivatives needed by the technique.">
    </member>
    <member
      name="prime_counter"
      type="int"
      io_attributes="15"
      units="--"
      description="n Priming phase countdown. This is set to priming_state_size initially and upon reset. Priming is complete when the counter reaches zero.">
    </member>
    <member
      name="primed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Indicates whether the object has been primed. This is false initially and upon reset; it is set to true when priming is complete.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the current time step.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist_anchor"
      type="double"
      io_attributes="15"
      units="--"
      description="n Cached pointer to the data needed for deletion after PDH is rotated">
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist_anchor"
      type="double"
      io_attributes="15"
      units="--"
      description="n Cached pointer to the data needed for deletion after VDH is rotated">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__BaseIntegrationGroup">
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__TimeInterface">
  </class>

  <class name="er7_utils__IntegrationMessages">
    <member
      name="unsupported_option"
      type="char"
      io_attributes="15"
      units="--"
      description="n Issued when some user input is invalid.">
      <dimension>0</dimension>
    </member>
    <member
      name="invalid_item"
      type="char"
      io_attributes="15"
      units="--"
      description="n Issued when an item is somehow invalid; a duplicate entry for example.">
      <dimension>0</dimension>
    </member>
    <member
      name="internal_error"
      type="char"
      io_attributes="15"
      units="--"
      description="n Issued when the ER7_UTILS programmer messed up.">
      <dimension>0</dimension>
    </member>
    <member
      name="invalid_request"
      type="char"
      io_attributes="15"
      units="--"
      description="n Issued when a non-ER7_UTILS programmer messed up.">
      <dimension>0</dimension>
    </member>
    <member
      name="information"
      type="char"
      io_attributes="15"
      units="--"
      description="n Issued in non-error messages.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__BogusIntegrationControls">
    <member
      name="time_scale_factor"
      type="double"
      io_attributes="15"
      units="--"
      description="n Ratio of dynamic delta time to simulation engine delta time.">
    </member>
    <member
      name="integ_starttime"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation time of the start of the current integration loop as requested by the simulation engine.">
    </member>
    <member
      name="integ_simdt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The simulation time span of the current integration loop as requested by the simulation engine.">
    </member>
    <member
      name="integ_dyndt"
      type="double"
      io_attributes="15"
      units="--"
      description="n The dynamic time span corresponding to integ_simdt.">
    </member>
    <member
      name="integ_time_scale"
      type="double"
      io_attributes="15"
      units="--"
      description="n The time scale factor, with 0.0 representing the start of the integration interval and 1.0 representing the end of the interval.">
    </member>
    <member
      name="integ_simtime"
      type="double"
      io_attributes="15"
      units="--"
      description="n The current simulation time.">
    </member>
    <member
      name="transition_table"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n A number_stages array that directs the transitions from one integration stage to the next. The constructor for this class populates this array with values 1 (element 0) to number_stages (element number_stages-1).">
      <dimension>0</dimension>
    </member>
    <member
      name="step_number"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Starts at zero at the start of a tour, increments by one per stage, and resets to zero at the end of a tour.">
    </member>
    <member
      name="cycle_stage"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The technique-specific finite state machine state number of the current stage. The cycle_stage starts at zero, nominally incrementing by one for each cycle, until it attains a value equal to the final_stage. At this point the cycle begins anew with a cycle_stage of zero.">
    </member>
    <member
      name="final_stage"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The stage number at which the cycle_stage resets to zero.">
    </member>
    <member
      name="number_stages"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the transition table, nominally final_stage.">
    </member>
    <member
      name="reset_needed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Wen set, indicates that a reset needs to be performed at the start of the next integration cycle. The flag will be cleared when the reset is performed.">
    </member>
  </class>

  <class name="er7_utils__TrickIntegrator">
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_derivs"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of derivatives used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State vector used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of accelerations used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_velocity"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of velocities used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_position"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of positions used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv2"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv2.">
      <dimension>0</dimension>
    </member>
    <member
      name="trick_integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor used to generate the state integrators this object.">
      <dimension>0</dimension>
    </member>
    <member
      name="first_order_integrator"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a first order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="second_order_integrator"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a second order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="buf_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="half_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="integ_mode"
      type="er7_utils__TrickIntegrator__IntegrationMode"
      io_attributes="15"
      units="--"
      description="n Indicates which of the first or second order integrator is to be used.">
    </member>
    <member
      name="default_first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Default value of the Trick::Integrator first_step_deriv flag.">
    </member>
  </class>

  <class name="er7_utils__TrickFirstOrderOdeIntegrator">
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_derivs"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of derivatives used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State vector used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of accelerations used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_velocity"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of velocities used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_position"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of positions used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv2"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv2.">
      <dimension>0</dimension>
    </member>
    <member
      name="trick_integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor used to generate the state integrators this object.">
      <dimension>0</dimension>
    </member>
    <member
      name="first_order_integrator"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a first order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="second_order_integrator"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a second order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="buf_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="half_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="integ_mode"
      type="er7_utils__TrickIntegrator__IntegrationMode"
      io_attributes="15"
      units="--"
      description="n Indicates which of the first or second order integrator is to be used.">
    </member>
    <member
      name="default_first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Default value of the Trick::Integrator first_step_deriv flag.">
    </member>
  </class>

  <class name="Trick__ABM_IntegratorHelper">
    <member
      name="helper_integ_constructor"
      type="er7_utils__ABM4IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with ABM integration.">
    </member>
  </class>

  <class name="Trick__ABM_Integrator">
    <member
      name="helper_integ_constructor"
      type="er7_utils__ABM4IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with ABM integration.">
    </member>
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_derivs"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of derivatives used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State vector used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of accelerations used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_velocity"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of velocities used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_position"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of positions used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv2"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv2.">
      <dimension>0</dimension>
    </member>
    <member
      name="trick_integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor used to generate the state integrators this object.">
      <dimension>0</dimension>
    </member>
    <member
      name="first_order_integrator"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a first order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="second_order_integrator"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a second order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="buf_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="half_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="integ_mode"
      type="er7_utils__TrickIntegrator__IntegrationMode"
      io_attributes="15"
      units="--"
      description="n Indicates which of the first or second order integrator is to be used.">
    </member>
    <member
      name="default_first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Default value of the Trick::Integrator first_step_deriv flag.">
    </member>
  </class>

  <class name="er7_utils__SymplecticEulerIntegratorConstructor">
  </class>

  <class name="er7_utils__EulerFirstOrderODEIntegrator">
    <member
      name="state_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the state.">
    </member>
  </class>

  <class name="er7_utils__SymplecticEulerSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__SymplecticEulerSimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__SymplecticEulerGeneralizedStepSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__TrickSecondOrderOdeIntegrator">
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_derivs"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of derivatives used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State vector used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of accelerations used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_velocity"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of velocities used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_position"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of positions used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv2"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv2.">
      <dimension>0</dimension>
    </member>
    <member
      name="trick_integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor used to generate the state integrators this object.">
      <dimension>0</dimension>
    </member>
    <member
      name="first_order_integrator"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a first order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="second_order_integrator"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a second order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="buf_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="half_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="integ_mode"
      type="er7_utils__TrickIntegrator__IntegrationMode"
      io_attributes="15"
      units="--"
      description="n Indicates which of the first or second order integrator is to be used.">
    </member>
    <member
      name="default_first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Default value of the Trick::Integrator first_step_deriv flag.">
    </member>
  </class>

  <class name="Trick__Euler_Cromer_IntegratorHelper">
    <member
      name="helper_integ_constructor"
      type="er7_utils__SymplecticEulerIntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with Euler Cromer integration.">
    </member>
  </class>

  <class name="Trick__Euler_Cromer_Integrator">
    <member
      name="helper_integ_constructor"
      type="er7_utils__SymplecticEulerIntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with Euler Cromer integration.">
    </member>
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_derivs"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of derivatives used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State vector used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of accelerations used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_velocity"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of velocities used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_position"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of positions used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv2"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv2.">
      <dimension>0</dimension>
    </member>
    <member
      name="trick_integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor used to generate the state integrators this object.">
      <dimension>0</dimension>
    </member>
    <member
      name="first_order_integrator"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a first order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="second_order_integrator"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a second order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="buf_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="half_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="integ_mode"
      type="er7_utils__TrickIntegrator__IntegrationMode"
      io_attributes="15"
      units="--"
      description="n Indicates which of the first or second order integrator is to be used.">
    </member>
    <member
      name="default_first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Default value of the Trick::Integrator first_step_deriv flag.">
    </member>
  </class>

  <class name="er7_utils__EulerIntegratorConstructor">
  </class>

  <class name="er7_utils__EulerSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__EulerSimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__EulerGeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__EulerGeneralizedStepSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__Euler_IntegratorHelper">
    <member
      name="helper_integ_constructor"
      type="er7_utils__EulerIntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with Euler integration.">
    </member>
  </class>

  <class name="Trick__Euler_Integrator">
    <member
      name="helper_integ_constructor"
      type="er7_utils__EulerIntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with Euler integration.">
    </member>
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_derivs"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of derivatives used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State vector used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of accelerations used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_velocity"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of velocities used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_position"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of positions used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv2"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv2.">
      <dimension>0</dimension>
    </member>
    <member
      name="trick_integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor used to generate the state integrators this object.">
      <dimension>0</dimension>
    </member>
    <member
      name="first_order_integrator"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a first order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="second_order_integrator"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a second order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="buf_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="half_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="integ_mode"
      type="er7_utils__TrickIntegrator__IntegrationMode"
      io_attributes="15"
      units="--"
      description="n Indicates which of the first or second order integrator is to be used.">
    </member>
    <member
      name="default_first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Default value of the Trick::Integrator first_step_deriv flag.">
    </member>
  </class>

  <class name="er7_utils__MM4IntegratorConstructor">
  </class>

  <class name="er7_utils__RK2MidpointFirstOrderODEIntegrator">
    <member
      name="state_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the state.">
    </member>
    <member
      name="init_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__MM4SecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="pos_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position history, initial plus results from first two steps.">
      <dimension>3</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="vel_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position history, initial plus results from first two steps.">
      <dimension>3</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Derivative of generalized position.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__MM4SimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="pos_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position history, initial plus results from first two steps.">
      <dimension>3</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="vel_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position history, initial plus results from first two steps.">
      <dimension>3</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Derivative of generalized position.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__MM4GeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="pos_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position history, initial plus results from first two steps.">
      <dimension>3</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="vel_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position history, initial plus results from first two steps.">
      <dimension>3</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Derivative of generalized position.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__MM4_IntegratorHelper">
    <member
      name="helper_integ_constructor"
      type="er7_utils__MM4IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with MM4 integration.">
    </member>
  </class>

  <class name="Trick__MM4_Integrator">
    <member
      name="helper_integ_constructor"
      type="er7_utils__MM4IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with MM4 integration.">
    </member>
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_derivs"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of derivatives used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State vector used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of accelerations used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_velocity"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of velocities used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_position"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of positions used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv2"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv2.">
      <dimension>0</dimension>
    </member>
    <member
      name="trick_integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor used to generate the state integrators this object.">
      <dimension>0</dimension>
    </member>
    <member
      name="first_order_integrator"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a first order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="second_order_integrator"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a second order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="buf_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="half_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="integ_mode"
      type="er7_utils__TrickIntegrator__IntegrationMode"
      io_attributes="15"
      units="--"
      description="n Indicates which of the first or second order integrator is to be used.">
    </member>
    <member
      name="default_first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Default value of the Trick::Integrator first_step_deriv flag.">
    </member>
  </class>

  <class name="er7_utils__NystromLear2IntegratorConstructor">
  </class>

  <class name="er7_utils__NystromLear2SecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Derivative of generalized position.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__NystromLear2SimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Derivative of generalized position.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__NystromLear2GeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Derivative of generalized position.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__NL2_IntegratorHelper">
    <member
      name="helper_integ_constructor"
      type="er7_utils__NystromLear2IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with NL2 integration.">
    </member>
  </class>

  <class name="Trick__NL2_Integrator">
    <member
      name="helper_integ_constructor"
      type="er7_utils__NystromLear2IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with NL2 integration.">
    </member>
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_derivs"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of derivatives used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State vector used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of accelerations used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_velocity"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of velocities used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_position"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of positions used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv2"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv2.">
      <dimension>0</dimension>
    </member>
    <member
      name="trick_integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor used to generate the state integrators this object.">
      <dimension>0</dimension>
    </member>
    <member
      name="first_order_integrator"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a first order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="second_order_integrator"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a second order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="buf_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="half_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="integ_mode"
      type="er7_utils__TrickIntegrator__IntegrationMode"
      io_attributes="15"
      units="--"
      description="n Indicates which of the first or second order integrator is to be used.">
    </member>
    <member
      name="default_first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Default value of the Trick::Integrator first_step_deriv flag.">
    </member>
  </class>

  <class name="er7_utils__RK2HeunIntegratorConstructor">
  </class>

  <class name="er7_utils__RK2HeunFirstOrderODEIntegrator">
    <member
      name="state_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the state.">
    </member>
    <member
      name="init_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n State derivative at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RK2HeunSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity time derivative at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="final_posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the end of an integration cycle.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RK2HeunSimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity time derivative at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="final_posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the end of an integration cycle.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RK2HeunGeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity time derivative at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="final_posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the end of an integration cycle.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RK2HeunGeneralizedStepSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity time derivative at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="final_posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the end of an integration cycle.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__RK2_IntegratorHelper">
    <member
      name="helper_integ_constructor"
      type="er7_utils__RK2HeunIntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with RK2 integration.">
    </member>
  </class>

  <class name="Trick__RK2_Integrator">
    <member
      name="helper_integ_constructor"
      type="er7_utils__RK2HeunIntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with RK2 integration.">
    </member>
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_derivs"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of derivatives used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State vector used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of accelerations used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_velocity"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of velocities used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_position"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of positions used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv2"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv2.">
      <dimension>0</dimension>
    </member>
    <member
      name="trick_integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor used to generate the state integrators this object.">
      <dimension>0</dimension>
    </member>
    <member
      name="first_order_integrator"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a first order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="second_order_integrator"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a second order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="buf_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="half_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="integ_mode"
      type="er7_utils__TrickIntegrator__IntegrationMode"
      io_attributes="15"
      units="--"
      description="n Indicates which of the first or second order integrator is to be used.">
    </member>
    <member
      name="default_first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Default value of the Trick::Integrator first_step_deriv flag.">
    </member>
  </class>

  <class name="Trick__RK4_IntegratorHelper">
    <member
      name="helper_integ_constructor"
      type="er7_utils__RK4IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with RK4 integration.">
    </member>
  </class>

  <class name="Trick__RK4_Integrator">
    <member
      name="helper_integ_constructor"
      type="er7_utils__RK4IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with RK4 integration.">
    </member>
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_derivs"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of derivatives used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State vector used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of accelerations used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_velocity"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of velocities used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_position"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of positions used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv2"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv2.">
      <dimension>0</dimension>
    </member>
    <member
      name="trick_integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor used to generate the state integrators this object.">
      <dimension>0</dimension>
    </member>
    <member
      name="first_order_integrator"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a first order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="second_order_integrator"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a second order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="buf_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="half_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="integ_mode"
      type="er7_utils__TrickIntegrator__IntegrationMode"
      io_attributes="15"
      units="--"
      description="n Indicates which of the first or second order integrator is to be used.">
    </member>
    <member
      name="default_first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Default value of the Trick::Integrator first_step_deriv flag.">
    </member>
  </class>

  <class name="er7_utils__RKFehlberg45IntegratorConstructor">
  </class>

  <class name="er7_utils__RKFehlberg45FirstOrderODEIntegrator">
    <member
      name="state_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the state.">
    </member>
    <member
      name="init_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n State derivatives at each step in the integration cycle.">
      <dimension>6</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RKFehlberg45SecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>6</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>6</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RKFehlberg45SimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>6</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>6</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>6</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>6</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RKFehlberg45GeneralizedStepSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>6</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>6</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__RKF45_IntegratorHelper">
    <member
      name="helper_integ_constructor"
      type="er7_utils__RKFehlberg45IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with RKF45 integration.">
    </member>
  </class>

  <class name="Trick__RKF45_Integrator">
    <member
      name="helper_integ_constructor"
      type="er7_utils__RKFehlberg45IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with RKF45 integration.">
    </member>
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_derivs"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of derivatives used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State vector used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of accelerations used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_velocity"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of velocities used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_position"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of positions used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv2"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv2.">
      <dimension>0</dimension>
    </member>
    <member
      name="trick_integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor used to generate the state integrators this object.">
      <dimension>0</dimension>
    </member>
    <member
      name="first_order_integrator"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a first order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="second_order_integrator"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a second order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="buf_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="half_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="integ_mode"
      type="er7_utils__TrickIntegrator__IntegrationMode"
      io_attributes="15"
      units="--"
      description="n Indicates which of the first or second order integrator is to be used.">
    </member>
    <member
      name="default_first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Default value of the Trick::Integrator first_step_deriv flag.">
    </member>
  </class>

  <class name="er7_utils__RKFehlberg78IntegratorConstructor">
  </class>

  <class name="er7_utils__RKFehlberg78FirstOrderODEIntegrator">
    <member
      name="state_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the state.">
    </member>
    <member
      name="init_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n State derivatives at each step in the integration cycle.">
      <dimension>13</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RKFehlberg78SecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>13</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>13</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RKFehlberg78SimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>13</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>13</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>13</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>13</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RKFehlberg78GeneralizedStepSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>13</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>13</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__RKF78_IntegratorHelper">
    <member
      name="helper_integ_constructor"
      type="er7_utils__RKFehlberg78IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with RKF78 integration.">
    </member>
  </class>

  <class name="Trick__RKF78_Integrator">
    <member
      name="helper_integ_constructor"
      type="er7_utils__RKFehlberg78IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with RKF78 integration.">
    </member>
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_derivs"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of derivatives used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State vector used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of accelerations used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_velocity"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of velocities used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_position"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of positions used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv2"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv2.">
      <dimension>0</dimension>
    </member>
    <member
      name="trick_integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor used to generate the state integrators this object.">
      <dimension>0</dimension>
    </member>
    <member
      name="first_order_integrator"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a first order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="second_order_integrator"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a second order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="buf_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="half_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="integ_mode"
      type="er7_utils__TrickIntegrator__IntegrationMode"
      io_attributes="15"
      units="--"
      description="n Indicates which of the first or second order integrator is to be used.">
    </member>
    <member
      name="default_first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Default value of the Trick::Integrator first_step_deriv flag.">
    </member>
  </class>

  <class name="er7_utils__RKGill4IntegratorConstructor">
  </class>

  <class name="er7_utils__RKGill4FirstOrderODEIntegrator">
    <member
      name="state_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n The size of the state.">
    </member>
    <member
      name="init_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n State derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RKGill4SimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="saved_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position after stage 1.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RKGill4GeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="saved_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position after stage 1.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RKGill4GeneralizedStepSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="saved_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position after stage 1.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Product of delta t and weighted sum of generalized velocities.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="veldot_hist"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity derivatives at each step in the integration cycle.">
      <dimension>4</dimension>
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__RKG4_IntegratorHelper">
    <member
      name="helper_integ_constructor"
      type="er7_utils__RKGill4IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with RKG4 integration.">
    </member>
  </class>

  <class name="Trick__RKG4_Integrator">
    <member
      name="helper_integ_constructor"
      type="er7_utils__RKGill4IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="The integrator constructor associated with RKG4 integration.">
    </member>
    <member
      name="num_state"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="intermediate_step"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="is_2nd_order_ODE_technique"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="use_deriv2"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dt"
      type="double"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="state"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="deriv"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="deriv2"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="state_ws"
      type="double"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="time_0"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor that is used to generate the integration controls for this object and is used to generate the state integrators for each integrated object whose state is integrated by this group.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_interface"
      type="er7_utils__IntegratorInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the simulation engine's integration structure.">
      <dimension>0</dimension>
    </member>
    <member
      name="time_interface"
      type="er7_utils__TimeInterface"
      io_attributes="15"
      units="--"
      description="n The interface between the integration module and the object that represents time.">
      <dimension>0</dimension>
    </member>
    <member
      name="integ_controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that guides the integration process. This object is created at construction time by calling the integrator constructor's create_integration_controls method.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_derivs"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of derivatives used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_state"
      type="double"
      io_attributes="15"
      units="--"
      description="n State vector used to communicate between integrate_1st_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of accelerations used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_velocity"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of velocities used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="cached_position"
      type="double"
      io_attributes="15"
      units="--"
      description="n Vector of positions used to communicate between integrate_2nd_order_ode and integrate_bodies.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv.">
      <dimension>0</dimension>
    </member>
    <member
      name="er7_deriv2"
      type="double"
      io_attributes="15"
      units="--"
      description="n Allocated array of size buf_size used to trick out Trick's load_deriv2.">
      <dimension>0</dimension>
    </member>
    <member
      name="trick_integ_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n The integrator constructor used to generate the state integrators this object.">
      <dimension>0</dimension>
    </member>
    <member
      name="first_order_integrator"
      type="er7_utils__FirstOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a first order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="second_order_integrator"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The integrator that propagates state as a second order ODE.">
      <dimension>0</dimension>
    </member>
    <member
      name="buf_size"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="half_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n Number of elements in the Trick::Integration group arrays.">
    </member>
    <member
      name="integ_mode"
      type="er7_utils__TrickIntegrator__IntegrationMode"
      io_attributes="15"
      units="--"
      description="n Indicates which of the first or second order integrator is to be used.">
    </member>
    <member
      name="default_first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Default value of the Trick::Integrator first_step_deriv flag.">
    </member>
  </class>

  <class name="Trick__IntegrationManager">
  </class>

  <class name="Trick__IntegLoopScheduler">
    <member
      name="verbosity"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="last_step_deriv"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="first_step_deriv"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="integ_ptr"
      type="Trick__Integrator"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
    </member>
    <member
      name="sim_objects"
      type="Trick__IntegLoopScheduler__SimObjectVector"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="nominal_cycle"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="next_cycle"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="parent_sim_object"
      type="Trick__SimObject"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
    </member>
    <member
      name="pre_integ_jobs"
      type="Trick__ScheduledJobQueue"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="deriv_jobs"
      type="Trick__ScheduledJobQueue"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="integ_jobs"
      type="Trick__ScheduledJobQueue"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="dynamic_event_jobs"
      type="Trick__ScheduledJobQueue"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="post_integ_jobs"
      type="Trick__ScheduledJobQueue"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="IntegLoopSimObject">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="integ_sched"
      type="Trick__IntegLoopScheduler"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="TIME_OFFSET">
    <member
      name="day"
      type="int"
      io_attributes="15"
      units="--"
      description="Day of the year, 0 .. 364">
    </member>
    <member
      name="hour"
      type="int"
      io_attributes="15"
      units="--"
      description="Hour of the day, 0 .. 23">
    </member>
    <member
      name="min"
      type="int"
      io_attributes="15"
      units="--"
      description="Minute of the hour, 0 .. 59">
    </member>
    <member
      name="sec"
      type="double"
      io_attributes="15"
      units="--"
      description="Seconds of the hour 0.0 .. 59.99999999">
    </member>
  </class>

  <class name="Trick__SimTime">
    <member
      name="ret_time"
      type="TIME_OFFSET"
      io_attributes="15"
      units="--"
      description="current run elapsed time">
    </member>
    <member
      name="gmt_ref"
      type="TIME_OFFSET"
      io_attributes="15"
      units="--"
      description="gmt ref time">
    </member>
    <member
      name="gmt_sec_ref"
      type="double"
      io_attributes="15"
      units="--"
      description="gmt ref in seconds">
    </member>
    <member
      name="gmt_time"
      type="TIME_OFFSET"
      io_attributes="15"
      units="--"
      description="current gmt time">
    </member>
    <member
      name="met_ref"
      type="TIME_OFFSET"
      io_attributes="15"
      units="--"
      description="met ref time">
    </member>
    <member
      name="met_sec_ref"
      type="double"
      io_attributes="15"
      units="--"
      description="met ref in seconds">
    </member>
    <member
      name="met_time"
      type="TIME_OFFSET"
      io_attributes="15"
      units="--"
      description="current met time">
    </member>
  </class>

  <class name="er7_utils__IntegratorConstructorFactory">
  </class>

  <class name="er7_utils__BeemanIntegratorConstructor">
    <member
      name="primer_constructor"
      type="er7_utils__IntegratorConstructor"
      io_attributes="15"
      units="--"
      description="n Constructor used to create the primers.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__BeemanSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that controls this state integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="primer"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The state integrator used to prime this integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="priming_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The number of start of cycle derivatives needed by the technique.">
    </member>
    <member
      name="prime_counter"
      type="int"
      io_attributes="15"
      units="--"
      description="n Priming phase countdown. This is set to priming_state_size initially and upon reset. Priming is complete when the counter reaches zero.">
    </member>
    <member
      name="primed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Indicates whether the object has been primed. This is false initially and upon reset; it is set to true when priming is complete.">
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="mean_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity used to update position.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="prev_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the start of the previous integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the current time step.">
      <dimension>0</dimension>
    </member>
    <member
      name="posddot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position 2nd derivative at the current time step.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_posddot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position 2nd derivative at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="prev_posddot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position 2nd derivative at the start of the previous integration cycle.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__BeemanSimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that controls this state integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="primer"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The state integrator used to prime this integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="priming_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The number of start of cycle derivatives needed by the technique.">
    </member>
    <member
      name="prime_counter"
      type="int"
      io_attributes="15"
      units="--"
      description="n Priming phase countdown. This is set to priming_state_size initially and upon reset. Priming is complete when the counter reaches zero.">
    </member>
    <member
      name="primed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Indicates whether the object has been primed. This is false initially and upon reset; it is set to true when priming is complete.">
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="mean_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity used to update position.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="prev_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the start of the previous integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the current time step.">
      <dimension>0</dimension>
    </member>
    <member
      name="posddot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position 2nd derivative at the current time step.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_posddot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position 2nd derivative at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="prev_posddot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position 2nd derivative at the start of the previous integration cycle.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__BeemanGeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="controls"
      type="er7_utils__IntegrationControls"
      io_attributes="15"
      units="--"
      description="n The integration controls object that controls this state integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="primer"
      type="er7_utils__SecondOrderODEIntegrator"
      io_attributes="15"
      units="--"
      description="n The state integrator used to prime this integrator.">
      <dimension>0</dimension>
    </member>
    <member
      name="priming_state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The number of start of cycle derivatives needed by the technique.">
    </member>
    <member
      name="prime_counter"
      type="int"
      io_attributes="15"
      units="--"
      description="n Priming phase countdown. This is set to priming_state_size initially and upon reset. Priming is complete when the counter reaches zero.">
    </member>
    <member
      name="primed"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Indicates whether the object has been primed. This is false initially and upon reset; it is set to true when priming is complete.">
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="mean_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity used to update position.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="prev_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the start of the previous integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the current time step.">
      <dimension>0</dimension>
    </member>
    <member
      name="posddot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position 2nd derivative at the current time step.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_posddot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position 2nd derivative at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="prev_posddot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position 2nd derivative at the start of the previous integration cycle.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__PositionVerletIntegratorConstructor">
  </class>

  <class name="er7_utils__PositionVerletSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Derivative of generalized position.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Generalized position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="h_dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Half of the generalized position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="midpoint"
      type="double"
      io_attributes="15"
      units="--"
      description="n Leapfrog position.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of the current integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="prev_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the end of the previous integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="restart"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Make an initial half step when set. This flag is set initially and after a call to reset. The flag is cleared upon making that half step.">
    </member>
  </class>

  <class name="er7_utils__PositionVerletSimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Derivative of generalized position.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Generalized position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="h_dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Half of the generalized position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="midpoint"
      type="double"
      io_attributes="15"
      units="--"
      description="n Leapfrog position.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of the current integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="prev_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the end of the previous integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="restart"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Make an initial half step when set. This flag is set initially and after a call to reset. The flag is cleared upon making that half step.">
    </member>
  </class>

  <class name="er7_utils__PositionVerletGeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Derivative of generalized position.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Generalized position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="h_dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Half of the generalized position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="midpoint"
      type="double"
      io_attributes="15"
      units="--"
      description="n Leapfrog position.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of the current integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="prev_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the end of the previous integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="restart"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Make an initial half step when set. This flag is set initially and after a call to reset. The flag is cleared upon making that half step.">
    </member>
  </class>

  <class name="er7_utils__PositionVerletGeneralizedStepSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Derivative of generalized position.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Generalized position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="h_dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Half of the generalized position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="midpoint"
      type="double"
      io_attributes="15"
      units="--"
      description="n Leapfrog position.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of the current integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="prev_accel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the end of the previous integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="restart"
      type="bool"
      io_attributes="15"
      units="--"
      description="n Make an initial half step when set. This flag is set initially and after a call to reset. The flag is cleared upon making that half step.">
    </member>
  </class>

  <class name="er7_utils__RK2MidpointIntegratorConstructor">
  </class>

  <class name="er7_utils__RK2MidpointSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position step.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RK2MidpointSimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position step.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RK2MidpointGeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position step.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__RK2MidpointGeneralizedStepSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_pos"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of an integration cycle.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivatives.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position step.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__VelocityVerletIntegratorConstructor">
  </class>

  <class name="er7_utils__VelocityVerletSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of the integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the start of the integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Generalized position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the first step of the integration cycle.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__VelocityVerletSimpleSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of the integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the start of the integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Generalized position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the first step of the integration cycle.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__VelocityVerletGeneralizedDerivSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of the integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the start of the integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Generalized position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the first step of the integration cycle.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__VelocityVerletGeneralizedStepSecondOrderODEIntegrator">
    <member
      name="state_size"
      type="int"
      io_attributes="15"
      units="--"
      description="n The sizes of the generalized position and generalized velocity vectors.">
      <dimension>2</dimension>
    </member>
    <member
      name="problem_type"
      type="er7_utils__Integration__ODEProblemType"
      io_attributes="15"
      units="--"
      description="n The type of problem to be solved.">
    </member>
    <member
      name="init_vel"
      type="double"
      io_attributes="15"
      units="--"
      description="n Velocity at the start of the integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="init_acc"
      type="double"
      io_attributes="15"
      units="--"
      description="n Acceleration at the start of the integration interval.">
      <dimension>0</dimension>
    </member>
    <member
      name="dtheta"
      type="double"
      io_attributes="15"
      units="--"
      description="n Generalized position step.">
      <dimension>0</dimension>
    </member>
    <member
      name="posdot"
      type="double"
      io_attributes="15"
      units="--"
      description="n Position derivative at the first step of the integration cycle.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__IntegrableObject">
    <member
      name="integ_group"
      type="er7_utils__BaseIntegrationGroup"
      io_attributes="15"
      units="--"
      description="n The integration group that integrates this body, and possibly others.">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__IntegratorResultMerger">
  </class>

  <class name="er7_utils__BogusIntegratorResultMerger">
  </class>

  <class name="er7_utils__IntegratorResultMergerContainer">
    <member
      name="integ_merger"
      type="er7_utils__IntegratorResultMerger"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="er7_utils__LeftQuaternionGeneralizedPositionDerivativeFunctions">
  </class>

  <class name="er7_utils__LeftQuaternionGeneralizedPositionStepFunctions">
  </class>

  <class name="er7_utils__LeftQuaternionGeneralizedPositionFunctions">
  </class>

  <class name="er7_utils__NChooseM">
    <member
      name="triangle"
      type="unsigned long"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
    </member>
    <member
      name="triangle_nrows"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="er7_utils__UInt128">
    <member
      name="low"
      type="unsigned long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="high"
      type="unsigned long"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="er7_utils__Ratio128">
    <member
      name="sign"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="num"
      type="er7_utils__UInt128"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="den"
      type="er7_utils__UInt128"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__DataRecordBuffer">
  </class>

  <class name="Trick__DataRecordGroup">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="record"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="inited"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="group_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="output_dir"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="file_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="freq"
      type="Trick__DR_Freq"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="start"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="cycle"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="num_variable_names"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="variable_names"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="variable_alias"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="num_change_variable_names"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="change_variable_names"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="change_variable_alias"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="max_num"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="max_size_warning"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="writer_buff_size"
      type="unsigned long"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="byte_order"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="single_prec_only"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="buffer_type"
      type="Trick__DR_Buffering"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="job_class"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="curr_time"
      type="double"
      io_attributes="10"
      units="--">
    </member>
  </class>

  <class name="Trick__DRDMutexes">
    <member
      name="cancelled"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__DRDWriterThread">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="rt_priority"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__DataRecordDispatcher">
    <member
      name="drd_writer_thread"
      type="Trick__DRDWriterThread"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__DRAscii">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="record"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="inited"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="group_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="output_dir"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="file_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="freq"
      type="Trick__DR_Freq"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="start"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="cycle"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="num_variable_names"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="variable_names"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="variable_alias"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="num_change_variable_names"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="change_variable_names"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="change_variable_alias"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="max_num"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="max_size_warning"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="writer_buff_size"
      type="unsigned long"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="byte_order"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="single_prec_only"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="buffer_type"
      type="Trick__DR_Buffering"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="job_class"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="curr_time"
      type="double"
      io_attributes="10"
      units="--">
    </member>
    <member
      name="ascii_float_format"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="ascii_double_format"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="delimiter"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__DRBinary">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="record"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="inited"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="group_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="output_dir"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="file_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="freq"
      type="Trick__DR_Freq"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="start"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="cycle"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="num_variable_names"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="variable_names"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="variable_alias"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="num_change_variable_names"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="change_variable_names"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="change_variable_alias"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="max_num"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="max_size_warning"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="writer_buff_size"
      type="unsigned long"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="byte_order"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="single_prec_only"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="buffer_type"
      type="Trick__DR_Buffering"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="job_class"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="curr_time"
      type="double"
      io_attributes="10"
      units="--">
    </member>
  </class>

  <class name="Trick__DRHDF5">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="record"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="inited"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="group_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="output_dir"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="file_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="freq"
      type="Trick__DR_Freq"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="start"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="cycle"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="num_variable_names"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="variable_names"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="variable_alias"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="num_change_variable_names"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="change_variable_names"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="change_variable_alias"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="max_num"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="max_size_warning"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="writer_buff_size"
      type="unsigned long"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="byte_order"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="single_prec_only"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="buffer_type"
      type="Trick__DR_Buffering"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="job_class"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="curr_time"
      type="double"
      io_attributes="10"
      units="--">
    </member>
  </class>

  <class name="Trick__DebugPause">
    <member
      name="debug_pause_flag"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__EchoJobs">
    <member
      name="echo_job_flag"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__FrameDataRecordGroup">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="id"
      type="int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="object_disabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="record"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="inited"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="group_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="output_dir"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="file_name"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="freq"
      type="Trick__DR_Freq"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="start"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="cycle"
      type="double"
      io_attributes="15"
      units="s">
    </member>
    <member
      name="num_variable_names"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="variable_names"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="variable_alias"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="num_change_variable_names"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="change_variable_names"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="change_variable_alias"
      type="char"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
      <dimension>0</dimension>
    </member>
    <member
      name="max_num"
      type="unsigned int"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="max_size_warning"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="writer_buff_size"
      type="unsigned long"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="byte_order"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="single_prec_only"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="buffer_type"
      type="Trick__DR_Buffering"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="job_class"
      type="std__string"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="curr_time"
      type="double"
      io_attributes="10"
      units="--">
    </member>
    <member
      name="thread_id"
      type="unsigned int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="thread_record_time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="start_time"
      type="long long"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="frame_sched_time"
      type="long long"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__timeline_t">
    <member
      name="trick_job"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="id"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="start"
      type="long long"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="stop"
      type="long long"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__FrameLog">
    <member
      name="frame_log_flag"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="drg_trick"
      type="Trick__FrameDataRecordGroup"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
    </member>
    <member
      name="drg_frame"
      type="Trick__FrameDataRecordGroup"
      io_attributes="15"
      units="--">
      <dimension>0</dimension>
    </member>
    <member
      name="plots_per_page"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="number of plots per page">
    </member>
  </class>

  <class name="Trick__TestCase">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="status"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="parent"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="elapsed_time"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="failure_string"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__TestSuite">
    <member
      name="num_failures"
      type="unsigned int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="test_results"
      type="std__vector&lt;Trick__TestCase *>"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__UnitTest">
    <member
      name="enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="exit_code_enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="file_name"
      type="std__string"
      io_attributes="10"
      units="--">
    </member>
  </class>

  <class name="Trick__CheckPointRestart">
    <member
      name="checkpoint_times"
      type="std__priority_queue&lt;long long, std__vector&lt;long long>, std__greater&lt;long long> >"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="safestore_period"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="safestore_time"
      type="long long"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="pre_init_checkpoint"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="post_init_checkpoint"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="end_checkpoint"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="safestore_enabled"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
    <member
      name="cpu_num"
      type="int"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__AttributesMap">
    <member
      name="name_to_attr_map"
      type="std__map&lt;std__string, ATTRIBUTES *>"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__EnumAttributesMap">
    <member
      name="name_to_attr_map"
      type="std__map&lt;std__string, ENUM_ATTR *>"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__Sie">
    <member
      name="move_runtime_generation"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__STLInterface">
    <member
      name="stl_id"
      type="unsigned int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="restore"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__ExternalApplication">
    <member
      name="host"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="port"
      type="unsigned short"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="auto_reconnect"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="auto_reconnect_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="cycle_period"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="cycle_period_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="minimum_cycle_period"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="minimum_cycle_period_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="disconnect_behavior"
      type="Trick__ExternalApplication__DisconnectBehavior"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="disconnect_behavior_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="height"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="height_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="width"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="width_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="x"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="x_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="y"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="y_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="command_c_str"
      type="char"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="allocations"
      type="std__vector&lt;char *>"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__SimControlPanel">
    <member
      name="host"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="port"
      type="unsigned short"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="auto_reconnect"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="auto_reconnect_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="cycle_period"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="cycle_period_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="minimum_cycle_period"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="minimum_cycle_period_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="disconnect_behavior"
      type="Trick__ExternalApplication__DisconnectBehavior"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="disconnect_behavior_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="height"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="height_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="width"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="width_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="x"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="x_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="y"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="y_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="command_c_str"
      type="char"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="allocations"
      type="std__vector&lt;char *>"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="auto_exit"
      type="bool"
      io_attributes="15"
      units="--">
    </member>
  </class>

  <class name="Trick__TrickView">
    <member
      name="host"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="port"
      type="unsigned short"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="auto_reconnect"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="auto_reconnect_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="cycle_period"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="cycle_period_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="minimum_cycle_period"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="minimum_cycle_period_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="disconnect_behavior"
      type="Trick__ExternalApplication__DisconnectBehavior"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="disconnect_behavior_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="height"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="height_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="width"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="width_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="x"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="x_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="y"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="y_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="command_c_str"
      type="char"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="allocations"
      type="std__vector&lt;char *>"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="strip_charts_only"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="strip_charts_only_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="open_file"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="open_file_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="open_set_file"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="open_set_file_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="set_file"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="set_file_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__MalfunctionsTrickView">
    <member
      name="host"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="port"
      type="unsigned short"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="auto_reconnect"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="auto_reconnect_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="cycle_period"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="cycle_period_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="minimum_cycle_period"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="minimum_cycle_period_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="disconnect_behavior"
      type="Trick__ExternalApplication__DisconnectBehavior"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="disconnect_behavior_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="height"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="height_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="width"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="width_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="x"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="x_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="y"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="y_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="command_c_str"
      type="char"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="allocations"
      type="std__vector&lt;char *>"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__MonteMonitor">
    <member
      name="host"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="port"
      type="unsigned short"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="auto_reconnect"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="auto_reconnect_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="cycle_period"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="cycle_period_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="minimum_cycle_period"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="minimum_cycle_period_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="disconnect_behavior"
      type="Trick__ExternalApplication__DisconnectBehavior"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="disconnect_behavior_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="height"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="height_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="width"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="width_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="x"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="x_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="y"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="y_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="command_c_str"
      type="char"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="allocations"
      type="std__vector&lt;char *>"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__StripChart">
    <member
      name="host"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="port"
      type="unsigned short"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="auto_reconnect"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="auto_reconnect_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="cycle_period"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="cycle_period_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="minimum_cycle_period"
      type="double"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="minimum_cycle_period_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="disconnect_behavior"
      type="Trick__ExternalApplication__DisconnectBehavior"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="disconnect_behavior_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="height"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="height_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="width"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="width_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="x"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="x_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="y"
      type="int"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="y_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="command"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="command_c_str"
      type="char"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
    <member
      name="allocations"
      type="std__vector&lt;char *>"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="input_file"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="file_set"
      type="bool"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__RtiEventBase">
    <member
      name="ref"
      type="REF2"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__RtiList">
    <member
      name="event_list"
      type="std__vector&lt;RtiEventBase *>"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__RtiExec">
    <member
      name="frame_multiple"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="multiple of software_frame cycles to execute">
    </member>
    <member
      name="frame_offset"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="offset number of frames to execute">
    </member>
  </class>

  <class name="Trick__RtiStager">
  </class>

  <class name="Trick__UdUnits">
  </class>

  <class name="triad_t">
    <member
      name="op"
      type="int"
      io_attributes="15"
      units="--"
      description="operator">
    </member>
    <member
      name="a1"
      type="int"
      io_attributes="15"
      units="--"
      description="arg1">
    </member>
    <member
      name="a2"
      type="int"
      io_attributes="15"
      units="--"
      description="arg2">
    </member>
  </class>

  <class name="Units_t">
    <member
      name="nt"
      type="int"
      io_attributes="15"
      units="--"
      description="Number of Triads">
    </member>
    <member
      name="t"
      type="triad_t"
      io_attributes="15"
      units="--"
      description="triad array">
      <dimension>24</dimension>
    </member>
  </class>

  <class name="UnitsConvFn_t">
    <member
      name="C"
      type="double"
      io_attributes="15"
      units="--"
      description="Conversion Function Coefficients">
      <dimension>2</dimension>
    </member>
  </class>

  <class name="UCFn">
  </class>

  <class name="Unit__CONVERSION_ERROR">
  </class>

  <class name="Unit">
    <member
      name="units_name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="units"
      type="Units_t"
      io_attributes="15"
      units="1">
      <dimension>0</dimension>
    </member>
  </class>

  <class name="Trick__UnitsMap">
    <member
      name="param_units"
      type="std__map&lt;std__string, char *>"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="Trick__Zeroconf">
    <member
      name="name"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
    <member
      name="type"
      type="std__string"
      io_attributes="15"
      units="1">
    </member>
  </class>

  <class name="WAVE_FORM">
    <member
      name="type"
      type="Wave_Form"
      io_attributes="15"
      units="--"
      description="Type of wave form">
    </member>
    <member
      name="start"
      type="double"
      io_attributes="15"
      units="s"
      description="Start time for wave">
    </member>
    <member
      name="stop"
      type="double"
      io_attributes="15"
      units="s"
      description="Stop time for wave">
    </member>
    <member
      name="ramp"
      type="double"
      io_attributes="15"
      units="s"
      description="Time to ramp wave output from 0 to 1 and 1 to 0">
    </member>
    <member
      name="pause"
      type="double"
      io_attributes="15"
      units="s"
      description="Pause time between end of first doublet pulse and start of second doublet pulse">
    </member>
    <member
      name="frequency"
      type="double"
      io_attributes="15"
      units="1/s"
      description="Frequency of oscillating waves">
    </member>
    <member
      name="phase"
      type="double"
      io_attributes="15"
      units="rad"
      description="Phase offset for start of oscillating waves">
    </member>
    <member
      name="init_random"
      type="int"
      io_attributes="15"
      units="--"
      description="Yes = Initialize random number generator">
    </member>
    <member
      name="random_seed"
      type="unsigned long"
      io_attributes="15"
      units="--"
      description="Seed for random noise generator">
    </member>
    <member
      name="sigma_range"
      type="int"
      io_attributes="15"
      units="--"
      description="Range of output in standard deviations, 3 = 3sigma, 6 = 6sigma, etc.">
    </member>
    <member
      name="init_user_data"
      type="int"
      io_attributes="15"
      units="--"
      description="Yes = Open user defined wave form data file">
    </member>
    <member
      name="user_data"
      type="char"
      io_attributes="15"
      units="--"
      description="File name for user defined wave form">
      <dimension>0</dimension>
    </member>
    <member
      name="next_time"
      type="double"
      io_attributes="15"
      units="s"
      description="Next user time from user_data">
    </member>
    <member
      name="next_val"
      type="double"
      io_attributes="15"
      units="--"
      description="Next user wave value from user_data">
    </member>
    <member
      name="user_time"
      type="double"
      io_attributes="15"
      units="s"
      description="Current user time from user_data">
    </member>
    <member
      name="user_val"
      type="double"
      io_attributes="15"
      units="--"
      description="Current user wave value from user_data">
    </member>
  </class>

  <class name="RAND_GENERATOR">
    <member
      name="option"
      type="rand_generator"
      io_attributes="15"
      units="--"
      description="Random number generator option">
    </member>
    <member
      name="uniform"
      type="uniform_generator"
      io_attributes="15"
      units="--"
      description="Uniform Random number generator option">
    </member>
    <member
      name="seed_1"
      type="unsigned long"
      io_attributes="15"
      units="--"
      description="Seed for uniform RNG single LCG">
    </member>
    <member
      name="seed_trpl"
      type="long"
      io_attributes="15"
      units="--"
      description="Seed for uniform RNG triple LCG make this seed negative for initializing the RNG table">
    </member>
    <member
      name="iff"
      type="int"
      io_attributes="15"
      units="--"
      description="Initialization flag for triple LCG">
    </member>
    <member
      name="table"
      type="double"
      io_attributes="15"
      units="--"
      description="Table for RNG triple LCG">
      <dimension>98</dimension>
    </member>
    <member
      name="sigma_range"
      type="int"
      io_attributes="15"
      units="--"
      description="Sigma range for output random num Must be > 0 for triangular gauss. Must be >= 0 for bell gauss, 0 gives infinite sigma.">
    </member>
    <member
      name="ix1"
      type="long"
      io_attributes="15"
      units="--"
      description="Intermidiate Radom Number">
    </member>
    <member
      name="ix2"
      type="long"
      io_attributes="15"
      units="--"
      description="Intermidiate Radom Number">
    </member>
    <member
      name="ix3"
      type="long"
      io_attributes="15"
      units="--"
      description="Intermidiate Radom Number">
    </member>
  </class>

  <class name="TRICK_GSL_RANDIST">
    <member
      name="type"
      type="TRICK_GSL_TYPE"
      io_attributes="15"
      units="--"
      description="Type of distribution desired">
    </member>
    <member
      name="seed"
      type="unsigned long"
      io_attributes="15"
      units="--"
      description="Seed">
    </member>
    <member
      name="sigma"
      type="double"
      io_attributes="15"
      units="--"
      description="Std. dev for gauss/poisson distrib">
    </member>
    <member
      name="mu"
      type="double"
      io_attributes="15"
      units="--"
      description="mean for gauss/poisson distribution">
    </member>
    <member
      name="min"
      type="double"
      io_attributes="15"
      units="--"
      description="Minimum value in range">
    </member>
    <member
      name="max"
      type="double"
      io_attributes="15"
      units="--"
      description="Maximum value in range">
    </member>
    <member
      name="rel_min"
      type="int"
      io_attributes="15"
      units="--"
      description="Minimun is a relative minimum">
    </member>
    <member
      name="rel_max"
      type="int"
      io_attributes="15"
      units="--"
      description="Maximum is a relative maximum">
    </member>
    <member
      name="sigma_range"
      type="int"
      io_attributes="15"
      units="--"
      description="Sigma range for output random num Must be > 0 for triangular gauss. Must be >= 0 for bell gauss, 0 gives infinite sigma.">
    </member>
    <member
      name="uniform"
      type="uniform_generator"
      io_attributes="15"
      units="--"
      description="Uniform Random number generator option">
    </member>
  </class>

  <class name="TRICK_GSL_RETURN_TYPE">
    <member
      name="ui"
      type="unsigned int"
      io_attributes="15"
      units="--"
      description="unsigned integer return">
    </member>
    <member
      name="d"
      type="double"
      io_attributes="15"
      units="--"
      description="double return">
    </member>
    <member
      name="ll"
      type="unsigned long long"
      io_attributes="15"
      units="--"
      description="long long field used for printing">
    </member>
    <member
      name="ii"
      type="int"
      io_attributes="15"
      units="--"
      description="signed int return">
    </member>
  </class>

  <class name="_LQUEUE">
    <member
      name="list"
      type="_DLLIST"
      io_attributes="15"
      units="--"
      description="implementing queue using a linked-list">
    </member>
  </class>

  <class name="LQUEUE">
    <member
      name="list"
      type="_DLLIST"
      io_attributes="15"
      units="--"
      description="implementing queue using a linked-list">
    </member>
  </class>

  <class name="_LSTACK">
    <member
      name="list"
      type="_DLLIST"
      io_attributes="15"
      units="--"
      description="implementing the stack using a list">
    </member>
  </class>

  <class name="LSTACK">
    <member
      name="list"
      type="_DLLIST"
      io_attributes="15"
      units="--"
      description="implementing the stack using a list">
    </member>
  </class>

<!--
Runtime Allocations
Do not edit this comment or file content past this point
-->
</sie>