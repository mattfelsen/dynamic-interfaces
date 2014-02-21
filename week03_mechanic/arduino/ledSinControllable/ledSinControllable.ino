#include <WiFlyHQ.h>
#include <Adafruit_NeoPixel.h>
#define PIN 6

// WiFi credentials
const char wifiSSID[] = "AP0N";
const char wifiPassword[] = "";

// Server & port to connect to
const char server[] = "192.168.1.124";
const int port = 9001;

WiFly wifly;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(180, PIN, NEO_GRB + NEO_KHZ800);

int r = 0;
int g = 25;
int b = 25;

String str = "";
char rChar[3];
char gChar[3];
char bChar[3];

void setup()
{
  // Set up the strip
  strip.begin();
  strip.show();

  // Set up pin 13 LED for output
  pinMode(13, OUTPUT);

  // Begin serial ports
  Serial.begin(9600);
  wifly.begin(&Serial);

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
      char ch = Serial.read();
      if (ch == '\n') {
        String rStr = str.substring(0, 2);
        String gStr = str.substring(3, 5);
        String bStr = str.substring(6, 8);

        rStr.toCharArray(rChar, 3);
        gStr.toCharArray(gChar, 3);
        bStr.toCharArray(bChar, 3);

        r = atoi(rChar);
        g = atoi(gChar);
        b = atoi(bChar);

        str = "";
      } else {
        str += ch;
      }

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

  setLEDs();
}

void setLEDs() {
  float time = millis() / 200.0;
  float step = TWO_PI * 16 / strip.numPixels();

  for (uint16_t i = 0; i < strip.numPixels(); i++) {

    float sinVal = sin(time + step * i);
    float sinNormalized = sinVal / 2 + 0.5;

    //if (!i) Serial.println(sinVal);

    float rBri = sinNormalized * r;
    float gBri = sinNormalized * g;
    float bBri = sinNormalized * b;

    strip.setPixelColor(i, rBri, gBri, bBri);

  }
  strip.show();
  delay(50);
}
