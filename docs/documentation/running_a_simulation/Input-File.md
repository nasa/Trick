| [Home](/trick) → [Documentation Home](../Documentation-Home) → [Running a Simulation](Running-a-Simulation) → Input File |
|------------------------------------------------------------------|

The primary interface between the simulation executable and the user is the runstream
input file. The Trick simulation input file syntax is Python.  All Python syntax rules
apply

Rather than discuss an explicit syntax definition (which would probably be more
confusing than informative), each specific capability of the input processor, and
its associated input file syntax, will be discussed.

## Accessing Simulation Parameters

The parameter naming convention for ALL input parameters is the parameter's actual
source code name. The following is a line from an input file :
```python
ball.obj.state.output.position[0] = 1.0 ;
```
In this example, ball is the sim object name in the S_define file for a sim object
which contains the data structure named obj, where the obj data structure declaration
is as follows:

```cpp
class BallSimObject : Trick::SimObject { 
...
  Ball obj ;
...
}

BallSimObject ball ;
```

`state` is a class member found in the `Ball` class.

```cpp
class Ball {
...
    public:
        BallState state; /**< -- Ball state object. */
...
};
```

`output` is a member of the `BallState` class, and finally `position` is
a member of the `BallStateOutput` class.

```cpp
class BallState {
...
  public:
   BallStateOutput output; /**< trick_units(--) User outputs.  */
...
};

class BallStateOutput {
...
    public:
        double position[2];  /**< trick_units(m)    X(horizontal), Y(vertical) position. */
...
};
```

Arrays of simulation parameters may be read and written to with one Python statement.

```python
"""
These variables are declared in the ball structure.
double da[3] ;
double daa[3][3] ;
"""

# Python lists are the equivalent of the C/C++ arrays.

ball.da = [ 1.0 , 2.0 , 3.0 ] 
print ball.da
# [ 1.0 , 2.0 , 3.0 ] is printed

ball.daa = [[ 1.0 , 0.0 , 0.0 ] , [ 0.0 , 1.0 , 0.0 ] , [0.0 , 0.0 , 1.0]] 
print ball.daa

# [[ 1.0 , 0.0 , 0.0 ] ,
#  [ 0.0 , 1.0 , 0.0 ] ,
#  [0.0 , 0.0 , 1.0]]
# is printed
```

## Accessing Simulation Enumerated Types

Global Enumerations are available through the `trick` module.

```python
# from sim_services/include/Flag.h

print trick.True trick.False trick.Yes trick.No
1 0 1 0
```

## Accessing Simulation Functions and Object Member functions

Almost all functions and public object methods are available to call from the Python input file.
Arguments must be filled in just as they would be in C/C++ code.  There is more information about what
Trick simulation services routines are available later in this chapter.

```python
# Trick simulation services routines are called by "trick.<function>".
trick.exec_get_sim_time()
trick.checkpoint(100.0)
trick.stop(300.0)

# C User model functions are also called by "trick.<function>".
trick.ball_print(ball.state)

# C++ User model class methods are called by referencing the full object path just like in C++
ball.obj.state.print_position()

```

When calling functions, intrinsic typed simulation variables, e.g. int or double, will not work directly
as intrisic typed arguments.

```cpp
// If we have a c function
void foo( double d) ;

// And a structure with a variable declared as this
double length ; /* (m) length */

```

This call in the input will not work

```python
# Will not work
foo(length)
```

The reason is that in python space the variable length is an object that contains both the value of length
and the units.  The built in python command `float()` will strip the units off leaving a double that can be
used in the function call.

```python
# Works
foo(float(length))
```

Structure and class variables do not carry around units, and therefore the units do not have to be removed.

## Creating New Objects and Allocating Memory

It is possible to create new objects and allocate new memory for structures directly in the Python
input file.  Three different ways are described below.

### 1. Call ```Trick::MemoryManager``` Allocation Routines Directly

The first method is to call the `Trick::MemoryManager` routines to allocate memory.  
There are 3 `Trick::MemoryManager` calls with varying arguments that can be used to allocate memory

