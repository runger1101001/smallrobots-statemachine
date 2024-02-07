
#pragma once

#include <stdint.h>

#if !defined(SMALLROBOTS_STATEMACHINE_MAX_EVENTS)
#define SMALLROBOTS_STATEMACHINE_MAX_EVENTS 10    
#endif

#if !defined(SMALLROBOTS_STATEMACHINE_MAX_PARALLEL_STATES)
#define SMALLROBOTS_STATEMACHINE_MAX_PARALLEL_STATES 10
#endif

#define SMALLROBOTS_STATEMACHINE_NO_STATE 0xFF
#define SMALLROBOTS_STATEMACHINE_NO_EVENT 0xFF
#define SMALLROBOTS_STATEMACHINE_TIMEOUT_EVENT 0xFE

namespace SmallRobots::StateMachine {

    class State;


    class Transition {
    public:
        Transition(uint8_t event, uint8_t target_state, void (*action)(State& state, Event event) = nullptr, bool (*guard)(State& state, Event event) = nullptr, uint8_t num_emits = 0, uint8_t* emits = nullptr);
        Transition& emits(uint8_t num_emits, uint8_t* emits);
        Transition& timeout(uint32_t timeout_ms);
        Transition& guard(bool (*guard)(State& state, Event event));
        Transition& action(void (*action)(State& state, Event event));
        uint8_t event;
        uint8_t target_state;
        bool (*_guard)(State& state, Event event) = nullptr;
        void (*_action)(State& state, Event event) = nullptr;
        uint8_t* _emits;
        uint8_t num_emits;
        uint32_t timeout_ms;
    };


    class State {
    public:
        State(uint8_t id, void (*run)(State& state) = nullptr);
        State(uint8_t id, uint8_t num_transitions, Transition* transitions, void (*run)(State& state) = nullptr);
        State& transitions(uint8_t num_transitions, Transition* transitions);
        State& run(void (*run)(State& state));
        uint8_t id;
        void (*_run)(State& state) = nullptr;
        Transition* _transitions;
        uint8_t num_transitions;
    };



    class Event {
    public:
        uint8_t id;
        void* data;
    };



    class StateMachine {
    public:
        StateMachine(uint8_t initial_state, uint8_t num_states, State* states);
        void _global_transitions(uint8_t num_global_transitions, Transition* global_transitions);

        void emit(uint8_t event, void* data = nullptr);
        void run();
        bool in_state(uint8_t state);
    protected:
        bool pop(Event& event);
        bool push(uint8_t event, void* data = nullptr);
        uint8_t events_available();

        State* find_state(uint8_t id);

        uint8_t num_states;
        State* states;

        uint8_t num_global_transitions;
        Transition* _global_transitions;

        uint8_t num_active;
        State* active_states[SMALLROBOTS_STATEMACHINE_MAX_PARALLEL_STATES];

        Event event_queue[SMALLROBOTS_STATEMACHINE_MAX_EVENTS];
        uint32_t pushed = 0;
        uint32_t popped = 0;
    };


}; // namespace SmallRobots::StateMachine
