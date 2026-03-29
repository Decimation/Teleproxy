/*
 Name:		Teleproxy.ino
 Created:	3/28/2026 5:37:44 PM
 Author:	Deci
*/

// ReSharper disable CppClangTidyModernizeMacroToEnum
// ReSharper disable CppInconsistentNaming
#include "SIM7600HttpClient.h"
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
#include "SIM7600TCPClient.h"   // TCP client functionality

namespace SIM = SIM7600;

SIM::Modem     modem(&SerialAT);
SIM::TCPClient tcp(&modem);


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

	SerialMon.println("Initialized modem");
	SIM::RegStatus regStatus;

	auto s = modem.waitForNetworkRegistration(regStatus);
	SerialMon.println(SIM::statusToString(s));
	SerialMon.println(static_cast<uint8_t>(regStatus));

	modem.startTCPIPService();

	// auto apn = modem.configureAPN("wholesale");
	// SerialMon.println(SIM::statusToString(apn));

	// modem.sendATCmd("CNAME");
	auto cn = tcp.connect("www.google.com", 80);
	SerialMon.println(cn);
	SerialMon.println(tcp.available());
	tcp.disconnect();
	modem.stopTCPIPService();
	modem.powerOff();
}

void loop()
{
	// modem.loop();
}
