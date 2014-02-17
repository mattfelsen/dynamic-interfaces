#include <WiFlyHQ.h>

// WiFi credentials
const char wifiSSID[] = "Comfort_Zone";
const char wifiPassword[] = "1441dekalbno8";

// Server & port to connect to
const char server[] = "192.168.0.4";
const int port = 9001;

WiFly wifly;

void setup()
{
  
  // Set up pin 13 LED for output
  pinMode(13, OUTPUT);

  // Begin serial ports
  Serial.begin(9600);
  !wifly.begin(&Serial);

  /* Join wifi network if not already associated */
  if (!wifly.isAssociated()) {
    /* Setup the WiFly to connect to a wifi network */
    wifly.setSSID(wifiSSID);
    wifly.setPassphrase(wifiPassword);
    wifly.setJoin(WIFLY_WLAN_JOIN_AUTO);
    wifly.enableDHCP();
    wifly.save();
    wifly.join();
  }

  wifly.setDeviceID("Wifly-TCP");
  wifly.setIpProtocol(WIFLY_PROTOCOL_TCP);

  // Close old connection if active
  if (wifly.isConnected()) {
    wifly.close();
  }

  // Connect initially before we hit the run loop
  wifly.open(server, port);

}

uint32_t connectTime = 0;
boolean saidHello = false;

void loop()
{
  int available;

  if (wifly.isConnected() == false) {
    if (wifly.open(server, port)) {
      connectTime = millis();
      saidHello = false;
    }
  } else {

    available = wifly.available();
    if (available < 0) {
      //Serial.println("Disconnected");
    } else if (available > 0) {
      int state = wifly.parseInt();
      
      if (state == 1) {
        digitalWrite(13, HIGH);
      } else {
        digitalWrite(13, LOW);
      }
      
      //char receivedData = wifly.read();
      //wifly.print(receivedData);
    } else {
      /* Disconnect after 10 seconds */
      if ((millis() - connectTime) > 5000 && !saidHello) {
        //Serial.println("Disconnecting");
        //wifly.close();
        wifly.println("Hello from WiFly");
        saidHello = true;
      }
    }

  }
}
