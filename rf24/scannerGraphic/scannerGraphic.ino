/*
 * Copyright (C) 2022 Brendan Doherty <2bndy5@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

/*
 * This example uses 1 of 2 different popular displays. To control which display to use,
 * comment/uncomment the lines below that define
 * - `SPI_DISPLAY`: This requires the "Adafruit ST7735 and ST7789 Library" installed
 * - `I2C_DISPLAY`: This requires the "Adafruit SSD1306" library installed
 * 
 * Use the Arduino Library manager to ensure the required libraries are installed.
 * By default, this sketch uses the SPI_DISPLAY (ST7789). Using both displays at the same
 * time is not supported by this sketch.
 * 
 * NOTES:
 *     The `SCREEN_HEIGHT` and `SCREEN_WIDTH` defines may need to be adjusted according
 *     to your display module's capability. This example expects the display to be at
 *     least 128 pixels wide. Otherwise, you would have to reduce the `numChannels`
 *     constant to fit within your display's width.
 *
 *     The SPI_DISPLAY uses its own pins defined by `TFT_CS`, `TFT_DC`, and the
 *     optional `TFT_RST` (see below). The SPI bus is shared between radio and display,
 *     so the display's CS pin must be connected as specified by `TFT_CS`.
 *     If your ST7789 display does not have a CS pin, then further modification must
 *     be made so it does not use the same SPI bus that the radio uses.
 * 
 *     `DEBUGGING` can be enabled (uncommented) to show Serial output. This is just a
 *     convenience to set radio data rate or further development. See our other
 *     RF24/scanner example that only uses the Serial Monitor instead of a graphic
 *     display.
 *
 * See documentation at https://nRF24.github.io/RF24
 */
#include <Adafruit_GFX.h>  // dependency of Adafruit display libraries
#include "RF24.h"
#include <bitset>

/********************************************************************
 * CHOOSE A DISPLAY INTERFACE
 * uncomment/comment only 1 of the following to use the desired display
 ********************************************************************/
// #define I2C_DISPLAY  // using the SSD1306
#define SPI_DISPLAY  // using ST7789

/********************************************************************
 * Choose a sketch feature
 * uncomment any of the following to enable a special feature
 ********************************************************************/
// #define DEBUGGING  // uncomment to enable Serial output (optional)
// #define HOLD_PEAKS  // uncomment to disable decay of maxPeak pixels (useful for assessing total noise)

/********************************************************************
 * Instantiate the radio and app-specific attributes
 ********************************************************************/

#define CE_PIN 5
#define CSN_PIN 1
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

// To detect noise, we'll use the worst addresses possible (a reverse engineering tactic).
// These addresses are designed to confuse the radio into thinking
// that the RF signal's preamble is part of the packet/payload.
const uint8_t noiseAddress[][2] = { { 0x55, 0x55 }, { 0xAA, 0xAA }, { 0xA0, 0xAA }, { 0xAB, 0xAA }, { 0xAC, 0xAA }, { 0xAD, 0xAA } };

const uint8_t numChannels = 126;  // 0-125 are supported

/***********************************************************************
 * Declare caching mechanism to track history of signals for peak decay
 **********************************************************************/

constexpr std::size_t MAX_SAMPLES{100};
constexpr std::size_t DECAY_MULTIPLIER{16}; // Allow for n-step decays.
constexpr std::size_t CLAMP_MAX{0x0F};
constexpr std::uint8_t SCALAR_MARKS{4};

/// A data structure to organize the cache of signals for a certain channel.
struct ChannelHistory {
  /// Push a signal's value into cached history while popping
  /// oldest cached value. This also sets the maxPeak value.
  /// @returns The sum of signals found in the cached history
  std::size_t push(std::size_t pos, bool value) {
    samples[pos] = value;

    std::size_t sum = samples.count();
    maxPeak = max(sum * DECAY_MULTIPLIER, maxPeak);
    return sum;
  }

  void decMaxPeak() {
    if (maxPeak > 0) {
      --maxPeak;
    }
  }

