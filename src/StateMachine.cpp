

#include "./StateMachine.h"
#include <Arduino.h>

namespace SmallRobots::StateMachine {



    State::State(uint8_t id, void (*run)(State& state)) {
        this->id = id;
        this->run = run;
    };



    State::State(uint8_t id, uint8_t num_transitions, Transition* transitions, void (*run)(State& state)) {
        this->id = id;
        this->num_transitions = num_transitions;
        this->_transitions = transitions;
        this->_run = run;
    };



    State& State::transitions(uint8_t num_transitions, Transition transitions[]) {
        this->num_transitions = num_transitions;
        this->_transitions = transitions;
        return *this;
    };



    State& State::run(void (*run)(State& state)) {
        this->_run = run;
        return *this;
    };



    Transition::Transition(uint8_t event, uint8_t target_state, void (*action)(State& state, Event event), bool (*guard)(State& state, Event event), uint8_t num_emits, uint8_t* emits) {
        this->event = event;
        this->target_state = target_state;
        this->_guard = guard;
        this->_action = action;
        this->num_emits = num_emits;
        this->_emits = emits;
    };



    Transition& Transition::emits(uint8_t num_emits, uint8_t* emits) {
        this->num_emits = num_emits;
        this->_emits = emits;
        return *this;
    };



    Transition& Transition::timeout(uint32_t timeout_ms) {
        this->timeout_ms = timeout_ms;
        return *this;
    };



    Transition& Transition::guard(bool (*guard)(State& state, Event event)) {
        this->_guard = guard;
        return *this;
    };



    Transition& Transition::action(void (*action)(State& state, Event event)) {
        this->_action = action;
        return *this;
    };



    StateMachine::StateMachine(uint8_t initial_state, uint8_t num_states, State* states) {
        this->num_states = num_states;
        this->states = states;
        this->active_states[0] = find_state(initial_state);
        this->num_active = 1;
    };




    void StateMachine::global_transitions(uint8_t num_global_transitions, Transition* global_transitions) {
        this->num_global_transitions = num_global_transitions;
        this->_global_transitions = global_transitions;
    };




    void StateMachine::on(uint8_t event, void* data) {
        push(event, data);
    };



    bool StateMachine::in_state(uint8_t state) {
        for (int i=0; i<num_active; i++) {
            if (active_states[i]!=nullptr && active_states[i]->id==state)
                return true;
        }
        return false;
    };



    void StateMachine::run() {
        Event event;
        uint8_t N = num_active;
        uint8_t E = events_available();
        uint8_t replaced = SMALLROBOTS_STATEMACHINE_NO_STATE;
        while (E-->0) {
            pop(event);
            // check the global transitions
            for (int i=0; i<num_global_transitions; i++) {
                if (_global_transitions[i].event == event.id) {
                    // TODO: check the guard
                    if (_global_transitions[i].action != nullptr)
                        _global_transitions[i].action(*active_states[0], event);
                    if (_global_transitions[i].target_state != SMALLROBOTS_STATEMACHINE_NO_STATE) {
                        State* state = find_state(_global_transitions[i].target_state);  
                        if (!in_state(state->id)) {
                            // TODO check max parallel states
                            active_states[num_active++] = state;
                        }
                    }
                    if (_global_transitions[i].emits != nullptr) {
                        for (int j=0; j<_global_transitions[i].num_emits; j++)
                            push(_global_transitions[i].emits[j]);
                    }
                }
            }
            // check the active states' transitions
            for (int i=0; i<N; i++) {
                for (int j=0; j<active_states[i]->num_transitions; j++) {
                    if (active_states[i]->transitions[j].event == event.id) {
                        // TODO: check the guard
                        if (active_states[i]->transitions[j].action != nullptr)
                            active_states[i]->transitions[j].action(*active_states[i], event);
                        if (active_states[i]->transitions[j].target_state != SMALLROBOTS_STATEMACHINE_NO_STATE) {
                            State* new_state = find_state(active_states[i]->transitions[j].target_state);
                            if (new_state!=nullptr && !in_state(new_state->id))
                                active_states[i] = new_state;
                            else {
                                // TODO handle this case properly!
                                active_states[i] = nullptr;
                            }
                        }
                        if (active_states[i]->transitions[j].emits != nullptr) {
                            for (int k=0; k<active_states[i]->transitions[j].num_emits; k++)
                                push(active_states[i]->transitions[j].emits[k]);
                        }
                    }
                    else {
                        // TODO: check the timeout
                    }
                }
            }
        }
        // run the active state's run methods, if any
        for (int i=0; i<num_active; i++)
            if (active_states[i]!=nullptr && active_states[i]->run!=nullptr)
                active_states[i]->run(*active_states[i]);
    };



    bool StateMachine::pop(Event& event) {
        bool result = false;
        noInterrupts();
        if (pushed > (popped+1)) {
            if (popped < pushed - SMALLROBOTS_STATEMACHINE_MAX_EVENTS)
                popped = pushed - SMALLROBOTS_STATEMACHINE_MAX_EVENTS;
            event = event_queue[popped%SMALLROBOTS_STATEMACHINE_MAX_EVENTS];
            popped += 1;
            result = true;
        }
        interrupts();
        return result;
    };



    bool StateMachine::push(uint8_t event, void* data) {
        bool result = false;
        noInterrupts();
        if (pushed - popped < SMALLROBOTS_STATEMACHINE_MAX_EVENTS) {
            event_queue[pushed%SMALLROBOTS_STATEMACHINE_MAX_EVENTS].id = event;
            event_queue[pushed%SMALLROBOTS_STATEMACHINE_MAX_EVENTS].data = data;
            pushed += 1;
            result = true;
        }
        interrupts();
        return result;
    };



    uint8_t StateMachine::events_available() {
        uint32_t result;
        noInterrupts();
        result = pushed - popped;
        interrupts();
        return result;
    };



    State* StateMachine::find_state(uint8_t id) {
        for (int i = 0; i < num_states; i++) {
            if (states[i].id == id) {
                return &states[i];
            }
        }
        return nullptr;
    };





};  // namespace SmallRobots::StateMachine

