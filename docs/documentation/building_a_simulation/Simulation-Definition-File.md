| [Home](/trick) → [Documentation Home](../Documentation-Home) → [Building a Simulation](Building-a-Simulation) → Simulation Definition File |
|------------------------------------------------------------------|

The Simulation Definition File or S_define is the file which lays out the architecture
of the simulation.  Details about job frequencies, job class, job data, importing/exporting
data to other simulations, freeze cycles, integration, etc. are all housed in this one file.
It is the file which Trick's main processor, CP, parses to determine what will be part of
the simulation and how the pieces fit together.

This section begins with the syntax for the S_define, then details each and every entry
in the syntax template. This section also details how to use Trick's main processor, CP.

```
[/* [TRICK_HEADER]

PURPOSE: (purpose statement)
[LIBRARY DEPENDENCIES:
    (
     [(<rel_path_of_model>/<model_source_code_n.c>)]
    )
]

[DEFAULT_DATA:
    (
     [(struct_type instance_name <rel_path_of_model>/<default_data_file>)]
    )
]
*/]

#include "sim_objects/default_trick_sys.sm"

##include "<rel_path_of_model>/<model_header_file.h>"

%header{
    /* User header code block */
%}

%{
    /* User code block */
%}

class <sim_object_type_name> : public Trick::SimObject {

    [(public|protected|private):]

        <CLASS_TYPE|STRUCTURE_TYPE|ENUMERATED_TYPE|intrinsic_type> [*]* <data_varaible_name> [dims]*

        <sim_object_type_name>([args]) {
            [C<#>] [{job_tag}] [P<#>] ([<cycle_time>, [<start_time>, [<stop_time>,]]] <job_class>) \
             [<return_var> =] <module>([args]) ;

        }

        [<method_name>([args]) { ... } ;]
} ;

[job_class_order {
   <job_class_1>,
   <job_class_2>,
   ...
} ;]

[<sim_object_type_name> <sim_object_instantiation>[(args)] ;]

[integrate <integrator_name> (<integration_dt>) <sim_object_name> [,<sim_object_name] ;]

[collect <reference> = {[<reference> [,<reference>]]};]

[void create_connections() { <glue_code> }]
```

## Trick Header Comment

This optional section of the S_define file is a C style comment found anywhere in the S_define file.
CP will parse the Trick Header comment looking for library dependencies and default data.  Library
dependencies are model source code files that are added to the list of files to be compiled and
linked into the simulation.  These dependencies differ from the ones found in the actual model source
code in that they are the full relative path to the actual source code file, not the resulting object file.
CP also looks for old style default data files.  Each default data entry has 3 fields, the structure type, the
instance name, and the relative path to the default data file.  CP will read in the default data file
substituting each occurrence of the structure type in the file with the instance name.  All of the default
data files are concatenated to the S_default.dat file.

### S_define Library Dependencies
```
LIBRARY DEPENDENCY:
    ((relative_path/model_1.c)
     (relative_path/model_2.cpp))
```

Library dependencies list out model source code files required by the simulation.  There are several
locations to add library dependencies, one of which is in the S_define file.  The format of
dependencies in the S_define file is a relative path to the model source file.  The path is relative
to -I include paths found in TRICK_CFLAGS and TRICK_CXXFLAGS.

For example if the full path to our model is /this/is/a/full/path/to/model.c and in our TRICK_CFLAGS
we have -I/this/is/a/full as one of the included search paths, the library dependency must complete the
full path to the file, in this case path/to/model.c.  Library dependendencies in the S_define file
differ from ones found in model source code as they must be the full path to the source file not the
object file.

## Include files

There are two types of includes in the S_define file.

### Single pound "#" includes.

Include files with a single pound "#" are parsed as they are part of the S_define file.  They are
treated just as #include files in C or C++ files.  These files usually include other sim objects or
instantiations as part of the S_define file.

### Double pound "#" includes.

Include files with a double pound "##" are not parsed as part of the S_define file.  These files are the
model header files.  They include the model class and structure definitions as well as C prototypes for
functions used in the S_define file.  Double pound files are copied, minus one pound, to S_source.hh.

