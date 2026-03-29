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

	pinMode(PIN2, OUTPUT);
	digitalWrite(PIN2, HIGH);

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

	FormatInvokeF2("Reg: %s", ln, SIM::statusToString(s));

	auto h = modem.startHttpService();
	FormatInvokeF2("HTTP svc: %s", ln, SIM::statusToString(h));

	auto cn = http.connect();
	FormatInvokeF2("Http connect: %s", ln, SIM::statusToString(cn));

	auto addPara = http.addPara(SIM7600::HttpPara::URL, PSTR("http://www.google.com"));
	FormatInvokeF2("Http add para: %s", ln, SIM::statusToString(addPara));

	http.disconnect();
	modem.stopHttpService();
	modem.powerOff();
}

void loop()
{
	// modem.loop();
}