```python
trick.TMM_declare_var_s("declaration")
trick.TMM_declare_var_1d("enh_type_spec", e_elems)
trick.alloc_type(e_elems , "enh_type_spec")

# Some examples using a c++ declaration
# double * foo ;
# All 3 of the following statments allocates the same amount of memory

foo = trick.TMM_declare_var_s("double[6]")
foo = trick.TMM_declare_var_1d("double", 6)
foo = trick.alloc_type(6 , "double")

# Some examples using a c++ declaration
# double ** food ;
# All 3 of the following statments allocates the same amount of memory

food = trick.TMM_declare_var_s("double *[3]")
food[0] = trick.TMM_declare_var_s("double [4]")
food[1] = trick.TMM_declare_var_s("double [5]")
food[2] = trick.TMM_declare_var_s("double [6]")

food = trick.TMM_declare_var_1d("double *", 3)
food[0] = trick.TMM_declare_var_1d("double", 4)
food[1] = trick.TMM_declare_var_1d("double", 5)
food[2] = trick.TMM_declare_var_1d("double", 6)

food = trick.alloc_type(3, "double *")
food[0] = trick.alloc_type(4, "double")
food[1] = trick.alloc_type(5, "double")
food[2] = trick.alloc_type(6, "double")
```

Memory allocated using the above routines are tracked by the memory manager and is checkpointable and data recordable.

### 2. Use a Factory Function
The benefit of this method is flexibility in how objects are initialized. For example, we might want to initialize our objects with a non-default constructor. So, the requirements for our factory function are:

1. Allocate a memory object via the Trick Memory Manager, and
2. Call a constructor to initialize the object (using placement-new)

#### A Few Words About Placement-new
In C++ one often instanciates a class object using the **new** operator, for example:

```
MyClass * p = new MyClass(a,b,c);
```

This form of **new**

1. allocates memory, and then
2. calls a constructor.

Another form of **new**, is called "placement-new". Rather than allocating and calling a constructor to initialize memory, placement-new simply calls a constructor to initialize memory that has already been allocated (e.g., from the Memory Manager). 

If ```p``` points to allocated memory, then we can initialize that memory with **placement-new** : 

```
new (p) MyClass(a,b,c); 
```

In our factory function we'll pass the object pointer we got from the Memory Manager to our placement-new call, to initialize it.

#### Example Factory Function from ```SIM_contact```
SIM_contact simulates collisions between moving balls (think "pool balls"). From the input file, one or more balls can be added to the simulation. Each ball is initialized with a mass, a size, a position, and a velocity. The Ball class also includes a (non-default) constructor.

```C++
class Ball {
    public:
        Ball(){}
        double pos[2];
        double vel[2];
        double mass;
        double radius;
        
        // A Non-Default Constructor
        Ball(double x, double y, double vx, double vy, double r, double m);
};
```
To create and initialize a new Ball object, we have the function ```make_Ball```.

```C++
// Factory function Implementation
Ball* make_Ball(double x, double y, double vx, double vy, double r, double m) {
    Ball* b = (Ball*)TMM_declare_var_s("Ball");
    return (new (b) Ball(x,y,vx,vy,r,m));
}
```

Because this function is bound to Python by SWIG, it can be called from the input file.
For example :

##### From ```RUN_Newtons_cradle/input.py```
```Python
dyn.contact.nballs = 7
dyn.contact.balls = trick.TMM_declare_var_1d("Ball*", dyn.contact.nballs)
dyn.contact.balls[0] = trick.make_Ball(-4.00, 0.0, 2.0, 0.0, 0.5, 1.0)
dyn.contact.balls[1] = trick.make_Ball(-1.00, 0.0, 0.0, 0.0, 0.5, 1.0)
dyn.contact.balls[2] = trick.make_Ball( 0.01, 0.0, 0.0, 0.0, 0.5, 1.0)
dyn.contact.balls[3] = trick.make_Ball( 1.02, 0.0, 0.0, 0.0, 0.5, 1.0)
dyn.contact.balls[4] = trick.make_Ball( 2.03, 0.0, 0.0, 0.0, 0.5, 1.0)
dyn.contact.balls[5] = trick.make_Ball( 7.00, 0.0, 0.0, 0.0, 1.0, 1000000.0)
dyn.contact.balls[6] = trick.make_Ball(-7.00, 0.0, 0.0, 0.0, 1.0, 1000000.0)
```

