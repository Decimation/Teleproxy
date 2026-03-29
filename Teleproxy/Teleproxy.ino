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

#define SIM7600_MODEM_TX_BUFFER_SIZE_B 512
#define SIM7600_MODEM_RX_BUFFER_SIZE_B 512
#define SIM7600_LOG_VERBOSE_ERROR_CODES
#define SIM7600_LOG_STREAM SerialMon
#define SIM7600_LOG_LEVEL 5  // Enable all logs including AT commands

#include <SIM7600Modem.h>
#include "SIM7600HttpClient.h"


namespace SIM = SIM7600;

SIM::Modem      modem(&SerialAT);
SIM::HttpClient http(&modem);


void setup()
{
	using std::size_t;

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

	FormatInvoke("Reg: %s", SIM::statusToString(s));
		
	auto h = modem.startHttpService();
	FormatInvoke("HTTP svc: %s", SIM::statusToString(h));

	auto cn = http.connect();
	FormatInvoke("Http connect: %s", SIM::statusToString(cn));

	auto addPara = http.addPara(SIM7600::HttpParameter::URL, PSTR("http://www.google.com"));
	FormatInvoke("Http add para: %s", SIM::statusToString(addPara));

	uint8_t buf1[4096];
	size_t  outSize;

	auto rhRes = http.readHead(buf1, &outSize);
	FormatInvoke("Http head: %s", SIM::statusToString(rhRes));

	http.disconnect();
	modem.stopHttpService();
	modem.powerOff();
}

void loop()
{
	modem.loop();
}
