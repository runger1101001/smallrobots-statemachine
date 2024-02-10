
#include "StateMachine.h"
#include "Arduino.h"

using namespace SmallRobots::StateMachine;



// simulated motor
class Motor {
public:
    void set_target(float left, float right) {
        Serial.print("Motor: left=");
        Serial.print(left);
        Serial.print(", right=");
        Serial.println(right);
    }
};

Motor motor;


enum robot_State : uint8_t {
    FORWARD = 1,
    TURN_LEFT = 2,
    TURN_RIGHT = 3,
    BACKWARD = 4
};

enum robot_Events : uint8_t {
    SENSOR_LEFT = 1,
    SENSOR_RIGHT = 2,
    SENSOR_FRONT = 3
};



State _FORWARD = State(FORWARD, 3, (Transition[]){ 
    Transition(SENSOR_LEFT, TURN_RIGHT), 
    Transition(SENSOR_RIGHT, TURN_LEFT), 
    Transition(SENSOR_FRONT, BACKWARD)
})
.run((state, event)=>{ motor.set_target(2.0f, 2.0f); });


State _TURN_RIGHT = State(TURN_RIGHT, 1, (Transition[]){ 
    Transition(SMALLROBOTS_STATEMACHINE_TIMEOUT_EVENT, FORWARD).timeout(1000)
})
.run((state, event)=>{ motor.set_target(2.0f, -2.0f); });


State _TURN_LEFT = State(TURN_LEFT, 1, (Transition[]){ 
    Transition(SMALLROBOTS_STATEMACHINE_TIMEOUT_EVENT, FORWARD).timeout(1000)
})
.run((state, event)=>{ motor.set_target(-2.0f, 2.0f); });


State _BACKWARD = State(BACKWARD, 1, (Transition[]){ 
    Transition(SMALLROBOTS_STATEMACHINE_TIMEOUT_EVENT, FORWARD).timeout(3000)
})
.run((state, event)=>{ motor.set_target(-2.0f, -2.0f); });



State states[] = { _FORWARD, _TURN_LEFT, _TURN_RIGHT, _BACKWARD };

StateMachine state_machine(robot_State::FORWARD, 4, states);



void setup() {
    Serial.begin(115200);
    Serial.println("Traffic light example");
};



void loop() {
    state_machine.run();
    
    // read sensors and send events to the state machine
    if (digitalRead(PIN_SENSOR_FORWARD) == HIGH) {
        state_machine.on(robot_Events::SENSOR_FRONT, NULL);
    }
    else if (digitalRead(PIN_SENSOR_LEFT) == HIGH) {
        state_machine.on(robot_Events::SENSOR_LEFT, NULL);
    }
    else if (digitalRead(PIN_SENSOR_RIGHT) == HIGH) {
        state_machine.on(robot_Events::SENSOR_RIGHT, NULL);
    }

    delay(100);             // run about 10x per second
};