## User Header Code Block

This section of the S_define (encapsulated by "%header{...%}") can be used for including header files
directly into the S_source.hh. Header files listed here will not be input processed.

## User Code Block

This section of the S_define (encapsulated by %{.....%}) can be used for any user specific
global C code. CP will simply insert this section of code into the S_source.c file after
all header files are included. Typically this feature is used as a quick method for customizing
simulations with additions of global variables and functions.

## Simulation Object Definition

A simulation definition file may contain any number of simulation object definitions.
A simulation object definition is of the form: class <sim_object_name> : public Trick::SimObject { ... }.
All sim objects must inherit from the Trick::SimObject or a derivative.  A sim object definition
may contain zero or more data structure declarations and zero or more module declarations.

## Model Classes and Data Structures

Model classes and data structures are declared within a sim object. Model classes and data structures
may be public, protected, or private within the sim object.  Standard C++ privacy rules apply to
all data with the sim object. Sim object protected and private data will not be accessible to the input
processor.

Intrinsic types are allowed as sim object data members.

## Job Declarations

Jobs are the hands and feet of the simulation.  They are the building blocks for the
simulation.  Jobs are C or C++ routines with special Trick tags that determine scheduling,
object dependencies, etc.

Jobs only appear in the constructor of the sim object.

```
[C<#>] [{job_tag}] [P<#>] ([<cycle_time>, [<start_time>, [<stop_time>,]]] <job_class>) <module>([args]) ;
```

Most of these fields are optional depending on how the module is classified or utilized
within the sim. The following subsections detail the usage and purpose of each of these fields.

### Child Thread Specification

The first field of a module declaration is an optional child process specification in
the form of a capital C immediately followed by an integer child ID number; i.e. C1, C2, C3,
etc. The child process specification allows parallel processing for simulation modules.

Every simulation has a parent process. A child specification will result in the spawning of
a thread for each distinct child ID specified. More than one job can be specified for each child ID.
Jobs with child specifications will run in parallel with other jobs within each software frame,
so users may be required to specify job dependencies (see Section 4.4.10) to keep parallel jobs
from stepping on each other's common data access. The collection of the parent process and all
of its children defined in one S_define file is referred to as a Process Group (PG). A simulation
can be composed of multiple synchronized or non-synchronized PGs which will be discussed in more
detail in Section 4.4.12. and Section 7.2.

In most cases, for maximum execution performance, jobs should only be specified to run on a child
process if the host workstation has more than one CPU; and preferably, one CPU per child specified.
With the same rule in mind, using more than one PG in the simulation design should only occur when
the simulation has parallel components and multiple process/multiple computers are available. When
the host workstation has only one CPU, a simulation with a job running on a child will be much slower
than the same simulation with no children. There are exceptions to this rule, like running asynchronous
background modules on a child that only runs the child during the wait cycle of a simulation set up for
real-time operations.

Child IDs start at 1 and may skip any integer values. The Trick Executive will spawn enough threads
to accomodate the highest Child ID specified in the S_define file.  Jobs without a child specification
will be executed by the parent process. Jobs with a C1 child specification will be executed by the
first child thread; jobs with a C2 specification will be executed by the second child thread; and so on.

