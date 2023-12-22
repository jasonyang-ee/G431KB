#ifndef CORE_INC_SERIALCOM
#define CORE_INC_SERIALCOM

#include "algorithm"
#include "array"
#include "main.h"
#include "string"

#ifndef UART_BUFFER
#define UART_BUFFER 64
#endif

/**
 * @brief Construct a new Uart serial COM port
 * @warning setPort(&huartX) is required to run.
 * @note define UART_BUFFER to overwrite the default 64 byte
 * @note Need linker option: -u _printf_float
 */
class SerialCOM {
   public:
    SerialCOM();
    virtual ~SerialCOM();
    void setPort(UART_HandleTypeDef *);

    void sendLn();
    void sendString(std::string);
    void sendNumber(bool);


    /**
     * @brief Append single number to buffer.
     *
     * @param value all kinds of numbers.
     */
    template <class T>
    void sendNumber(T value) {
        m_buffer_msg.append(std::to_string(value));
    }

    /**
     * @brief Append array of numbers to buffer.
     *
     * @param items std::array of numbers.
     */
    template <class T, size_t N>
    void sendNumber(std::array<T, N> items) {
        for (auto &i : items) {
            m_buffer_msg.append(std::to_string(i));
            if (&i != &items.back()) m_buffer_msg.append(", ");
        }
    }

    bool sendOut();

    // Tx Rx Memory for DMA to fetch and push
	// rewrite to use std::array
	std::array<uint8_t, UART_BUFFER> m_tx_data; // UART_BUFFER may be re-defined in main.h
	std::array<uint8_t, UART_BUFFER> m_rx_data;

    // Port
    UART_HandleTypeDef *m_port;

    // Data
    uint16_t m_tx_data_size{};
    std::string m_buffer_msg{};
};

#endif /* CORE_INC_SERIALCOM */