This creates and initializes seven *Ball* objects needed to configure a Newton's cradle.

### 3. Call the Wrapped Class Constructor Directly
The third method is to call the wrapped constructor of the class directly.  This is analogous to declaring local
variables in C/C++ routines.  And like local variables in C/C++ if the python variable goes out of scope in the
input file, then python will try and free the memory associated with the local object.  Memory allocated this
way is not checkpointable or data recordable.

For example if we are trying to instantiate a new C++ `Ball` object in the input file.

```python
# The new_ball_1 instantiation is at the top level, new_ball_1 will not be freed.
new_ball_1 = trick.Ball() ;

# The new_ball_2 instantiation is in the function foo.
# When foo returns new_ball_2 will be freed by python

def foo():
    new_ball_2 = trick.Ball() ;
```

To stop python from freeing this memory we must tell python that it does not own the memory.
This can be done in two ways.  1) Tell Python it does not own the memory by modifying the
`thisown` flag.  2) Use a non-constructor routine that allocates memory and returns that
to the Python variable.

```python
# In the above example, we can avoid new_ball_2 from being freed
# when foo returns by setting the thisown flag to 0 in the new_ball_2 object.

def foo():
    new_ball_2 = trick.Ball() ;
    new_ball_2.thisown = 0 ;

# Alternatively we could call a non-constructor C/C++ routine that returns a new Ball
# object to python.  The python interpreter does not sense it allocated anything and 
# will not free it.

"""
C++ code for get_new_ball_obj()

Ball * get_new_ball_obj() {
    return(new Ball) ;
}
"""

def foo():
    new_ball_2 = trick.get_new_ball_obj() ;

```

## Comments

Comments in Python come in two forms.

```python
# A single line comment starts with a '#' sign

"""
Multi line comments are enclosed in
three sets of double quotes.
"""
```

## Nested File Inclusion

There are several ways to include files in Python.

```python
# One way is to use the execfile command
exec(open("Modified_data/data_record.py").read())

# Another way is to make the included file a module and import it.
# Import search paths may be added using the sys.path.append command.

sys.path.append("/my/python/dir") ;
import my_new_module
```

## Local Python Variables

Local variables may be used anywhere in the Python input file.  Local variables will follow normal
Python scoping rules.  Shortcut variable names may be created to reference simulation variables.

```python
my_position = ball.obj.state.output.position

my_position[0] = 4.5
my_position[1] = 6.7

print ball.obj.state.output_position
# printout would read "4.5, 6.7"
```

## Environment Variables

Environment Variables are available through the Python `os.getenv` call

```python
print os.getenv("TRICK_CFLAGS")
```

## Measurement Units

Every input parameter has associated measurement units specified in its corresponding data
structure definition file declaration. It specifies the units for the internal source code to
use for that parameter. However, Trick users also have certain control over units specification
from the input file.

`trick.attach_units()` attaches a unit to a value or some Python objects.

```python
"""
This variables is declared in the ball structure.
double position[3] ;     /* (m) X,Y,Z position */
"""

# Assign X position to 2m
ball.position[0] = trick.attach_units( "m" , 2.0 )

# Automatic units conversion is done if the attached unit is compatible with the variable.
# Assign Y position to 2ft
ball.position[1] = trick.attach_units( "ft" , 2.0 )

# Error is raised.
ball.position[2] = trick.attach_units( "ft/s" , 3.0 )

# Units may be attached to python lists and assigned to the array with one statement
# Automatic units conversion is done on the entire list.
ball.position = trick.attach_units( "ft" , [1.0 , 2.0, 3.0] )

# Lists may even include values of different units.  Automatic units conversion is
# done element by element.
ball.position = [trick.attach_units( "ft" , 1.0 ) , trick.attach_units( "m" , 2.0 ) , trick.attach_units( "cm" , 3.0 )] 

```

Printing parameters in the Python script will include the attached units.

