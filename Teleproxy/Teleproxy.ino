/*
 Name:		Teleproxy.ino
 Created:	3/28/2026 5:37:44 PM
 Author:	Deci
*/

// ReSharper disable CppClangTidyModernizeMacroToEnum
// ReSharper disable CppInconsistentNaming

// ReSharper disable CppClangTidyClangDiagnosticHeaderHygiene
#include <Arduino.h>

#define SerialAT Serial1
#define SerialMon Serial

// #include <SIM7600Log.h>
#define SIM7600_LOG_VERBOSE_ERROR_CODES
// #define SIM7600_LOG_STREAM SerialMon
#define SIM7600_LOG_LEVEL 5  // Enable all logs including AT commands
// #define SIM7600_MODEM_TX_BUFFER_SIZE_B 512
// #define SIM7600_MODEM_RX_BUFFER_SIZE_B 512
#include <SIM7600Modem.h>

#include "SIM7600Log.h"


using namespace SIM7600;

Modem modem{&SerialAT};

// SIM::HttpClient http{&modem};
void modemReadyCB()
{
	SerialMon.println("Event: Modem is ready!");
}

void networkChangedCB(const bool registered, const RegStatus reg_status)
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
		Status status = modem.init();

		if (status == Status::Success) {
			SerialMon.println("Modem initialized successfully!");
			break;
		} else {
			SerialMon.print("Modem initialization failed: ");
			SerialMon.println(statusToString(status));
			SerialMon.println("Retrying in 1 seconds...");
			delay(1000);
		}
	}

	SerialMon.println("Done");

	// modem.stopHttpService();

	RegStatus reg   = {};
	Status    wfNet = {};
	
	wfNet           = modem.waitForNetworkRegistration(reg, 20000);
	SerialMon.print("Network reg: ");
	SerialMon.println(statusToString(wfNet));
	SerialMon.println((int)reg);

	String num = "123456789";

	auto status = modem.setSmsFormat(SmsFormat::Text);
	SerialMon.println(statusToString(status));
	// char buf[64] = {0};
	// sprintf(buf, "AT+CMGF=%d", static_cast<uint8_t>(SmsFormat::Text));
	// SIM7600_LOGD("ass", buf);

	// auto status = modem.sendATCmdAndWaitResp(buf,AT_OK,15000);
	// SerialMon.println(statusToString(status));


	status = modem.sendSMS(num, "hello world");
	SerialMon.println(statusToString(status));

	modem.powerOff();
}

void loop()
{
	modem.loop();
}