Child Threads have three different scheduling choices.  See [Executive Scheduler](https://nasa.github.io/trick/documentation/simulation_capabilities/Executive-Scheduler) -> [Thread Control](https://nasa.github.io/trick/documentation/simulation_capabilities/Executive-Scheduler#thread-control) for child thread scheduling
details.

### Job Tagging

This optional field allows you to tag a job or set of jobs.  The tag is surrounded in curly
braces.  In the input file, you may then operate on the tag.  All jobs with the same tag will be
modified in the same manner.  For example, if jobA and jobB are tagged BLUE, the input file may
have a statement:

```python
trick.add_read(13.0, """trick.exec_set_job_cycle("BLUE" , 0.001)""")
```

This will change the frequency of the jobs to 1 millisecond.  You might also disable the jobs
tagged BLUE with the following:

```python
trick.add_read(10.0, """trick.exec_set_job_onoff("BLUE" , False)""")
```

### Job Phasing

The next field of a job declaration is an optional phase number specification in the form of a
capital P immediately followed by an integer phase ID number from 1 to 65534, e.g., P1, P2, P3, etc.
Without a specified phase field, the default phase number is 60000.  The phase specification may be
used on all class jobs to sequence the execution of jobs in the same class.  Jobs tagged with P1
execute first, then P2, etc.  Jobs with the same phase number are executed in the order they are
in the S_define.

### Execution Schedule Time Specification

The execution schedule specification specifies the job's execution cycle time, start time, and
stop time. The time specs must be a comma separated list of floating point numbers enclosed by
parentheses, e.g. (1.0,0.5,10.0) would execute a module starting at 0.5 seconds, and every 1.0
seconds thereafter until 10.0 seconds was reached (9.5 seconds would be the time of the last
job call). The start and stop fields are optional; e.g. (1.0,0.5) does the same as the previous
example except the module will keep being called after 10.0 seconds. Also, a (1.0) specification
will start the job at 0.0 (the default) and continue calling the job at 1.0 second intervals.

Only the jobs categorized as CYCLIC or also
the freeze_scheduled job class (see Table SD_1 below) require the execution time specification.
All schedule time specifications are in seconds.

All other job classes categorized in Table SD_1 should NOT specify an execution time specification:
- NON-CYCLIC (red color) of course do not require a spec because they run only once.
- FRAME-BOUNDARY (blue color) are tied to each execution frame and not a cycle time.
- INTEGRATOR (cyan color) are specified via the state integration specifications for the simulation (see Section 4.4.7).ME-BOUNDARY (blue color) are tied to each execution frame and not a cycle time.
- INTEGRATOR (cyan color) are specified via the state integration specifications for the simulation (see Section 4.4.7).
- SELF-SCHEDULING (yellow color) are responsible for scheduling themselves.
- FREEZE (purple color) are tied to the freeze frame (EXCEPT for freeze_scheduled class jobs which DO need the spec.)
- CHECKPOINT (orange color) are tied to checkpointing and run only once.

#### Job Class
The job class determines where in the simulation a job is run.  Each job in the S_define file
requires a job class.  

<center>
<b>KEY: Job Class Category Colors used in Table SD_1</b>
<table>
<tr bgcolor="#c0c0c0"><th>Category Color</th><th>The job classes in Table SD_1 are categorized as one of the following:</th></tr>
<tr><td bgcolor="#ff8080">NON-CYCLIC</td><td>jobs that run once either at simulation startup or termination</td></tr>
<tr><td bgcolor="#add8e6">FRAME-BOUNDARY</td><td>jobs that run cyclically before/after the scheduled job loop</td></tr>
<tr><td bgcolor="#ccffff">INTEGRATOR</td><td>jobs that run first in the scheduled job loop for Trick state integration</td></tr>
<tr><td bgcolor="#ffff99">SELF-SCHEDULING</td><td>jobs in the scheduled job loop that must schedule themselves when to run</td></tr>
<tr><td bgcolor="#ccffcc">CYCLIC</td><td>jobs in the scheduled job loop that run cyclically according to their specified cycle time</td></tr>
<tr><td bgcolor="#e6ceff">FREEZE</td><td>jobs that are run during the freeze execution mode</td></tr>
<tr><td bgcolor="#ffc285">CHECKPOINT</td><td>jobs that are run when a checkpoint is dumped or loaded</td></tr>
</table>

<b>Table SD_1 Trick-Provided Job Classes</b>
<table>
<tr bgcolor="#c0c0c0"><th>Job Class Name</th><th>Description</th></tr>
<tr><td bgcolor="#ff8080">default_data</td><td>Module executes only once at simulation time = zero.
    Called before input file is read.</td></tr>
<tr><td bgcolor="#ff8080">initialization</td><td>Module executes only once at simulation time = zero.
    Called after input file is read.</td></tr>
<tr bgcolor="add8e6"><td colspan="2" align="center">BEGIN EXECUTION FRAME</td></tr>
<tr><td bgcolor="add8e6">top_of_frame</td><td>Module runs at the beginning of every execution frame, before the scheduled job loop,
    even before any child threads are started for the frame.</td></tr>
<tr bgcolor="ccffcc"><td colspan="2" align="center">BEGIN SCHEDULED JOB LOOP</td></tr>
<tr><td bgcolor="#ccffff">pre_integration</td><td>Runs only once before the integration loop.
    For example, in the case of a Runge Kutta 4, the derivative and integration jobs will be called
    four times during the integration steps.  A pre_integration job will execute a
    single time before this four step integration process has occurred.</td></tr>
<tr bgcolor="ccffff"><td colspan="2" align="center">BEGIN INTEGRATION LOOP</td></tr>
<tr><td bgcolor="#ccffff">derivative</td><td>Equations of motion (EOM) derivative function.</td></tr>
<tr><td bgcolor="#ccffff">integration</td><td>Equations of motion state integration function.</td></tr>
<tr bgcolor="ccffff"><td colspan="2" align="center">END INTEGRATION LOOP</td></tr>
<tr><td bgcolor="#ccffff">post_integration</td><td>Runs only once after the integration loop.</td></tr>
<tr><td bgcolor="#ccffff">dynamic_event</td><td>Runs after the integration loop
    (after any post_integration jobs). Provides a continuous time dependent equation whose root
    defines a discontinuous event in the system EOM, evaluation of function returns an
    estimated delta time to reach the root.</td></tr>
<tr><td bgcolor="ffff99">automatic</td><td>Module which reschedules its own next call time, and will run before other CYCLIC jobs in the frame.</td></tr>
<tr><td bgcolor="#ccffcc">environment</td><td>Time dependent boundary conditions (mother nature).</td></tr>
<tr><td bgcolor="#ccffcc">sensor</td><td> Simulated interface between dynamics and control simulation components.</td></tr>
<tr><td bgcolor="#ccffcc">sensor_emitter</td><td>Same as sensor, but for the emitter portion of an integrated
    multi-component sensor system.</td></tr>
<tr><td bgcolor="#ccffcc">sensor_reflector</td><td>Same as sensor, but for the reflector portion of an
    integrated multi-component sensor system.</td></tr>
<tr><td bgcolor="#ccffcc">sensor_receiver</td><td>Same as sensor, but for the receiver portion of an
    integrated multi-component sensor system.</td></tr>
<tr><td bgcolor="#ccffcc">scheduled</td><td>Typical flight software and hardware subsystems.</td></tr>
<tr><td bgcolor="#ccffcc">effector</td><td>Simulated interface between control and force generator
    simulation components.</td></tr>
<tr><td bgcolor="#ccffcc">effector_emitter</td><td>Same as effector, but for the action portion of an
    action-reaction effector system.</td></tr>
<tr><td bgcolor="#ccffcc">effector_receiver</td><td>Same as effector, but for the reaction portion of an
    action-reaction effector system.</td></tr>
<tr><td bgcolor="#ccffcc">logging</td><td>Simulation data recording or displaying.</td></tr>
<tr><td bgcolor="#ffff99">automatic_last</td><td>Module which reschedules its own next call time, and will run after other CYCLIC jobs in the frame.</td></tr>
<tr bgcolor="ccffcc"><td colspan="2" align="center">END SCHEDULED JOB LOOP</td></tr>
<tr><td bgcolor="add8e6">end_of_frame</td><td>Module runs at the end of every execution frame, after the scheduled job loop completes.</td></tr>
<tr bgcolor="add8e6"><td colspan="2" align="center">END EXECUTION FRAME</td></tr>
<tr><td bgcolor="#e6ceff">freeze_init</td><td>Module executes only once upon entering FREEZE mode.</td></tr>
<tr bgcolor="e6ceff"><td colspan="2" align="center">BEGIN FREEZE FRAME</td></tr>
<tr bgcolor="e6ceff"><td colspan="2" align="center">BEGIN FREEZE LOOP</td></tr>
<tr><td bgcolor="#e6ceff">freeze_scheduled</td><td>Module executes cyclically during simulation FREEZE mode according to its specified cycle time.</td></tr>
<tr bgcolor="e6ceff"><td colspan="2" align="center">END FREEZE LOOP</td></tr>
<tr><td bgcolor="#e6ceff">freeze</td><td>Module runs at end of every freeze frame, after the freeze loop completes.</td></tr>
<tr bgcolor="e6ceff"><td colspan="2" align="center">END FREEZE FRAME</td></tr>
<tr><td bgcolor="#e6ceff">unfreeze</td><td>Module executes only once upon leaving FREEZE mode before returning to RUN mode.</td></tr>
<tr><td bgcolor="#ffc285">checkpoint</td><td>Module executes only once before a checkpoint is dumped.</td></tr>
<tr><td bgcolor="#ffc285">post_checkpoint</td><td>Module executes only once after a checkpoint is dumped.</td></tr>
<tr><td bgcolor="#ffc285">preload_checkpoint</td><td>Module executes only once before a checkpoint is loaded.</td></tr>
<tr><td bgcolor="#ffc285">restart</td><td>Module executes only once after a checkpoint is loaded.</td></tr>
<tr><td bgcolor="#ff8080">shutdown</td><td>Module executes only once at simulation termination to allow a
    graceful shutdown.</td></tr>
</table>
</center>

#### Job Return Type

All integration class jobs must return an integer value which represents the current integration
pass identifier. If all integration passes are complete, the job must return a zero.

All dynamic_event class jobs must return a double precision value representing the estimated time
to go to the defined event, in seconds.

All other jobs managed by the Trick executive, not integration or dynamic_event, may return any
type. If a function is declared with an integer return value, the job must return a non-negative
integer or else the executive will assume an error occurred an immediately terminate the simulation.
If the job does not return an integer, Trick will not take any action based on a return value. Note
that initialization job return values are NOT checked by the executive.

#### Job Name

This field specifies the job name of the job as defined in the job’s source code.

C function and C++ member functions can be called as a jobs. Here is a quick example of a C and C++ 
calls.

```C++
%{
extern "C" {
    c_function() ;
}
%}

class MySimObject() : public Trick::SimObject {

    public:
        Ball my_ball ;

    MySimObject() {
        (1.0 , "scheduled") c_function() ;
        (1.0 , "scheduled") my_ball.print_state() ;
    }

}
```


### Job Calling Arguments

Job calling arguments adhere to C++ calling argument standards.

## Sim Object Methods

Methods may be defined within a sim object.  These methods may be used as simulation jobs.
A possible use for sim object methods would be to call non C/C++ code with minimal overhead from
the S_define file.

## Specifying Scheduled Loop Job Class Order

This section of the S_define (encapsulated by "job_class_order{...};) can be used to specify a new
scheduled loop job class order.  The user may simply re-order the existing job classes that exist or
can specify a new set of scheduled loop job classes. Job classes that are eligible for reassignment
are listed in Table SD_1 between automatic and automatic_last inclusive. The order they are shown
in the table is the default ordering. Note that if the user provides an ordering, classes that are 
not included in the ordering (excluding automatic and automatic_last) will not be handled by any scheduler,
 and therefore not executed in the sim.


```C++
job_class_order {
   my_job_class_1 ,
   my_job_class_2 ,
   scheduled ,
   my_job_class_3
};
```

## Simulation Object C++ properties

Sim objects are C++ objects.  They possess all of the capabilities of C++ objects.  This section
describes how to use some C++ features with sim objects.

### Simulation Object Instantiations

#### Multiple Instantiations

Sim objects are instantiated within the S_define file. They are regular class objects, and as such
are treated that way.  Sim objects may be multiply instantiated.  Multiply instantiated sim objects
works with both C and C++ models contained within the sim object.

```C++
class ballSimObject : public Trick::SimObject {
 public:
    Ball obj ;
    ballSimObject() {
        ("initialization") obj.state_init() ;
        ("derivative") obj.force_field() ;
        ("derivative") obj.state_deriv() ;
        ("integration", &my_integ) trick_ret = obj.state_integ() ;
        (10.0, "scheduled") trick_ret = obj.state_print() ;
    }
}

// Make 2 balls
ballSimObject ball ;
ballSimObject ball2 ;
```

#### Sim Object Constructor Arguments and Initializer Lists

Sim objects instantiations may take arguments.  These arguments may be used in the sim object's
initialization list.  An initialization list constructs member variables of the class.  They
are listed as a comma separated list after the declaration of a constructor.  Arguments passed
to the sim object constructor may be passed onto member variable constructors.

C structures may be zeroed out when included in the sim object's initialization list.

```C++
class ballSimObject : public Trick::SimObject {
 public:
    Ball obj ;
    C_STRUCT c_struct ;

    // passes int argument to obj constructor.  Zeros out c_struct.
    ballSimObject(int num) : obj(num) , c_struct() {
    } ;
}

// Sim object constructor requires an integer argument.
ballSimObject ball(5) ;
```

#### Sim Object Constructor Arguments and Job Control

Arguments to sim objects may also be used to control job execution.  Most items in the job
specification may be set to the value of an argument.

```C++
class ballSimObject : public Trick::SimObject {
 public:
    Ball obj ;
    ballSimObject(int phase, double cycle , const char * job_class) {
        (job_class) obj.state_init() ;
        Pphase ("derivative") obj.force_field() ;
        ("derivative") obj.state_deriv() ;
        ("integration", &my_integ) trick_ret = obj.state_integ() ;
        (cycle, "scheduled") trick_ret = obj.state_print() ;
    }
}

ballSimObject ball(1 , 10.0 , ~@~\initialization~@~]) ;
// This ball has different job properties than the first ball.
ballSimObject ball2( 2 , 5.0 , ~@~\default_data~@~] ) ;
```

### Multiple Constructors

Sim objects may define multiple constructors.  Each constructor may define different job
parameters or even an entirely different set of jobs.  Arguments to the sim object
instantiation determine which sim object constructor and which jobs are run.

```C++
class ballSimObject : public Trick::SimObject {
 public:
    Ball obj ;
    ballSimObject(int phase, double cycle , const char * job_class) {
        (job_class) obj.state_init() ;
        Pphase ("derivative") obj.force_field() ;
        ("derivative") obj.state_deriv() ;
        ("integration", &my_integ) trick_ret = obj.state_integ() ;
        (cycle, "scheduled") trick_ret = obj.state_print() ;
    }
    ballSimObject(const char * job_class) {
        (job_class) obj.state_init() ;
    }

}

ballSimObject ball(1 , 10.0 , "initialization") ;
ballSimObject ball2( "default_data" ) ;
```

### Sim Object Inheritance

Sim objects may inherit from other sim objects.  Jobs in the derived class will be run after those
of the base sim object class.  Both C and C++ jobs may be inherited.

```C++
class ballSimObject : public Trick::SimObject {
 public:
    Ball obj ;
    ballSimObject() {
        (10.0, "scheduled") trick_ret = obj.state_print() ;
    }
}

class anotherBallSimObject : public ballSimObject {
    public:
    anotherBallSimObject() {
         // This job will run after the above state_print()
        (10.0, "scheduled") another_print() ;
    }
}

anotherBallSimObject ball() ;
```

### Polymorphism in Sim Object jobs.

Polymorphism can be used to dynamically set objects at initialization or even change object types
during runtime.  Given an abstract class and two derived classes:

```C++
class Ball {
    public:
        virtual void print_type() = 0 ;
} ;

class Baseball : public Ball {
    public:
        virtual void print_type() ;
} ;

class Soccerball : public Ball {
    public:
        virtual void print_type() ;
} ;
```

We may use a Ball base pointer in the S_define.

```C++
class ballSimObject : public Trick::SimObject {
    public:
        Ball * ball_ptr ;
        ballSimObject() {
            (1.0 , "scheduled") ball_ptr->print_type() ;
        }
} ;

ballSimObject ball ;
```

`ball_ptr` is not instantiated when the simulation is compiled.  If nothing is assigned to `ball_ptr`
before the first scheduled call of `ball_ptr->print_type()` then the call will fail and the sim
will core.  We can allocate `ball_ptr` in the input file.  We can even change `ball_ptr` in the
middle of the simulation.

```python
ball.ball_ptr = trick.TMM_declare_var_s("Soccerball[1]")
trick.add_read(20.0 , """ball.ball_ptr = trick.TMM_declare_var_s("Baseball[1]")""")
```

## State Integration Specification

Trick manages state integration with exceptional flexibility. The integration specification
allows the developer to group the derivative, integration, and dynamic_event class modules
(for any combination of sim objects) for state integration using a particular integrator and
state integration time step. Some simulations will have several different sets of state
parameters, each set requiring a unique state integration scheme and integration time step.
Likewise, other simulations will require all the derivative class modules from a group of
sim objects to be executed before the integration class modules of the same sim object group.
The integration specification provides this capability.

The integration specification is of the following form:

```
integrate <integrator_name> (<integration_dt>) <sim_object_name> [,<sim_object_name_n>] ;
```

An alternative instantiation syntax which is pure C++ is of the form:

```C++
IntegLoopSimObject <integrator_name> (<integration_dt>, <sim_object_name> [,<sim_object_name_n>], NULL ) ;
```

This form must have NULL as the final argument to the instantiation.

The integrate tag is a reserved word for the CP. The <integration_dt> is a state integration
cycle time in seconds. At least one sim object name must be specified followed by any number
of additional sim object names separated by commas. An S_define can have at most one integrate
statement per sim object, and at least one integrate statement for all sim objects.

## Parameter Collection Mechanism

The parameter collection mechanism is probably the most difficult capability of the CP to 
understand and utilize. This capability is useful when the user wants a single job to handle
`n` number (either a known or unknown `n`) of parameters of the same type. The parameter
collection mechanism is an alternative for a variable calling argument list The collection
mechanism syntax in the S_define file is as follows:

```C++
collect <reference> = { } ;
```

or

```C++
collect <reference> = { <reference_1> [,<reference_n>] } ;
```

There is also a C code equivalent to adding collect references one at a time that may
be put in a create_connections section of the S_define file.  The advantage of this
method is that not all of the collects must be listed in a single collect statement.
This function call syntax may also be used in the input file to add collects at runtime.

```C++
void create_connections() {
    reference = add_collect( reference , reference_1 ) ;
    reference = add_collect( reference , reference_2 ) ;
}
```

The collect capability allows the developer to build a job which accesses an unknown number
of independent simulation parameters. For example, if designed correctly, a derivative class
module should be capable of incorporating any number of known and unknown (future capabilities)
external forces and torques without any code changes to the derivative module. The collection
mechanism stores the addresses of, and number of, any number of independent parameters in a
single pointer array. The derivative module can use this array to access each parameter in the
collection list. See Section 10.0 for programming requirements for this capability.

The collect statements in the S_define file must be supported by source code implemented by
the math model developer. This collect support code can reside in any function in the simulation,
including functions that are not listed in the S_define file. In general, for every collect
statement in the S_define file, there are two places where source code must be developed: a
data structure definition file (`*.h`) and a function source file (`*.c`).

As a real world example, orbital dynamics can include a large number of environmental effects
for high precision state propagation, or a small number of effects for general purpose state
propagation. A spacecraft EOM derivative module should be designed to include any number and
combination of known and unknown (future) effects. A low fidelity parameter collection for
external torques on the spacecraft might look like:

```C++
collect shuttle.orbital.rotation.external_torque[0] = {
            shuttle.aero.out.torque[0] } ;
```

A higher fidelity parameter collection might look like:

```C++
collect shuttle.orbital.rotation.external_torque[0] = {
            shuttle.aero.out.torque[0] ,
            shuttle.grav.out.gravity_gradient_torque[0] ,
            shuttle.solar_pressure.out.torque[0] } ;
```

For those cases when there are no parameters to collect:

```C++
collect shuttle.orbital.rotation.external_torque[0] = { } ;
```

The key here is that if a new external torque for the spacecraft is added to the simulation,
that torque can be accessed by the existing derivative module without code modification to the
derivative module. Note that all parameters listed must be of the same type and array dimension.

To use the parameter collection mechanism of the S_define file, the developer must perform three tasks:

1. from the example above, the external_torque parameter must be declared in its data structure
   definition file as a two dimensional void pointer, i.e. `void ** external_torque ;`,

2. a loop must be placed in the derivative module which accesses the collected parameters, and

3. the parameter collection statement must be added to the S_define.

The external_torque parameter must be declared as a two dimensional void pointer for two reasons.
First, the void type is not processed by the ICG. This means that this parameter cannot be recorded
for output or assigned data for input. If the type was any other type than void, the ICG would
assume the parameter required dynamic memory allocation and the resulting ICG generated code would
cause a fatal runtime error (usually accompanied by a core dump). Second, from an automatic code
generation viewpoint, the external_torque parameter is actually an unconstrained array of pointers,
where the pointers in the unconstrained array could be of any type (including data structure pointers);
i.e. the first pointer (*) of the declaration is the array dimension, the second is the address to
each of the collected parameters.

To make the collection mechanism work, the developer must add specific collection mechanism code to
their module. For the above example, the derivative module code might look like the following; the
text in bold indicates code which will be unchanged regardless of the parameters being collected:

```C++
#include "dynamics/v2/dynamics.h"
#include "sim_services/include/collect_macros.h"

int derivative_job( DYN_ROT * R ) {

    int i ;
    double **collect ;
    double total_torque[3] ;

    total_torque[0] = total_torque[1] = total_torque[2] = 0.0 ;

    /* typecast the void ** as a usable double** */
    collect = (double**)R->external_torque ;

    /*
       Loop on the number of collected items 
       from the above collect statement example:
       collect[0] -> shuttle.aero.out.torque
       collect[1] -> shuttle.grav.out.gravity_gradient_torque
       collect[2] -> shuttle.solar_pressure.out.torque
     */
    for( i = 0 ; i < NUM_COLLECT(collect) ; i++ ) {
        total_torque[0] += collect[i][0] ;
        total_torque[1] += collect[i][1] ;
        total_torque[2] += collect[i][2] ;
    }

    return( 0 ) ;
}
```

Several aspects of this example code which need mentioning are listed below.

1. A local pointer parameter must be declared of the same type as the parameters being
   collected, in this case the parameters being collected are double precision; hence,
   `double **collect ;`.

2. The `shuttle.orbital.rotation.external_torque` (actually a `void**`) is typecast as a
   `double**` and assigned to the local variable: `collect = (double**)R->external_torque ;`.

3. The number of parameters collected is saved in the first eight bytes before the
   address to the `external_torque` parameter. The conditional statement of the for loop
   demonstrates how the number of collected parameters is retrieved: `NUM_COLLECT(collect)`.
4. This example, and all other collection mechanism code implementations, assume the
   developer knows the type and array size of the parameters being collected. In this
   example, the parameters collected were single dimensioned double precision arrays with
   three elements per array.

## Create Connections

The create_connections section contains arbitrary code that is executed right after sim
object instantiations.  Code in this section is performed before any job of any job class.
The intended use of this section is to glue the sim objects together.  Sim objects that
need pointers to other sim objects may be assigned in the create_connections routine.
Default parameters may also be set such as defining a default simulation stop time.  Any
arbitrary code may be added to the create_connections section.

There may be multiple create_connection sections in the S_define file.  They will be
concatenated together in the order they appear in the S_define file.

```C++
class AsimObject : public Trick::SimObject {
    public:
        modelA a ;
        // This pointer points to a different sim object
        modelB * b_ptr ;

        AsimObject() {
            // This job requires type modelB from a different sim object
            (1.0 , "scheduled") a.job(b_ptr) ;
        }
} ;

class BsimObject: public Trick::SimObject {
    public:
        modelB b ;       
};

AsimObject A ;
BsimObject B ;

void create_connections() {

    // Connects the AsimObject and BsimObject together.
    A.b_ptr = &B.b

    // Sets a default stop time for the simulation.
    exec_set_terminate_time(300.0) ;
}
```

[Continue to Making The Simulation](Making-the-Simulation)
