# CppStateMachine
A pure C++ fluent state machine

---

## Usage

The template `IFiniteStateMachine<Trigger, State>` interface allows creation of finite state machine 
with `State` enum states that can be triggered with `Trigger` enum values

For example, let's say you were to model a motor turning on, speeding up, slowing down then shutting down

### States & Triggers

You can define the states in a `MotorStates` enum as

```cpp
enum MotorStates
{
	MotorStopped,
	MotorRunning,
	MotorAccelerating,
	MotorDecelerating
};
```

These states can be triggered by following enum values

```cpp
enum MotorTriggers
{
	MotorStop,
	MotorStart,
	MotorSpeedUp,
	MotorSpeedDown,
};
```

### Creating state machine

Include the single state machine header `export.h` & use define using namespace for convenience

```cpp
#include "export.h"

using namespace FSM; /* Optional convenience */
```

Create a new state machine with the initial state as `MotorStates::MotorStopped`

```cpp
CFiniteStateMachine<MotorTriggers, MotorStates> motor(MotorStates::MotorStopped);
```

### Configuring states and triggers

Configure states & their association with other states using the `Configure` method

```cpp
motor.Configure(MotorStates::MotorStopped)
```

Register a trigger to move to another state using the `AddTrigger` functionality

```cpp
motor.Configure(MotorStates::MotorStopped)
	->AddTrigger(MotorTriggers::MotorStart, MotorStates::MotorRunning);
```

Register an entry and/or exit callbacks. 
Entry callbacks are invoked when entering state being configured
Exit callbacks are invoked when exiting state being configured

```cpp
motor.Configure(MotorStates::MotorRunning)
	->OnEntry(OnMotorRunning)
	->OnExit(OnMotorNotRunning);
```

The callbacks can be function pointer, lambda methods or custom implementations of the `ICallback` interface

### Fluent interface

Configuration calls can be chained for a fluent interface design

```cpp
motor.Configure(MotorStates::MotorStopped)
	->AddTrigger(MotorTriggers::MotorStart, MotorStates::MotorRunning)
	->OnEntry(&motorStopped);

motor.Configure(MotorStates::MotorRunning)
	->AddTrigger(MotorTriggers::MotorSpeedUp,   MotorStates::MotorAccelerating)
	->AddTrigger(MotorTriggers::MotorSpeedDown, MotorStates::MotorDecelerating)
	->AddTrigger(MotorTriggers::MotorStop, MotorStates::MotorStopped)
	->OnEntry(&motorRunning);

motor.Configure(MotorStates::MotorAccelerating)
	->AddTrigger(MotorTriggers::MotorStart, MotorStates::MotorRunning)
	->OnEntry(&motorSpeedUp);

motor.Configure(MotorStates::MotorDecelerating)
	->AddTrigger(MotorTriggers::MotorStart, MotorStates::MotorRunning)
	->OnEntry(&motorSpeedDown);
```

### Firing Triggers

Once the state machine is constructed it state machine starts in the given initial state

A state change can only be triggered when states are configured

To fire a trigger use the `Fire` method on the state machine. In the motor example,

```cpp
motor.Fire(MotorStart);
motor.Fire(MotorSpeedUp);
motor.Fire(MotorSpeedDown);
motor.Fire(MotorStop);
```

