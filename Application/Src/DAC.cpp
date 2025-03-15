#include "DAC.hpp"

#include "Instances.hpp"
#include "cmath"

CustomDAC::CustomDAC() {}

CustomDAC::~CustomDAC() {}

void CustomDAC::setPort(DAC_HandleTypeDef *port, uint32_t channel) {
    m_port = port;
    m_channel = channel;
}

void CustomDAC::setVoltRef(double value) { m_vref = value; }

void CustomDAC::init() { HAL_DAC_Start(m_port, m_channel); }

void CustomDAC::setLevel(double value) {
    m_level = value;
    on();
}

void CustomDAC::addLevel(double value) {
    m_level += value;
    if (m_level > m_vref) m_level = m_vref;
    if (m_level < 0) m_level = 0;
    on();
}

double CustomDAC::getLevel() { return m_level; }

void CustomDAC::on() { setState(CustomDAC::State::s_on); }

void CustomDAC::off() { setState(CustomDAC::State::s_off); }

void CustomDAC::breath() { setState(CustomDAC::State::s_breath); }

void CustomDAC::sine() { setState(CustomDAC::State::s_sine); }

void CustomDAC::generateSineWave(double amplitude, double frequency, double samplingRate) {
	m_amplitude = amplitude;
	m_frequency = frequency;
	m_samplingRate = samplingRate;
	for (int i = 0; i < 1000; i++) {
		double time = i / samplingRate;
		sineWaveArray[i] = amplitude * std::sin(2 * M_PI * frequency * time);
	}
}


void CustomDAC::schedule() {
    if (m_state == CustomDAC::State::s_breath) {
        m_level = abs(sin(HAL_GetTick() % m_breeath_period)) * 3.3;
    }
	applyLevel();
}

void CustomDAC::setState(uint8_t cmd) { setState(static_cast<CustomDAC::State>(cmd)); }

uint8_t CustomDAC::getState() { return static_cast<uint8_t>(m_state); }

// Private Functions

void CustomDAC::applyLevel() {
    uint32_t count_value = static_cast<uint32_t>(m_level * 4096 / m_vref);
    HAL_DAC_SetValue(m_port, m_channel, m_alignment, count_value);
}

void CustomDAC::zeroLevel() { HAL_DAC_SetValue(m_port, m_channel, m_alignment, 0); }

void CustomDAC::setState(CustomDAC::State cmd) {
    switch (cmd) {
        case CustomDAC::State::s_off:
            zeroLevel();
            break;
        case CustomDAC::State::s_on:
            applyLevel();
            break;
        case CustomDAC::State::s_breath:
			zeroLevel();
            break;
        case CustomDAC::State::s_sine:
            break;
    }
    m_state = cmd;
}
