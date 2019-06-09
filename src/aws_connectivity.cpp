// #include "aws_connectivity.h"

// #include "secrets.h"

// AwsConnectivity::AwsConnectivity() :
//     client(net)
// {

// }

// void AwsConnectivity::initWifiSecure() {
//     net.setCACert(cacert);
//     net.setCertificate(client_cert);
//     net.setPrivateKey(privkey);
//     // net.setX509Time(time(nullptr));
// }

// void AwsConnectivity::connectToWifi() {
//     WiFi.mode(WIFI_STA);
//     WiFi.begin(ssid, password);
//     Serial.printf("Attempting to connect to SSID: ");
//     Serial.printf(ssid);
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         Serial.print(".");
//         delay(1000);
//     }
//     Serial.println("ok!");
// }


// void AwsConnectivity::connectToAws(callback_signture callback) {
//     client.setServer(mqttHost, 8883);
//     client.setCallback(callback);
//     if(client.connect(thingname)) {
//         Serial.println("connected to aws");
//         client.subscribe("animations/#");
//     }
//     else {
//         Serial.print("error state:");
//         Serial.println(client.state());
//     }
// }

// void AwsConnectivity::NTPConnect(void)
// {
//     static const time_t nowish = 1510592825;
//     time_t now;

//     Serial.print("Setting time using SNTP");
//     configTime(2 * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
//     now = time(nullptr);
//     while (now < nowish)
//     {
//         delay(500);
//         Serial.print(".");
//         now = time(nullptr);
//     }
//     Serial.println("done!");
//     struct tm timeinfo;
//     gmtime_r(&now, &timeinfo);
//     Serial.print("Current time: ");
//     Serial.print(asctime(&timeinfo));
// }
