# SmallRobots StateMachine Library

Arduino library implementing a simple state machine engine.

Define events, states and their transitions, and use the engine to run the state machine. It's simple to set up and simple to use.

## State machine paradigm

In the state machine paradigm used by this library, the `StateMachine` is the high level object through which the system is used. Initialize the `StateMachine` statically and during your `setup()` method and, use it from your code by calling its `run()` method regularly, and sending it events. Events can be supplied to the state machine at any time (also from interrupts), and all processing occurs within the call to `run()` (which should not be called from an interrupt).

The `StateMachine` is initialized with a set of states. Each state has an id, by which it is referenced, and a set of transitions. In addition, the `StateMachine` can have global transitions not attached to any state.

The `StateMachine` has a set of active states. The active states change as their transitions are trigged by events or timeouts occur. The `StateMachine` is initialized with a set of initially active states.

States can define a `run` callback, which is called once per `StateMachine.run()` on all active states. The `run` callback is optional. A state may define no transitions.

A state with no transitions and no `run` callback is considered a terminal state. A `StateMachine` whose active states are all terminal states can only receive events on global transitions. A state machine with only terminal active states and no global transitions can be considered terminated.
The set of active states can be empty, in which case the state machine can only receive events on global transitions. A state machine with no global transitions and no active states is considered terminated.

The transitions are triggered by an event, and identify a target state to which the state machine will transition the active state when the event is received. The transitions can define `guard` and `action` callbacks. The `guard` callback is called before the transition occurs, and can prevent the transition by returning false. The `action` callback is called when the transition occurs. The transition can be associated with `emits`, a set of events which are emitted when the transition occurs.

After a transition is completed the active state is replaced with the transitions target state, *but only if the active states do not already contain the target*.

A state may have more than one transition for the same event, in which case the first transition replaces the active state with it's target state, and subsequent transitions for the same event add their target state to the set of active states, *if the target state is not already active*.

A transition may define a NO_STATE target state, in which case the active state is removed from the set of active states when the transition is triggered. If the NO_STATE target is not the first transition to be triggered on this event, it's `guard` and `action` are called normally, and its `emits` events are emitted, but it has no effect on the active states.

A transition may define a timeout, in milliseconds. If the timeout is defined, and its duration elapses before the transitions event is received, the transition is triggered by the timeout. A transition may define the special `TIMEOUT` event id (0xFE) as trigger, in which case it *must* also define a timeout value. Such transitions are pure timeout transitions and will not be triggered by other events. A state may define multiple timeout transitions - although usually the lowest timeout would always trigger the transition, the transition `guard` could prevent the triggered timeout transition from occurring and thereby giving one of the other timeout transitions a chance. Transition timeout timestamps are reset whenever the transition is triggered (whether it actually occurs or not), and whenever the state becomes active.

Note that global transitions with timeout are useful, and can for example be used to easily schedule regular events, or re-activate otherwise terminal `StateMachines`.

