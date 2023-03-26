#include "CLI.hpp"

#include "instances.hpp"

CLI::CLI() {}

CLI::~CLI() {}

void CLI::init() {
    lwshell_init();
    lwshell_set_output_fn(&CLI::output);

    lwshell_register_cmd("help", &CLI::help, NULL);
    lwshell_register_cmd("led", &CLI::led, NULL);
    lwshell_register_cmd("flash", &CLI::flash, NULL);
    lwshell_register_cmd("show", &CLI::show, NULL);
}

bool CLI::parse() {
    if (lwshell_input(serialCOM.m_rx_data, m_cmd_size) == lwshellOK)
        return true;
    return false;
}

void CLI::setSize(uint16_t size) { m_cmd_size = size; }

void CLI::output(const char* str, lwshell* lwobj) { serialCOM.sendString(str); }

int32_t CLI::led(int32_t argc, char** argv) {
    // Detailed Menu
	const char* help_text =
        "\nLED Functions:\n"
        "  on\tTurns ON LED\n"
        "  off\tTurns OFF LED\n"
        "  breath\tLED in breath effect mode\n"
        "  blink\tLED in slow blink mode\n"
        "  rapid\tLED in fast blink mode\n\n";

	// Sub Command
    if (argc == 2) {
        if (!strcmp(argv[1], "help")) serialCOM.sendString(help_text);
        if (!strcmp(argv[1], "on")) led_user.on();
        if (!strcmp(argv[1], "off")) led_user.off();
        if (!strcmp(argv[1], "breath")) led_user.breath();
        if (!strcmp(argv[1], "blink")) led_user.blink();
        if (!strcmp(argv[1], "rapid")) led_user.rapid();
    }

	// Sub Command with values
    if (argc == 3) {
        if (!isdigit(*argv[2])) return 1;		// Check if number on input
        if (!strcmp(argv[1], "level"))
            led_user.setLevel(atof(argv[2]));	// Apply input number
        if (!strcmp(argv[1], "scale"))
            led_user.setScale(atof(argv[2]));	// Apply input number
    }
    return 0;
}

int32_t CLI::flash(int32_t argc, char** argv) {
    // Detailed Menu
	const char* help_text =
        "\nFlash Functions:\n"
        "  load\tUse setting from flash\n"
        "  unload\tSave setting to flash\n\n";

	// Sub Command
    if (argc == 2) {
		if (!strcmp(argv[1], "help")) serialCOM.sendString(help_text);
		if (!strcmp(argv[1], "unload")) xTaskResumeFromISR(thread.app_1_Handle);
		if (!strcmp(argv[1], "load")) xTaskResumeFromISR(thread.app_2_Handle);
	}

    return 0;
}

int32_t CLI::show(int32_t argc, char** argv) {
	xTaskResumeFromISR(thread.app_3_Handle);
    return 0;
}

int32_t CLI::help(int32_t argc, char** argv) {
    const char* help_menu =
        "\nUsage:  led\t[help] [on] [off]\n"
        "\t\t[breath] [blink] [rapid]\n"
        "\n"
        "\tflash\t[load] [unload]\n"
        "\n"
        "\tshow\n";
    serialCOM.sendString(help_menu);
    return 0;
}
