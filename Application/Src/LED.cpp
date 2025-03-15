#include "LED.hpp"

LED::LED(uint16_t period, uint16_t freq) : ratio(period / 100), frequency(freq) {
    // clang-format off
	entries = {
		{State::ON, nullptr, [this]() { actionOn(); }},
		{State::OFF, nullptr, [this]() { actionOff(); }},
		{State::BREATH, nullptr, [this]() { actionBreath(); }},
		{State::BLINK, nullptr, [this]() { actionToggle(); }},
		{State::RAPID, nullptr, [this]() { actionToggle(); }},
		{State::THREE, nullptr, [this]() { actionOff(); }}
	};
    transitions = {
        {State::OFF, Event::TOGGLE, State::ON, nullptr, [this]() { actionOn(); }},
        {State::ON, Event::TOGGLE, State::OFF, nullptr, [this]() { actionOff(); }},
        {State::BREATH, Event::SCHEDULE, State::BREATH, nullptr, [this]() { actionBreath(); }},
        {State::BLINK, Event::SCHEDULE, State::BLINK, [this] { return guardBlink(); }, [this]() { actionToggle(); }},
        {State::RAPID, Event::SCHEDULE, State::RAPID, [this] { return guardRapid(); }, [this]() { actionToggle(); }},
		{State::THREE, Event::SCHEDULE, State::OFF, [this] { return guardThree(); }, [this]() { actionOff(); }},
        {State::BREATH, Event::TOGGLE, State::OFF, nullptr, [this]() { actionOff(); }},
        {State::BLINK, Event::TOGGLE, State::OFF, nullptr, [this]() { actionOff(); }},
        {State::RAPID, Event::TOGGLE, State::OFF, nullptr, [this]() { actionOff(); }}
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
void LED::three() { setState(State::THREE); }
void LED::toggle() { triggerEvent(Event::TOGGLE); }
void LED::scheduler() { triggerEvent(Event::SCHEDULE); }

// Define Actions
void LED::actionOn() { *port = on_percent / dimmer * ratio; }
void LED::actionOff() { *port = 0; }
void LED::actionToggle() {
    if (*port == 0)
        *port = on_percent / dimmer * ratio;
    else
        *port = 0;
}
void LED::actionBreath() {
    if (breath_percent <= 100 && breath_direction) {
        breath_percent += 4;
    }
    if (breath_percent == 100 && breath_direction) {
        breath_direction = false;
    }
    if (breath_percent > 0 && !breath_direction) {
        breath_percent -= 10;
    }
    if (breath_percent == 0 && !breath_direction) {
        breath_direction = true;
    }
    *port = breath_percent / dimmer * ratio;
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
bool LED::guardThree() {
    // blink 3 times
	if (three_counter == three_count) {
		three_counter = 0;
		return true;
	}
    if (three_timer_off++ == 2) {
        actionToggle();
    }
    if (three_timer_on++ == 5) {
		three_timer_on = 0;
		three_timer_off = 0;
        three_counter++;
        actionToggle();
    }
	return false;
}

void LED::setDimmer(uint16_t value) { dimmer = value; }