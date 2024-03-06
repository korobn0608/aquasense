#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char DEVICE_LOGIN_NAME[]  = "ea6f0caa-d962-4014-bc64-8f05d57e9a2a";

const char SSID[]               = SECRET_SSID;    // Network SSID (name)
const char PASS[]               = SECRET_OPTIONAL_PASS;    // Network password (use for WPA, or use as key for WEP)
const char DEVICE_KEY[]  = SECRET_DEVICE_KEY;    // Secret device password


float ph;
float td;
float temp;
float tur;

void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(ph, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(td, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(temp, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(tur, READ, 1 * SECONDS, NULL);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
