#include "CLI.hpp"

#include "Instances.hpp"

CLI::CLI() { setCommands(); }

CLI::~CLI() {}

// Register commands and their respective functions
void CLI::setCommands() {
    cmd_map["?"] = [this](int32_t argc, char** argv) { func_help(argc, argv); };
    cmd_map["-h"] = [this](int32_t argc, char** argv) { func_help(argc, argv); };
    cmd_map["help"] = [this](int32_t argc, char** argv) { func_help(argc, argv); };
    cmd_map["led"] = [this](int32_t argc, char** argv) { func_led(argc, argv); };
    cmd_map["flash"] = [this](int32_t argc, char** argv) { func_flash(argc, argv); };
    cmd_map["idle"] = [this](int32_t argc, char** argv) { func_idle(argc, argv); };
    cmd_map["show"] = [this](int32_t argc, char** argv) { func_show(argc, argv); };
}

void CLI::func_help(int32_t argc, char** argv) {
    std::string help_text =
        "\nUsage:  led\t[help] [on] [off]\n"
        "\t\t[breath] [blink] [rapid]\n"
        "\t\t[scale #] [level #] [add #]\n"
        "\n"
        "\tflash\t[save] [load]\n"
        "\n"
        "\tdac\t[help] [level *] [add *]\n"
        "\n"
        "\tshow\t[help] [one]\n";
    serialCOM.sendString(help_text);
}

void CLI::func_idle(int32_t argc, char** argv) {
    main_sm.process_event(shutdown{});
    main_sm.process_event(start{});
}

void CLI::func_led(int32_t argc, char** argv) {
    // Detailed Menu
    const char* help_text =
        "\nLED Functions:\n"
        "  on\t\tTurns ON LED\n"
        "  off\t\tTurns OFF LED\n"
        "  breath\t\tLED in breath effect mode\n"
        "  blink\t\tLED in slow blink mode\n"
        "  rapid\t\tLED in fast blink mode\n"
        "  scale #value\tSet LED dimmer scale\n"
        "  level #value\tSet LED light level\n"
        "  add #value\tIncrease or Decrease LED light level\n\n";

    // No Sub Command
    if (argc == 1) {
        led_user.toggle();
    }

    // Sub Command
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "help" || arg == "?" || arg == "-h") {
            serialCOM.sendString(help_text);
        } else if (arg == "on") {
            led_user.on();
        } else if (arg == "off") {
            led_user.off();
        } else if (arg == "blink") {
            led_user.blink();
        } else if (arg == "rapid") {
            led_user.rapid();
        } else if (arg == "breath") {
            led_user.breath();
        } else if (arg == "three") {
			led_user.three();
		}
    }
}

void CLI::func_flash(int32_t argc, char** argv) {
    // Detailed Menu
    const char* help_text =
        "\nFlash Functions:\n"
        "  load\t\tse setting from flash\n"
        "  unload\t\tSave setting to flash\n\n";

    // Sub Command
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "help" || arg == "?" || arg == "-h") {
            serialCOM.sendString(help_text);
        } else if (arg == "save") {
            flash.Save();
        } else if (arg == "load") {
            flash.Load();
        }
    }
}

void CLI::func_show(int32_t argc, char** argv) {
    const char* help_text =
        "\nShow Telemetry:\n"
        "  Stream Telemetry\n"
        "  one \tShow Telemetry Once\n\n";

    // No Sub Command
    if (argc == 1) {
        stream_sm.process_event(toggle{});
    }

    // Sub Command
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "help" || arg == "?" || arg == "-h") {
            serialCOM.sendString(help_text);
        } else if (arg == "one") {
            stream_sm.process_event(oneshot{});
        }
    }
}