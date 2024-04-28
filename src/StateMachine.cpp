
#include "./StateMachine.h"
#include "Arduino.h"
#include <cstdarg>
#include "./SmallRobotDebug.h"


namespace SmallRobots {





    State::State(String name) : name(name) {
    };
    State::~State() {
    };


    Transition::Transition(String name, String event, State& from, State& to) : name(name), event(event), from(from), to(to) {
    };
    Transition::~Transition() {
    };


    StateMachine::StateMachine() {
    };

    StateMachine::~StateMachine() {
    };


    State State::ANY_STATE{"*"};


    void StateMachine::start(){
        if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: starting");
        current_state = initial_state;
        if (current_state==nullptr) {
            current_state = all_states[0];
        }
        if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: entering "+current_state->name);
        current_state->timestamp = millis();
        if (current_state->enter != nullptr) {
            if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: calling enter for "+current_state->name);
            current_state->enter();
        }
    };



    void StateMachine::trigger(String event) {
        if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: incoming event "+event);
        // TODO disconnect trigger function from trigger execution via a queue
        for (size_t i=0;i<all_transitions.size();i++) {
            Transition* t = all_transitions[i];
            if (t!=nullptr && t->event == event && ( &t->from == current_state || t->from == State::ANY_STATE)) {
                transition(*t);
                return;
            }
        }
        if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: no transition for event "+event);
    };


    void StateMachine::tick() {
        if (current_state->timeout > 0) {
            unsigned long now = millis();
            if (now - current_state->timestamp > current_state->timeout) {
                if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: timeout for "+current_state->name);
                current_state->timestamp = now; // reset timestamp
                trigger(current_state->name+"_timeout"); // send timeout event only to ourselves
            }
        }
    };


    void StateMachine::transition(Transition& t) {
        if (t.guard != nullptr) {
            if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: checking guard for "+t.name);
            if (!t.guard()) {
                if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: guard for "+t.name+" false, transition suppressed");
                return;
            }
        }
        if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: exiting "+current_state->name);
        if (current_state->exit != nullptr) {
            if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: calling exit for "+current_state->name);
            current_state->exit();
        }
        if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: transition "+t.name);
        if (t.on != nullptr) {
            if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: calling on for "+t.name);
            t.on();
        }
        if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: entering "+t.to.name);
        current_state = &t.to;
        current_state->timestamp = millis();
        if (current_state->enter != nullptr) {
            if (debug && smallrobot_debug_print!=nullptr) smallrobot_debug_print->println("StateMachine: calling enter for "+current_state->name);
            current_state->enter();
        }
    };

};
