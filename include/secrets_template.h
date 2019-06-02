#ifndef __SECRETS_H__
#define __SECRETS_H__

#include <Arduino.h>

/*
Usage: Copy this file to include/secrets.h, and fill the values set as 'xxxx'.
*/

const char *ssid = "xxxx";
const char *password = "xxxx";
const char *mqttHost = (const char *) "xxxx";
const char *thingname = "xxxx";

static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
xxxx
-----END CERTIFICATE-----
)EOF";

// Copy contents from XXXXXXXX-certificate.pem.crt here ▼
static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
xxxx
-----END CERTIFICATE-----
)KEY";

// Copy contents from  XXXXXXXX-private.pem.key here ▼
static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
xxxx
-----END RSA PRIVATE KEY-----
)KEY";



#endif // __SECRETS_H__


