

#include "StateMachine.h"
#include "Arduino.h"

using namespace SmallRobots::StateMachine;

enum traffic_States : uint8_t {
    RED = 1,
    YELLOW_TO_GREEN = 2,
    GREEN = 3,
    YELLOW_TO_RED = 4
};

enum traffic_Events : uint8_t {
    BUTTON = 1
};


// TODO why is the cast needed on the transitions array?
State states[] = {
    State(RED, 1,               (Transition[]){ Transition(BUTTON, YELLOW_TO_GREEN) } ),
    State(YELLOW_TO_GREEN, 1,   (Transition[]){ Transition(SMALLROBOTS_STATEMACHINE_TIMEOUT_EVENT, GREEN) }),
    State(GREEN, 1,             (Transition[]){ Transition(SMALLROBOTS_STATEMACHINE_TIMEOUT_EVENT, YELLOW_TO_RED) }),
    State(YELLOW_TO_RED, 1,     (Transition[]){ Transition(SMALLROBOTS_STATEMACHINE_TIMEOUT_EVENT, RED) })
};

StateMachine traffic_light(traffic_States::RED, 4, states);



void setup() {
    Serial.begin(115200);
    Serial.println("Traffic light example");
};



void loop() {
    traffic_light.run();
    delay(100);             // run about 10x per second
    if (digitalRead(PIN_BUTTON) == HIGH) {
        traffic_light.on(traffic_Events::BUTTON, NULL);
    }
};
