

#include "Thread.hpp"

#include "Instances.hpp"

Thread::Thread() {
    // Template for creating task
    // auto _task = [](void *arg) { static_cast<Thread *>(arg)->(); };
    // xTaskCreate(_task, "", 512, this, 2, &_Handle);

    auto t1 = [](void *arg) { static_cast<Thread *>(arg)->telemetry_human(); };
    xTaskCreate(t1, "telemetry_human", 300, this, 2, &telemetry_human_handle);
    vTaskSuspend(telemetry_human_handle);

    auto a1 = [](void *arg) { static_cast<Thread *>(arg)->idle(); };
    xTaskCreate(a1, "idle", 400, this, 2, &idle_handle);
    vTaskSuspend(idle_handle);

    auto a2 = [](void *arg) { static_cast<Thread *>(arg)->dacUpdate(); };
    xTaskCreate(a2, "dacUpdate", 400, this, 2, &dac_handle);
    vTaskSuspend(dac_handle);

    auto s0 = [](void *arg) { static_cast<Thread *>(arg)->init(); };
    xTaskCreate(s0, "system init", 500, this, 6, &init_handle);

    auto s1 = [](void *arg) { static_cast<Thread *>(arg)->schedule_20Hz(); };
    xTaskCreate(s1, "schedule 20Hz", 128, this, 5, &schedule_20Hz_handle);

    auto s11 = [](void *arg) { static_cast<Thread *>(arg)->serial_send(); };
    xTaskCreate(s11, "serial send out", 64, this, 0, &serial_send_handle);

    auto s12 = [](void *arg) { static_cast<Thread *>(arg)->parse(); };
    xTaskCreate(s12, "cli parsing", 800, this, 3, &parse_handle);
}

Thread::~Thread() {}

void Thread::telemetry_human() {
    while (1) {
        serialCOM.sendString("\n\n\n\n\n\n\n\n");
        serialCOM.sendString("\nDAC Target Value:\t");
        serialCOM.sendNumber(dac.getLevel());
        serialCOM.sendString("\nADC Sensing Value:\t");
        serialCOM.sendNumber(adc.volt_from_dac);
		serialCOM.sendString("\nDate:\t");
		serialCOM.sendNumber(rtc.getDate());
		serialCOM.sendString("\nTime:\t");
		serialCOM.sendNumber(rtc.getTime());
		serialCOM.sendLn();

        // State machine debug
        // serialCOM.sendString("\n\nCurrent State:\t");
        // main_sm.visit_current_states([](auto s) { serialCOM.sendString(s.c_str()); });
        // serialCOM.sendLn();

        stream_sm.process_event(finish{});
    }
}

/*---------------------------------------------------------------------------

        Applications

---------------------------------------------------------------------------*/

void Thread::idle() {
    while (1) {
        dac.setLevel(0);
        dac.off();
        main_sm.process_event(finish{});
    }
}

void Thread::dacUpdate() {
    while (1) {
        serialCOM.sendString("dac updating\n");
        dac.schedule();
        main_sm.process_event(finish{});
    }
}

/*---------------------------------------------------------------------------

        System initialization and periodic update

---------------------------------------------------------------------------*/

void Thread::init() {
    while (1) {
        serialCOM.sendString("RTOS Init\n");
        dac.init();
        flash.Load();

        main_sm.process_event(start{});
		main_sm.process_event(start{});

        vTaskSuspend(NULL);
    }
}

void Thread::schedule_20Hz() {
    while (1) {
        led_user.scheduler();
        vTaskDelay(50);
    }
}
void Thread::serial_send() {
    while (1) {
        ulTaskNotifyTake(pdTRUE, 300);
        serialCOM.commit();
    }
}

void Thread::parse() {
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (!cli.parse()) {
			serialCOM.sendString("Command not found\n");
		}
    }
}

void Thread::idling_start() { serialCOM.sendString("ideling start function called\n"); }

void Thread::dac_start() {
    serialCOM.sendString("dac start function called\n");
    serialCOM.sendNumber(dac.sineWaveArray);
	dac.sine();
}
void Thread::dac_shutdown() {
    serialCOM.sendString("dac shutdown function called\n");
    dac.off();
}
