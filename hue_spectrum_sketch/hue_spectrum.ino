#include <Adafruit_NeoPixel.h>
#include <cstdint>

namespace {
  constexpr std::uint16_t NUM_PIXELS = 1;
  constexpr std::uint16_t PIXEL_PIN = 16;

  using NeoPixel = Adafruit_NeoPixel;

  NeoPixel pixels(NUM_PIXELS, PIXEL_PIN);

  std::uint16_t hue = 0;
}

void setup() {
  pixels.begin();
  pixels.setBrightness(64);
  pixels.clear();
}

void loop() {
  std::uint32_t color = NeoPixel::gamma32(NeoPixel::ColorHSV(hue++));

  pixels.fill(color);
  pixels.show();
}