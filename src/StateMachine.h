
#pragma once

#include <functional>
#include <vector>
#include <Arduino.h>

namespace SmallRobots {

#define STATE(name) State name{#name}
#define TRANSITION(name, event, from, to) Transition name{#name, #event, from, to}



    class State {
    friend class StateMachine;
    public:
        State(String name);
        ~State();

        bool operator==(const State& other) const {
            if (this == &ANY_STATE || &other == &ANY_STATE) return true;
            return this == &other;
        }

        static State ANY_STATE;
        
        String name;
        uint32_t timeout;
        std::function<void()> enter = nullptr;
        std::function<void()> exit = nullptr;
    protected:
        uint32_t timestamp;
    };



    class Transition {
    public:
        Transition(String name, String event, State& from, State& to);
        ~Transition();
        String name;
        String event;
        State& from;
        State& to;
        std::function<void()> on = nullptr;
        std::function<bool()> guard = nullptr;
    };


    class StateMachine {
    public:
        StateMachine();
        ~StateMachine();
        
        void trigger(String event);
        void tick();
        void transition(Transition& t);

        void start();

        std::vector<Transition*> all_transitions;
        std::vector<State*> all_states;
        State* initial_state;

        bool debug = false;
    protected:
        State* current_state = nullptr;
    };

}; // namespace SmallRobots
