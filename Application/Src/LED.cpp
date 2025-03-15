#include "LED.hpp"

LED::LED(int32_t period, int32_t freq) : ratio(period / 100), frequency(freq) {
    // Event Handlers
    steadyState[State::OFF] = [this]() { setSteadyState(State::ON, &LED::actionOn); };
    steadyState[State::ON] = [this]() { setSteadyState(State::OFF, &LED::actionOff); };
    dynamicState[State::IDLE] = [this]() { setDynamicState(State::IDLE); };
    dynamicState[State::BREATH] = [this]() { setDynamicState(State::BREATH, &LED::actionBreath); };
    dynamicState[State::BLINK] = [this]() { setDynamicState(State::BLINK, &LED::actionBlink); };
    dynamicState[State::RAPID] = [this]() { setDynamicState(State::RAPID, &LED::actionRapid); };
}

LED::~LED() {}

void LED::setPort(__IO uint32_t *CCR) { port = CCR; }

void LED::on() {
    setDynamicState(State::IDLE);
    setSteadyState(State::ON, &LED::actionOn);
}
void LED::off() {
    setDynamicState(State::IDLE);
    setSteadyState(State::OFF, &LED::actionOff);
}
void LED::toggle() {
    setDynamicState(State::IDLE);
    steadyState[state_steady]();
}
void LED::scheduler() { dynamicState[state_dynamic](); }
void LED::breath() {
    setSteadyState(State::OFF, &LED::actionOff);
    setDynamicState(State::BREATH, &LED::actionBreath);
}
void LED::blink() { setDynamicState(State::BLINK, &LED::actionBlink); }
void LED::rapid() { setDynamicState(State::RAPID, &LED::actionRapid); }

void LED::actionOn() { *port = on_percent / scale * ratio; }
void LED::actionOff() { *port = 0; }
void LED::actionBreath() {
    if (breath_percent < 40 && breath_direction) {
        breath_percent += 3;
    }
    if (breath_percent >= 40 && breath_direction) {
        breath_percent += 4;
    }
    if (breath_percent >= 100 && breath_direction) {
        breath_direction = false;
    }
    if (breath_percent >= 60 && !breath_direction) {
        breath_percent -= 8;
    }
    if (breath_percent < 60 && !breath_direction) {
        breath_percent -= 5;
        if (breath_percent < 0) {
            breath_percent = 0;
        }
    }
    if (breath_percent <= 0 && !breath_direction) {
        breath_direction = true;
    }
    *port = breath_percent / scale * ratio;
}
void LED::actionBlink() {
    if (blink_timer++ > 5) {
        *port = on_percent / scale * ratio;
        blink_timer = 0;
        toggle();
    }
}
void LED::actionRapid() {
    if (rapid_timer++ > 1) {
        *port = on_percent / scale * ratio;
        rapid_timer = 0;
        toggle();
    }
}

void LED::setSteadyState(State newState, void (LED::*f)()) {
    state_steady = newState;
    (this->*f)();
}

void LED::setDynamicState(State newState) { state_dynamic = newState; }

void LED::setDynamicState(State newState, void (LED::*f)()) {
    state_dynamic = newState;
    (this->*f)();
}