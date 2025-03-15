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
    // State Machine Definition
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

   private:
    // State Machine Mechanism
    // Transition definition: (CurrentState, Event, NextState, GuardFunction, ActionFunction)
    using GuardFunc = std::function<bool()>;
    using ActionFunc = std::function<void()>;
    using Transition = std::tuple<State, Event, State, GuardFunc, ActionFunc>;
    using Entry = std::tuple<State, GuardFunc, ActionFunc>;

    State currentState;
    std::vector<Transition> transitions;
    std::vector<Entry> entries;

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
    __IO uint32_t* port;  // Ex: htim3.Instance->CCR2 for Timer3 Channel2
    int32_t ratio{1};
    int32_t scale{1};       // light scale
    uint16_t frequency{0};  // thread scheduler frequency

    int16_t on_percent{100};
    bool breath_direction{true};
    int16_t breath_percent{0};
    uint16_t blink_timer{0};
    uint16_t rapid_timer{0};

    // std::unordered_map<State, std::unordered_map<Event, State>> transitions;
    // std::unordered_map<State, std::function<void()>> actions;
    // std::unordered_map<State, std::unordered_map<Event, std::function<bool()>>> guards;

};  // class LED

#endif /* APPLICATION_INC_LED */
