#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define NUM_LEDS 288
#define BRIGHTNESS 255
#define serialRate 2000000

static const uint8_t prefix[] = {'A', 'd', 'a'};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);


void setup() {
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(serialRate);
  Serial.print("Ada\n");
}

void loop() {
  uint32_t red = 0;
  uint32_t green = 0;
  uint32_t blue = 0;

  for (int i = 0; i < sizeof(prefix); ++i) {
    while (!Serial.available());
    if (prefix[i] != Serial.read())
      return;
  }

  while (Serial.available() < 3);

  int highByte = Serial.read();
  int lowByte = Serial.read();
  int checksum = Serial.read();
  if (checksum != (highByte ^ lowByte ^ 0x55))
    return;

  uint16_t ledCount = ((highByte & 0x00FF) << 8 | (lowByte & 0x00FF)) + 1;
  if (ledCount > NUM_LEDS)
    ledCount = NUM_LEDS;

  for (int i = 0; i < ledCount; ++i) {
    while (Serial.available() < 3);
    red = Serial.read();
    green = Serial.read();
    blue = Serial.read();
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
    strip.show();
}
void setColor(int i, uint32_t r, uint32_t g, uint32_t b) {
  uint32_t tM = max(r, max(g, b));
  if(tM == 0)
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  else {
    float multiplier = 255.0 / tM;
    float hR = r*multiplier;
    float hG = g*multiplier;
    float hB = b*multiplier;

    float M = max(hR, max(hG, hB));
    float m = min(hR, min(hG, hB));
    float Luminance = ((M + m) / 2.0 - 127.5) * (255.0/127.5) / multiplier;

    uint32_t Wo = (int)Luminance;
    uint32_t Ro = (int)(r - Luminance);
    uint32_t Bo = (int)(b - Luminance);
    uint32_t Go = (int)(g - Luminance);

    if (Wo < 0) Wo = 0;
    if (Bo < 0) Bo = 0;
    if (Ro < 0) Ro = 0;
    if (Go < 0) Go = 0;
    if (Wo > 255) Wo = 255;
    if (Bo > 255) Bo = 255;
    if (Ro > 255) Ro = 255;
    if (Go > 255) Go = 255;

    strip.setPixelColor(i, strip.Color(Ro, Go, Bo, Wo));
  }
}

