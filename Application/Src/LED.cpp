#include "LED.hpp"

LED::LED(int32_t period, int32_t freq) : ratio(period / 100), frequency(freq) {
    // clang-format off
    transitions = {
        {State::OFF, Event::TOGGLE, State::ON, nullptr, [this]() { actionOn(); }},
        {State::ON, Event::TOGGLE, State::OFF, nullptr, [this]() { actionOff(); }},
        {State::BREATH, Event::SCHEDULE, State::BREATH, nullptr, [this]() { actionBreath(); }},
        {State::BLINK, Event::SCHEDULE, State::BLINK, [this] { return guardBlink(); }, [this]() { actionToggle(); }},
        {State::RAPID, Event::SCHEDULE, State::RAPID, [this] { return guardRapid(); }, [this]() { actionToggle(); }},
        {State::BREATH, Event::TOGGLE, State::OFF, nullptr, [this]() { actionOff(); }},
        {State::BLINK, Event::TOGGLE, State::OFF, nullptr, [this]() { actionOff(); }},
        {State::RAPID, Event::TOGGLE, State::OFF, nullptr, [this]() { actionOff(); }}
	};
    entries = {
		{State::ON, nullptr, [this]() { actionOn(); }},
		{State::OFF, nullptr, [this]() { actionOff(); }},
		{State::BREATH, nullptr, [this]() { actionBreath(); }},
		{State::BLINK, nullptr, [this]() { actionToggle(); }},
		{State::RAPID, nullptr, [this]() { actionToggle(); }}
	};
    // clang-format on

    // Set initial state
    currentState = State::BREATH;
    for (auto entry : entries) {
        if (std::get<0>(entry) == currentState) {
            std::get<2>(entry)();
            break;
        }
    }
}

LED::~LED() {}

// Public Methods
void LED::setPort(__IO uint32_t *CCR) { port = CCR; }
void LED::on() { setState(State::ON); }
void LED::off() { setState(State::OFF); }
void LED::breath() { setState(State::BREATH); }
void LED::blink() { setState(State::BLINK); }
void LED::rapid() { setState(State::RAPID); }
void LED::toggle() { triggerEvent(Event::TOGGLE); }
void LED::scheduler() { triggerEvent(Event::SCHEDULE); }

// Define Actions
void LED::actionOn() { *port = on_percent / scale * ratio; }
void LED::actionOff() { *port = 0; }
void LED::actionToggle() {
    if (*port == 0)
        *port = on_percent / scale * ratio;
    else
        *port = 0;
}
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

// Define Guards
bool LED::guardBlink() {
    if (blink_timer++ > 5) {
        blink_timer = 0;
        return true;
    }
    return false;
}
bool LED::guardRapid() {
    if (rapid_timer++ > 1) {
        rapid_timer = 0;
        return true;
    }
    return false;
}