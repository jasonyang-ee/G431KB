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
    enum class State { ON, OFF, IDLE, BREATH, BLINK, RAPID };
    State state_steady{State::OFF};
    State state_dynamic{State::BREATH};
    void actionOn();
    void actionOff();
    void actionBreath();
    void actionBlink();
    void actionRapid();

    void setSteadyState(State newState, void (LED::*f)());
    void setDynamicState(State newState);
    void setDynamicState(State newState, void (LED::*f)());

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

    std::unordered_map<State, std::function<void()>> steadyState;
    std::unordered_map<State, std::function<void()>> dynamicState;

};  // class LED

#endif /* APPLICATION_INC_LED */