```pycon
>>> print ball.position
[1.0m , 2.0m , 3.0m]
```

## Time Based Input Processing

The input processor allows pieces of the input file to be processed at a later simulation time.
To process code at a later time call `trick.add_read(<time>, "<code_to_be_executed>")`. 

```python
# simple statement
trick.add_read(1.0 , "ball.obj.state.out.position[0] = 4.0") ;

# Use triple quotes for multi line input or code segments that include quotes.
trick.add_read(2.0 , """
ball.obj.state.out.position[0] = 4.0 
print "This is a quoted string inside the triple quotes"
"""

# Use a local python variable called read to get an appearance similar previous Trick input files
read = 4.0
trick.add_read(read , ... ) 

read += 0.2
trick.add_read(read , ... ) 

read = 5.0
trick.add_read(read , ... ) 
```

## Freeze the Simulation

To freeze a simulation call `trick.freeze([<freeze_time>])`.  `trick.freeze()` called with no
arguments will freeze immediately.  An optional freeze time may be provided to freeze some time
in the future.

```python
# Freezes immediately
trick.freeze()

# Freezes at an absolute time
trick.freeze(100.0)

# Freezes 5 seconds relative from the current sim_time
trick.freeze(trick.exec_get_sim_time() + 5.0)

```

## Checkpoint the Simulation

`trick.checkpoint()` called with no arguments will checkpoint immediately. To checkpoint a simulation call `trick.checkpoint(<checkpoint_time>)`.
An optional checkpoint time may be provided to checkpoint some time in the future. When you supply a name, you can save the checkpoint to a specific file. For example, `trick.checkpoint(<file_name>)` creates a checkpoint in a file with the given name immediately. `trick.checkpoint(<checkpoint_time>, <file_name>)` saves a checkpoint in
the described file at the given time. 

```python
# Checkpoints immediately
trick.checkpoint()

# Checkpoints immediately, saving to 'checkpoint_save'
trick.checkpoint("checkpoint_save")

# Checkpoints at an absolute time
trick.checkpoint(100.0)

# Checkpoints at an absolute time, saving to 'late_checkpoint'
trick.checkpoint(100.0, "late_checkpoint")

# Checkpoints 5 seconds relative from the current sim_time
trick.checkpoint(trick.exec_get_sim_time() + 5.0)

# Checkpoints to 'checkpoint.txt' immediately 
trick.checkpoint("checkpoint.txt")

# Checkpoints to 'checkpoint2.txt' at an absolute time
trick.checkpoint(50.0, "checkpoint.txt")
```

## Stopping the Simulation

To shutdown a simulation call trick.stop([<stop_time>]).  trick.stop() called with no
arguments will shutdown immediately.  An optional stop time may be provided to shutdown some time
in the future.

```python
# Stop immediately
trick.stop()

# Stop at an absolute time
trick.stop(100.0)

# Stop 5 seconds relative from the current sim_time
trick.stop(trick.exec_get_sim_time() + 5.0)
```

## Events and Malfunctions

Trick 10 events are a hybrid of Trick 07 events and malfunctions. A Trick 07 event has one or more conditions, one action, and is evaluated by the input processor. A Trick 07 malfunction also has one or more conditions (called triggers) that you can disable/enable, multiple actions, manual mode, and is evaluated before/after a specified job. Multiple conditions in malfunctions are ORed in 07, while multiple conditions in events can be specified by the user as being ORed or ANDed.  Here is the Python syntax showing how Trick 10 events implement all of this functionality.

For information on how Trick processes events during runtime, see [Event Processing](/trick/documentation/simulation_capabilities/Event-Manager).

### Basic Event Usage

