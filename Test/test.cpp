// write a test program to test the class

#include <iostream>
#include <string>
#include "CLI.hpp"
#include "serialcom.hpp"
int main() {

	std::string commands[] = {
		"help",
		"led",
		"flash",
		"idle",
		"dac",
		"show",
		"date"
	};

	SerialCOM serial;


	CLI cli;
	for (auto &command : commands) {
		// copy command into serial m_rx_data using std::array method
		for (int i = 0; i < command.length(); i++) {
			serial.m_rx_data[i] = command[i];
		}
		cli.setSize(command.length());
		cli.parse();
		return 1;
	}
return 0;}