  std::int16_t getMaxPeakHeight(std::uint16_t chartHeight) const {
    constexpr std::size_t MULTIPLIER = CLAMP_MAX * DECAY_MULTIPLIER;
    std::size_t clampedMaxPeak = std::min(MULTIPLIER, maxPeak);
    std::int32_t maxPeakHeight = chartHeight * clampedMaxPeak;

    maxPeakHeight /= MULTIPLIER;

    return static_cast<std::int16_t>(maxPeakHeight);
  }

private:
  std::size_t maxPeak = 0;
  std::bitset<MAX_SAMPLES> samples;
};

/// An array of caches to use as channels' history
ChannelHistory stored[numChannels];
std::size_t samplePos = 0;

/********************************************************************
 * Instantiate the appropriate display objects according to the
 * defines (above near top of file)
 ********************************************************************/

#ifdef I2C_DISPLAY

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1        // Or set to -1 and connect to Arduino RESET pin
#define SCREEN_ADDRESS 0x3D  // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BLACK SSD1306_BLACK
#define WHITE SSD1306_WHITE
#define REFRESH ({ display.display(); })
#define CLEAR_DISPLAY ({ display.clearDisplay(); })

#elif defined(SPI_DISPLAY)

#include <Adafruit_SH110X.h>

#define OLED_CS 6
#define OLED_DC 7
#define OLED_RST 8

#define SCREEN_WIDTH 128   // TFT display width, in pixels
#define SCREEN_HEIGHT 64  // TFT display height, in pixels

Adafruit_SH1106G display{SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS};

#define BLACK SH110X_BLACK
#define WHITE SH110X_WHITE
#define REFRESH ({ display.display(); })
#define CLEAR_DISPLAY ({ display.clearDisplay(); })

#endif  // if defined(I2C_DISPLAY) || defined(SPI_DISPLAY)

// constant chart size attributes
const uint16_t margin = 1;  // use 1 pixel margin for markers on each side of chart
const uint16_t barWidth = (SCREEN_WIDTH - (margin * 2)) / numChannels;
const uint16_t chartHeight = SCREEN_HEIGHT - 10;
const uint16_t chartWidth = margin * 2 + (numChannels * barWidth);

/********************************************************************
 * Configure debugging on Serial output
 ********************************************************************/

#ifdef DEBUGGING
#include "printf.h"
#define SERIAL_DEBUG(x) ({ x; })
#else
#define SERIAL_DEBUG(x)
#endif

/********************************************************************
 * Setup the app
 ********************************************************************/
void setup(void) {
#ifdef DEBUGGING
  // Print preamble
  Serial.begin(115200);
  while (!Serial) {
    // some boards need this to wait for Serial connection
  }
  Serial.println(F("RF24/examples/scannerGraphic"));
#endif

#ifdef I2C_DISPLAY
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    SERIAL_DEBUG(Serial.println(F("SSD1306 allocation failed")););
    while (true) {
      // Don't proceed, loop forever
    }
  }
#elif defined(SPI_DISPLAY)
  display.begin(0, true);
#endif

  display.setRotation(2); // Rotate 180 degrees.

  // Clear the buffer
  CLEAR_DISPLAY;

  // Setup and configure rf radio
  if (!radio.begin()) {
    SERIAL_DEBUG(Serial.println(F("radio hardware not responding!")););
    display.setCursor(1, 1);
    display.setTextColor(WHITE);
    display.print(F("radio hardware\nnot responding!"));
    REFRESH;
    while (true) {
      // hold in an infinite loop
    }
  }
  displayChartAxis();

  radio.setAutoAck(false);   // Don't acknowledge arbitrary signals
  radio.disableCRC();        // accept any signal we find
  radio.setAddressWidth(2);  // a reverse engineering tactic (not typically recommended)
  for (uint8_t i = 0; i < 6; ++i) {
    radio.openReadingPipe(i, noiseAddress[i]);
  }

  // set the data rate