```python
# Create an event that can be viewed and manipulated in MTV, and can be checkpointed
<event name> =  trick.new_event("<event name>")
# Alternatively, you can create an unnamed event (hidden in MTV and not checkpointable)
<event name> =  trick.new_event()

# Set the event condition
# (note that because there can be multiple conditions, you must specify a condition index starting at 0)
# The number of conditions an event can have is unlimited 0...n
# When an (enabled) event condition is true, we say it has "fired"
<event name>.condition(<index>, "<input text string>" [,"<optional comment displayed in mtv>"])

# Set the condition evaluation such that ANY fired condition will cause all of this event's (enabled) actions to run
# (conditions are ORed -- this is the default)
<event name>.condition_any()
# Set the condition evaluation such that ALL (enabled) conditions must fire to cause all of event's (enabled) actions to run
# (conditions are ANDed)
<event name>.condition_all()

# Set the frequency that the event condition(s) will be evaluated at
<event name>.set_cycle(<cycle time in seconds>)

# Set the event action to occur when any/all condition(s) have fired
# (again specify an index because there can be multiple actions)
# The number of actions an event can have is unlimited 0...n
<event name>.action(<index>, "<input text string>", [,"<optional comment displayed in mtv>"])

# Set the event active so that it is processed (default is not active, and event is also deactivated when fired)
<event name>.activate()     # the opposite would be <event name>.deactivate()
# Note that your event will be a one-time thing unless you put the activate statement in your action
# This behavior can be overridden using "manual mode" described below

# Add the event to the input processor's list of events (it will be processed at top of frame before scheduled jobs)
trick.add_event(<event name>)

# Tell trick whether to terminate the sim if an error occurs while parsing Python code. Defaults to False
trick.terminate_on_event_parse_error(<True|False>)
```

### Advanced Event (Malfunction) Usage

```python
# Insert the event before/after a "target" job so that it is evaluated then (as opposed to top of frame)
# Note that the event cycle is ignored in this case, the event takes on the cycle time of the target job
# Also note that an event can be inserted before/after multiple target jobs,
# in which case it will be evaluated multiple times per frame
# The job name string is the "sim_object.job_name" from the S_define file.
# The job instance is used to specify which instance of the job to use if identically named jobs are
# multiply defined in a sim object.  If the argument is not specified, the first instance of the is the default.
trick.add_event_before(<event name>, "<job name string>" [, <job instance> = 1])
trick.add_event_after(<event name>, "<job name string>" [, <job instance> = 1])

# Remove an event from everywhere it's been added (you can then subsequently add the event again if you want)
trick.remove_event(<event_name>)

# Delete an event permanently from the sim so that you can no longer add it again
trick.delete_event(<event_name>)

# Use a model variable or job as a condition
# It is more optimal to use model code as a condition, because of the python parsing involved in a normal condition()
# Variable (the variable's value will be taken as the condition boolean) :
<event name>.condition_var(<index>, "<variable name>" [,"<optional comment displayed in mtv>"])
# Job (the job's return value will be taken as the condition boolean) :
<event name>.condition_job(<index>, "<job name>" [,"<optional comment displayed in mtv>"])
# Any combination of condition(), condition_var(), or condition_job() can be used for your malfunction condition(s).
# NOTE: If the job is something you created just for use in malfunctions (e.g. it is not a scheduled job),
        then it must be specified once and only once in the S_define file as a "malfunction" class job.

# Set an event condition's "hold" on so that when it fires, it stays in the fired state, causing actions to run every cycle
# (the default is hold off so fire only once)
<event name>.condtion_hold_on(<index>)    # the opposite would be <event name>.condition_hold_off(<index>)
# Note that you would still need "activate" in your action otherwise hold_on won't have any effect.

# Disable a condition from being evaluated (default is enabled)
<event name>.condition_disable(<index>)    # the opposite would be <event name>.condition_enable(<index>)

# Use a model job as an action
# It is more optimal to use model code as an action, because of the python parsing involved in a normal action()
<event name>.action_job(<index>, "<job name>" [,"<optional comment displayed in mtv>"])
# Turn a model job ON/OFF as an action
<event name>.action_job_on(<index>, "<job name>" [,"<optional comment displayed in mtv>"])
<event name>.action_job_off(<index>, "<job name>" [,"<optional comment displayed in mtv>"])
# Any combination of action(), action_job(), action_job_on(), or action_job_off() can be used for your malfunction action(s).
# NOTE: If the job is something you created just for use in malfunctions (e.g. it is not a scheduled job),
        then it must be specified once and only once in the S_define file as a "malfunction" class job.

# Disable an action from being run (default is enabled)
<event name>.action_disable(<index>)     # the opposite would be <event name>.action_enable(<index>)

# Manually fire the event once now, so that its actions will run once now
# (this event is now in "manual mode" and its conditions will not be evaluated until manual_done commanded)
<event name>.manual_fire()

# Manually set an event as fired and hold on, so that its actions will run each cycle
# (this event is now in "manual mode" and its conditions will not be evaluated until manual_done commanded)
<event name>.manual_on()

# Manually set an event as not fired, so that its actions will not run
# (this event is now in "manual mode" and its conditions will not be evaluated until manual_done commanded)
<event name>.manual_off()

# Exit "manual mode" for this event and return to normal processing of its conditions
<event name>.manual_done()
```

