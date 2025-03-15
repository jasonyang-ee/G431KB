#ifndef APPLICATION_INC_LED
#define APPLICATION_INC_LED

#include <functional>
#include <unordered_map>

#include "main.h"

/**
 * @brief Construct a new LED::led object
 * @param period Set PWM reload period.
 * @param frequency Set scheduler running frequency in Hz.
 * @warning setPort(&htimX->CCRX) is required to run.
 */

class LED {
   public:
    LED(uint16_t period, uint16_t frequency);
    virtual ~LED();

    // Public Methods
    void setPort(__IO uint32_t*);
    void on();
    void off();
    void toggle();
    void breath();
    void blink();
    void rapid();
	void three();
    void scheduler();
    void setDimmer(uint16_t dimmer);

   private:
    // State Machine Definition
    enum class State { ON, OFF, BREATH, BLINK, RAPID, THREE };
    enum class Event { ON, OFF, TOGGLE, BREATH, BLINK, RAPID, SCHEDULE };
    void actionOn();
    void actionOff();
    void actionToggle();
    void actionBreath();
    void actionBlink();
    void actionRapid();
    bool guardBlink();
    bool guardRapid();
    bool guardThree();

   private:
    // State Machine Mechanism
    // Transition definition: (CurrentState, Event, NextState, GuardFunction, ActionFunction)
    // StateEntry definition: (TargetState, GuardFunction, ActionFunction)
    using GuardFunc = std::function<bool()>;
    using ActionFunc = std::function<void()>;
    using Entry = std::tuple<State, GuardFunc, ActionFunc>;
    using Transition = std::tuple<State, Event, State, GuardFunc, ActionFunc>;

    State currentState;
    std::vector<Entry> entries;
    std::vector<Transition> transitions;

    void triggerEvent(Event event) {
        // Find a valid transition for the current state and event
        auto it = std::find_if(transitions.begin(), transitions.end(), [&](const Transition& t) {
            return std::get<0>(t) == currentState && std::get<1>(t) == event;
        });

        if (it != transitions.end()) {
            GuardFunc guard = std::get<3>(*it);
            ActionFunc action = std::get<4>(*it);
            State nextState = std::get<2>(*it);

            // Check guard condition if it exists
            if (!guard || guard()) {
                currentState = nextState;
                if (action) action();
            }
        }
    }

    void setState(State state) {
        currentState = state;
        for (auto entry : entries) {
            if (std::get<0>(entry) == currentState) {
                std::get<2>(entry)();
                break;
            }
        }
    }

   private:
    __IO uint32_t* port;    // Ex: htim3.Instance->CCR2 for Timer3 Channel2
    uint16_t ratio{1};      // Max CCR value ratio to 100%
    uint16_t frequency{0};  // thread scheduler frequency
    uint16_t dimmer{1};     // User custom dimmer value 0 - 10

    bool breath_direction{true};
    uint16_t on_percent{100};
    uint16_t breath_percent{0};
    uint16_t blink_timer{0};
    uint16_t rapid_timer{0};
    uint16_t three_timer_on{0};
    uint16_t three_timer_off{0};
	uint16_t three_counter{0};
    uint16_t three_count{3};

};  // class LED

#endif /* APPLICATION_INC_LED */