#ifdef DEBUGGING
  unsigned long inputTimeout = millis() + 7000;
  Serial.print(F("Select your Data Rate. "));
  Serial.println(F("Enter '1' for 1Mbps, '2' for 2Mbps, '3' for 250kbps. Defaults to 1 Mbps."));
  while (!Serial.available() && millis() < inputTimeout) {
    // Wait for user input. Timeout after 7 seconds.
  }
  char dataRate = !Serial.available() ? '1' : Serial.parseInt();
#else
  char dataRate = '1';
#endif
  if (dataRate == '2') {
    SERIAL_DEBUG(Serial.println(F("Using 2 Mbps.")););
    radio.setDataRate(RF24_2MBPS);
  } else if (dataRate == '3') {
    SERIAL_DEBUG(Serial.println(F("Using 250 kbps.")););
    radio.setDataRate(RF24_250KBPS);
  } else {  // dataRate == '1' or invalid values
    SERIAL_DEBUG(Serial.println(F("Using 1 Mbps.")););
    radio.setDataRate(RF24_1MBPS);
  }

  // Get into standby mode
  radio.startListening();
  radio.stopListening();
  radio.flush_rx();
}

/********************************************************************
 * Make the app loop forever
 ********************************************************************/
void loop(void) {
  for (uint8_t channel = 0; channel < numChannels; ++channel) {
    bool foundSignal = scanChannel(channel);
    std::size_t cacheSum = stored[channel].push(samplePos, foundSignal);

    std::int16_t x = (barWidth * channel) + 1 + margin - (barWidth * (bool)channel);
    // reset bar for current channel to 0
    display.fillRect(x, 0, barWidth, chartHeight, BLACK);

    std::size_t clampedSum = std::min(CLAMP_MAX, cacheSum);
    std::int16_t sumHeight = chartHeight * clampedSum / CLAMP_MAX;

    std::int16_t maxPeakHeight = stored[channel].getMaxPeakHeight(chartHeight);

    if (maxPeakHeight > sumHeight) {
      // draw a peak line only if it is greater than current sum of cached signal counts
      std::int16_t y = chartHeight - maxPeakHeight;
      display.drawLine(x, y, x + barWidth, y, WHITE);
#ifndef HOLD_PEAKS
      stored[channel].decMaxPeak();  // decrement max peak
#endif
    }
    if (sumHeight > 0) {  // draw the cached signal count
      display.fillRect(x, chartHeight - sumHeight, barWidth, sumHeight, WHITE);
    }
  }

  REFRESH;

  samplePos = (samplePos + 1) % MAX_SAMPLES;
}  // end loop()

/// Scan a specified channel and return the resulting flag
bool scanChannel(uint8_t channel) {
  radio.setChannel(channel);

  // Listen for a little
  radio.startListening();
  delayMicroseconds(130);
  bool foundSignal = radio.testRPD();
  radio.stopListening();

  // Did we get a signal?
  if (foundSignal || radio.testRPD() || radio.available()) {
    radio.flush_rx();  // discard packets of noise
    return true;
  }
  return false;
}

/// Draw the chart axis and labels
void displayChartAxis() {
  // draw base line
  display.drawLine(0, chartHeight + 1, chartWidth - margin, chartHeight + 1, WHITE);

  // draw base line border
  display.drawLine(margin, SCREEN_HEIGHT, margin, chartHeight - 2, WHITE);
  display.drawLine(chartWidth - margin, SCREEN_HEIGHT, chartWidth - margin, chartHeight - 2, WHITE);

  // draw scalar marks
  for (uint8_t i = 0; i < SCALAR_MARKS; ++i) {
    uint8_t scalarHeight = chartHeight * i / SCALAR_MARKS;
    display.drawLine(0, scalarHeight, chartWidth, scalarHeight, WHITE);
  }

  // draw channel range labels
  display.setTextSize(1);
  display.setTextColor(WHITE);
  uint8_t maxChannelDigits = 0;
  uint8_t tmp = numChannels;
  while (tmp) {
    maxChannelDigits += 1;
    tmp /= 10;
  }
  display.setCursor(chartWidth - (7 * maxChannelDigits), chartHeight + 3);
  display.print(numChannels - 1);
  display.setCursor(margin + 2, chartHeight + 3);
  display.print(0);

  // refresh display
  REFRESH;
}
