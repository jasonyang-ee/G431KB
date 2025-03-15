#ifndef APPLICATION_INC_LED
#define APPLICATION_INC_LED

#include <functional>
#include <unordered_map>

#include "main.h"

/**
 * @brief Construct a new LED::led object
 * @param pwm_period(opt) Set PWM reload period.
 * @param freq(opt) Set scheduler running frequency in Hz.
 * @warning setPort(&htimX->CCRX) is required to run.
 */

class LED {
   public:
    LED(int32_t, int32_t);
    virtual ~LED();
    void setPort(__IO uint32_t*);

    void on();
    void off();
    void toggle();
    void breath();
    void blink();
    void rapid();
    void scheduler();

    uint16_t getLevel() { return breath_percent; }

   private:
    enum class State { ON, OFF, BREATH, BLINK, RAPID };
    enum class Event { ON, OFF, TOGGLE, BREATH, BLINK, RAPID, SCHEDULE };

    void actionOn();
    void actionOff();
	void actionToggle();
    void actionBreath();
    void actionBlink();
    void actionRapid();

	bool guardBlink();
	bool guardRapid();

    void triggerEvent(Event event) {
        if (transitions[currentState].count(event)) {
            State nextState = transitions[currentState][event];

            // Check if a guard function exists for this transition
            if (guards[currentState].count(event)) {
                if (!guards[currentState][event]()) {
                    return;
                }
            }

            currentState = nextState;  // Move to next state
            actions[currentState]();   // Execute state action
		}
	}

    void setState(State state) {
        currentState = state;
        actions[currentState]();  // Execute state action
    }

   private:
    __IO uint32_t* port;  // Ex: htim3.Instance->CCR2 for Timer3 Channel2
    int32_t ratio{1};
    int32_t scale{1};       // light scale
    uint16_t frequency{0};  // thread scheduler frequency

    int16_t on_percent{100};
    bool breath_direction{true};
    int16_t breath_percent{0};
    uint16_t blink_timer{0};
    uint16_t rapid_timer{0};

    State currentState{State::BREATH};
    std::unordered_map<State, std::unordered_map<Event, State>> transitions;
    std::unordered_map<State, std::function<void()>> actions;
	std::unordered_map<State, std::unordered_map<Event, std::function<bool()>>> guards;

};  // class LED

#endif /* APPLICATION_INC_LED */
