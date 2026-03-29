/*
 Name:		Teleproxy.ino
 Created:	3/28/2026 5:37:44 PM
 Author:	Deci
*/

// ReSharper disable CppClangTidyModernizeMacroToEnum
// ReSharper disable CppInconsistentNaming

#include <Arduino.h>
#include <IPAddress.h>
#include <string.h>
#include <WString.h>

#define SerialAT Serial1
#define SerialMon Serial

#define SIM7600_MODEM_TX_BUFFER_SIZE_B 512
#define SIM7600_MODEM_RX_BUFFER_SIZE_B 512

#define SIM7600_LOG_LEVEL 5  // Enable all logs including AT commands

#include <SIM7600Modem.h>
#include "SIM7600HttpClient.h"

namespace SIM = SIM7600;

SIM::Modem      modem(&SerialAT);
SIM::HttpClient http(&modem);


void setup()
{
	SerialMon.begin(115200);
	SerialAT.begin(115200, SERIAL_8N1);

	pinMode(PIN2, OUTPUT); // Power control pin
	digitalWrite(PIN2, HIGH); // Power on the module

	while (true) {
		auto status = modem.init();
		if (status == SIM::Status::Success) {
			break;
		}

		SerialMon.println(SIM::statusToString(status));
		delay(5000);
	}

	SerialMon.println(F("Initialized modem"));
	SIM::RegStatus regStatus;

	auto s = modem.waitForNetworkRegistration(regStatus);

	FmtInvoke("Reg: %s", Serial.println, SIM::statusToString(s));

	// modem.sendATCmd("CNAME");
	auto h = modem.startHttpService();
	FmtInvoke("HTTP svc: %s", Serial.println, SIM::statusToString(h));

	auto cn = http.connect();
	FmtInvoke("Http connect: %s", Serial.println, SIM::statusToString(cn));

	http.disconnect();
	modem.stopHttpService();
	modem.powerOff();
}

void loop()
{
	// modem.loop();
}
