/*
 Name:		Teleproxy.ino
 Created:	3/28/2026 5:37:44 PM
 Author:	Deci
*/

// ReSharper disable CppClangTidyModernizeMacroToEnum
// ReSharper disable CppInconsistentNaming

#include <Arduino.h>

#define SerialAT Serial1
#define SerialMon Serial

#define SIM7600_LOG_LEVEL 5  // Enable all logs including AT commands
// #define SIM7600_MODEM_TX_BUFFER_SIZE_B 512
// #define SIM7600_MODEM_RX_BUFFER_SIZE_B 512
#define SIM7600_LOG_VERBOSE_ERROR_CODES
#define SIM7600_LOG_STREAM SerialMon

#include "SIM7600Modem.h"
#include "SIM7600HttpClient.h"

namespace SIM = SIM7600;

SIM::Modem modem{&SerialAT};

// SIM::HttpClient http{&modem};
void modemReadyCB()
{
	SerialMon.println("Event: Modem is ready!");
}

void networkChangedCB(const bool registered, const SIM7600::RegStatus reg_status)
{
	auto regStr = registered ? "Yes" : "No";
	auto regInt = static_cast<uint8_t>(reg_status);
	Serial.print("Event: Network changed! Registered: Status:");
	SerialMon.print(regStr);
	SerialMon.print(' ');
	SerialMon.println(regInt);
}

void setup()
{
	// using std::size_t;

	delay(2000);

	SerialMon.begin(115200);
	SerialAT.begin(115200, SERIAL_8N1);

	// modem.setModemReadyCallback(modemReadyCB);
	// modem.setNetworkChangedCallback(networkChangedCB);

	pinMode(PIN2, OUTPUT);
	digitalWrite(PIN2, HIGH);

	delay(2000);

	while (true) {
		SerialMon.println("Initializing modem...");
		SIM7600::Status status = modem.init();

		if (status == SIM7600::Status::Success) {
			SerialMon.println("Modem initialized successfully!");
			break;
		} else {
			SerialMon.print("Modem initialization failed: ");
			SerialMon.println((int) SIM7600::statusToString(status));
			SerialMon.println("Retrying in 5 seconds...");
			delay(5000);
		}
	}

	SerialMon.println("Done");

	// modem.stopHttpService();

	SIM::RegStatus  reg   = {};
	SIM7600::Status wfNet = {};

	wfNet = modem.waitForNetworkRegistration(reg, 9000);

	String num = "123456789";

	auto status = modem.setSmsFormat(SIM7600::SmsFormat::Text);
	SerialMon.println(SIM::statusToString(status));


	status = modem.sendSMS(num, "hello world");
	SerialMon.println(SIM::statusToString(status));

	// modem.powerOff();
}

void loop()
{
	modem.loop();
}
