#include <WiFlyHQ.h>
#include <Adafruit_NeoPixel.h>

#define PIN 7
#define NUM_SPOKES 12

// WiFi credentials
const char wifiSSID[] = "AP0N";
const char wifiPassword[] = "";

// Server & port to connect to
const char server[] = "192.168.1.114";
const int port = 9001;

WiFly wifly;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(103, PIN, NEO_GRB + NEO_KHZ800);

int spokes[NUM_SPOKES][2] = {
  { 0, 4 },
  { 7,  12 },
  { 17, 22 },
  { 25, 30},
  { 36, 42 },
  { 45, 50 },
  { 55, 61 },
  { 64, 68 },
  { 73, 77 },
  { 81, 85 },
  { 90, 94 },
  { 97, 102 }
};

// defaults for color & number of spokes lit
int spokesLit = 0;

int rLoading = 225;
int gLoading = 255;
int bLoading = 255;

int rComplete = 0;
int gComplete = 255;
int bComplete = 255;

// string buffer for reading wifi data
String str = "";

void setup() {
  strip.begin();
  strip.show();

  // Begin serial ports
  Serial.begin(9600);
  wifly.begin(&Serial);

  /* Join wifi network if not already associated */
  //if (!wifly.isAssociated()) {
  /* Setup the WiFly to connect to a wifi network */
  wifly.setSSID(wifiSSID);
  wifly.setPassphrase(wifiPassword);
  wifly.setJoin(WIFLY_WLAN_JOIN_AUTO);
  wifly.enableDHCP();
  wifly.save();
  wifly.join();
  //}

  wifly.setDeviceID("Wifly-TCP");
  wifly.setIpProtocol(WIFLY_PROTOCOL_TCP);

  // Close old connection if active
  if (wifly.isConnected()) {
    wifly.close();
  }

  // Connect initially before we hit the run loop
  wifly.open(server, port);
}

void loop() {

  updateWiFly();

  setLoadingSpokes(spokesLit);
  setCompletedSpokes(spokesLit);

  strip.show();
  delay(5);
}

void setLoadingSpokes(uint16_t number) {

  // loop through spokes from number-end and set them to a color
  // with brightness determined by a sine wave

  // update time & step variables
  float time = millis() / 100.0;
  float step = TWO_PI / (NUM_SPOKES - number);

  // make 'em pulse
  for (int i = NUM_SPOKES - number - 1; i >= 0; i--) {

    float sinVal = sin(time + step * i);
    float sinNormalized = sinVal / 2 + 0.5;

    // this line "compressess" the sine wave so there's a bigger
    // difference between first/brightest spoke and the last/dimmest one
    sinNormalized = sinNormalized * sinNormalized * sinNormalized;

    float rBri = sinNormalized * rLoading;
    float gBri = sinNormalized * gLoading;
    float bBri = sinNormalized * bLoading;

    for (uint16_t j = spokes[i][0]; j <= spokes[i][1]; j++) {
      strip.setPixelColor(j, rBri, gBri, bBri);
    }

  }
}

void setCompletedSpokes(uint16_t number) {
  if (number < NUM_SPOKES) {
    // if the spinner isn't completely loaded then...
    // loop through number of completed spokes and set them to solid
    // we actually start from spoke 12 and go down so that they fill in
    // clockwise, the same direction of the spinner
    for (int i = 0; i < number; i++) {
      int currentSpoke = NUM_SPOKES - 1 - i;
      for (uint16_t j = spokes[currentSpoke][0]; j <= spokes[currentSpoke][1]; j++) {
        strip.setPixelColor(j, rComplete, gComplete, bComplete);
      }
    }
  } else {
    // if it's completely loaded, make all the lights pulse together!
    
    // loop through spokes from 0-end and set them to a color
    // with brightness determined by a sine wave

    // update time
    float time = millis() / 300.0;

    // make 'em pulse
    for (int i = 0; i < NUM_SPOKES; i++) {

      float sinVal = sin(time);
      float sinNormalized = sinVal / 2 + 0.5;

      // this line "compressess" the sine wave so there's a bigger
      // difference between first/brightest spoke and the last/dimmest one
//      sinNormalized = sinNormalized * sinNormalized * sinNormalized;

      float rBri = sinNormalized * rComplete;
      float gBri = sinNormalized * gComplete;
      float bBri = sinNormalized * bComplete;

      for (uint16_t j = spokes[i][0]; j <= spokes[i][1]; j++) {
        strip.setPixelColor(j, rBri, gBri, bBri);
      }

    }
  }
}

void updateWiFly() {

  if (wifly.isConnected() == false) {
    wifly.open(server, port);
  } else {

    int available = wifly.available();

    if (available > 0) {
      char ch = Serial.read();

      if (ch == '\n') {
        // after we get a full line, read in colors by substrings
        spokesLit = str.substring(0, 2).toInt();
        // rLoading = str.substring(3, 6).toInt();
        // gLoading = str.substring(7, 10).toInt();
        // bLoading = str.substring(11, 14).toInt();

        // clear out the String buffer since we've processed this line
        str = "";

      } else {
        // otherwise, if it's not the end of the line, append to the String buffer
        str += ch;
      }

    }
  }
}
