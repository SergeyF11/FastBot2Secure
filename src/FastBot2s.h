#pragma once
#include <Arduino.h>
#include <WiFiClientSecure.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include "FastBot2Client.h"
#if defined FASTBOT_SECURED_CLIENT
  #include <TelegramCertificate.h>
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

class FastBot2 : public FastBot2Client {
   public:
    FastBot2() : FastBot2Client(client) {

#if defined FASTBOT_SECURED_CLIENT
	    client.setTrustAnchors(&cert);
#else
        client.setInsecure();
#endif

#if defined(ESP8266)
  #if defined MFLN_SIZE
		const int mfln_size = MFLN_SIZE;
		bool mfln = client.probeMaxFragmentLength( TELEGRAM_HOST, TELEGRAM_PORT, mfln_size);  
		if (mfln) {
		  	client.setBufferSizes(mfln_size, mfln_size);
		} else {
			client.setBufferSizes(512, 512);
		}
  #else
        client.setBufferSizes(512, 512);
  #endif
#endif
    }

    WiFiClientSecure client;
   private:
};
