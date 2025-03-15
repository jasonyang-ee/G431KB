#ifndef APPLICATION_INC_ACLI
#define APPLICATION_INC_ACLI

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class CLI {
   private:
    void func_help(int32_t argc, char** argv);
    void func_led(int32_t argc, char** argv);
    void func_flash(int32_t argc, char** argv);
    void func_idle(int32_t argc, char** argv);
    void func_show(int32_t argc, char** argv);

   private:
    void setCommands();
    size_t rx_size{};
    std::string rx_cache{};
    std::unordered_map<std::string, std::function<void(int32_t, char**)>> cmd_map{};

   public:
    CLI();
    virtual ~CLI();

    void saveCache(std::string rx) { rx_cache = rx; }

    bool parse() {
        // tokenizing rx_cache by space
        std::vector<std::string> tokens;
        std::string token;
        for (char c : rx_cache) {
            if (c == ' ' || c == '\n' || c == '\r') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }

        // parse command and arguments
        if (!tokens.empty()) {
            std::string command = tokens[0];
            // using first token as command
            auto arguments = cmd_map.find(command);
            if (arguments != cmd_map.end()) {
                int32_t argc = tokens.size();
                std::vector<char*> argv;
                for (auto& t : tokens) {
                    argv.push_back(&t[0]);
                }
                // Invoke command functions by first token
                arguments->second(argc, argv.data());
            } else {
                return false;
            }
        }
        return true;
    }
};

#endif /* APPLICATION_INC_ACLI */