### Setting variables synchronously: Real Time Variable Injector

You can also use `rti_add`/`rti_fire` commands in your event action syntax (or as standalone commands in the input file or
via the variable server) to set variables. See "Real Time Variable Injector".

### Accessing the current Event state

```python
<event name>.condtion_fired(<index>)          # boolean:  test if a particular event condition fired this cycle
<event name>.condition_fired_count(<index>)   # integer:  number of times a particular event condition has fired
<event name>.condtion_fired_time(<index>)     # double:   last sim time a particular event condition has fired
<event name>.action_ran(<index>)              # boolean:  test if a particular event action ran this cycle
<event name>.action_ran_count(<index>)        # integer:  number of times a particular event action has run
<event name>.action_ran_time(<index>)         # double:   last sim time a particular event action has run
<event name>.fired                            # boolean:  test if the event conditions setup evaluated to true this cycle
<event name>.fired_count                      # integer:  number of times this event has fired
<event name>.fired_time                       # double:   last sim time this event has fired
<event name>.ran                              # boolean:  test if any event action ran this cycle
<event name>.ran_count                        # integer:  number of times this event has run an action
<event name>.ran_time                         # double:   last sim time this event has run an action
<event name>.manual                           # boolean:  test if this event is in "manual mode"
<event name>.manual_fired                     # boolean:  test if this event was fired manually this cycle
```

### Event Example

Hopefully this example shows the various things you can do using events without being too confusing. Even the event components themselves can be queried and changed.

```python
# In this event example:
#    evaluate velocity before integration...
#    if it is over 50, print a message
#    if it is over 100, reset the velocity to 0 after data recording is done for this frame
#    if our sim has gone past 500 seconds, do not reset velocity when it goes over 100,
#       but instead shutdown when velocity goes over 200

vel_event = trick.new_event("vel_event")
vel_event.condition(0, "trick.as_pyfloat(ball.obj.state.output.velocity) > 50.0")
vel_event.condition(1, "trick.as_pyfloat(ball.obj.state.output.velocity) > 100.0")
vel_event.action(0, """
print "VELOCITY = ", ball.obj.state.ouput.velocity
vel_event.activate()
""")
vel_event.activate()

over100_event = trick.new_event("over100_event")
over100_event.condition(0, "vel_event.condition_fired(1)")
over100_event.action(0, "trick.as_pyfloat(ball.obj.state.output.velocity) = 0.0")
over100_event.action(1, "trick.stop()")
over100_event.action(2, "over100_event.activate()")
over100_event.action_disable(1)
over100_event.activate()

change_event = trick.new_event("change_event")
change_event.condition(0, "trick.exec_get_sim_time() > 500.0")
change_event.action(0, """
over100_event.action_disable(0)
over100_event.action_enable(1)
vel_event.condition(1, "trick.as_pyfloat(ball.obj.state.output.velocity) > 200.0")
""")
change_event.activate()

trick.add_event_before(vel_event, "ball.obj.state_integ")
trick.add_event_before(change_event, "ball.obj.state_integ")
trick.add_event_after(over100_event, "data_record_group1.Ball")
```

## Trick Specific Python Usage

Many of Trick's functions can be called using Python commands in the input file (or via the variable server)
to perform various operations or to customize/configure Trick.

## I Just Want to Know How Do I Set this Value...

[Continue to Runtime GUIs](runtime_guis/Runtime-GUIs)
