#ifndef __AWS_CONNECTIVITY_H__
#define __AWS_CONNECTIVITY_H__

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>


class AwsConnectivity {

public:

    AwsConnectivity();

    typedef std::function<void(char*, uint8_t*, unsigned int)> callback_signture;

    void setup(callback_signture callback) {
        connectToWifi();
        NTPConnect();
        initWifiSecure();
        connectToAws(callback);
    }

    void loop() {
        client.loop();
    }
    
private:

    WiFiClientSecure net;
    PubSubClient client;

    void connectToAws(callback_signture callback);
    void initWifiSecure();
    void connectToWifi();
    void NTPConnect(void);

};


#endif // __AWS_CONNECTIVITY_H__