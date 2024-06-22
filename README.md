Cheap US$10 2.4Ghz band spectrum analyzer
=========================================

First off, the cheapest readily available device, that I found, that can handle the 2.4GHz band and that can credibly be called a spectrum analyzer is the [tinySA Ultra](https://www.tinysa.org/wiki/) at around US$120 (plus US$15 shipping).

The DIY setup described below may not really be a spectrum analyzer but it can help find the clearest part of the 2.4GHz band - a task for which one would normally otherwise need a spectrum analyzer.

The parts, including shipping, come to around US$10.

While I came up with the particular hardware selection below, all the real work was done by [2bndy](https://github.com/2bndy5) who wrote the [`scannerGraphic.ino`](https://github.com/nRF24/RF24/commits/master/examples/scannerGraphic/scannerGraphic.ino) example that's part of the Arduino [RF24 library](https://github.com/nRF24/RF24).

Parts:

* [nRF24L01+ DIP module](https://www.aliexpress.com/item/32330779943.html)
* [RP2040-Zero](https://www.aliexpress.com/item/1005004967926448.html) (buy the one with the header already soldered to save yourself time later).
* [128x64 1.3" white OLED module](https://www.aliexpress.com/item/1005003484018034.html)
* [3.3V voltage regulator](https://www.aliexpress.com/item/32464952488.html)
* [4x6cm protoboard](https://www.aliexpress.com/item/32864559799.html)

If you don't already have additional header, the same store, has [1x40 pin header](https://www.aliexpress.com/item/32402348546.html).

I sometimes want to use this setup on-the-go. If you already have a powerbank, this is the obvious easy solution for powering things in this situation. An alternative is to plug together a USB OTG setup and power it from your smartphone. For this, you'll need:

* A regular [USB-A to USB-C cable](https://www.aliexpress.com/item/1005005274790443.html)
* A [USB-A female to USB-C male OTG adapter](https://www.aliexpress.com/item/32830129548.html) (this page lists different variants, the cheap "USB2.0 TypeC to USB" one is fine).

It feels odd that such a cable combination costs almost as much as the total for all the other parts.

TODO:

* Is the SMD module more convenient than the DIP one.
* Was 4x6cm protoboard the best size?

Notes:

* Many AliExpress product pages use the odd term "welding" to mean a board that comes with header already soldered on.
* Many tutorials recommend only using NRF24L01+ modules with a FEM (Front End Module) or PA (Power Amplifier) - these allow the module to transmit further but as this setup only involves listening, i.e. receiving, a FEM/PA isn't necessary.
* One could power the NRF24L01+ module directly from the 3.3V pin of the RP2040-Zero but the NRF24L01+ module is very sensitive to noise and the small volatage regulator board includes a capacitor arrangement that others have found works well at making things less noisy. If you prefer, you could just solder in a suitable capacitor, on its own, instead.

The display used here comes in two variants - an I2C one and an SPI one. The I2C variant is easier to wire up (requiring four wires rather than the seven needed for the SPI one). I2C is slower than SPI but for this setup the speed difference shouldn't really matter. However, I went with the SPI one as I just came across far more issues around OLEDs using I2C than SPI when looking into which to use.

As far as I understand it one wants you want to be able to look at the unmodulated RF noise floor across the 2.4Ghz band. All kinds of devices, including micro-wave ovens and Wi-Fi routers, are pumping out RF energy at various frequencies in this band and one wants to find the area in the band with the least such energy, i.e. the clearest part of the band.

Note: I don't really know very much about RF - I'd appreciate being corrected if anything I just said isn't quite right.

I'd be curious to know if others think they can do better than this solution while still staying well below e.g. US$50 for parts.

An obvious first upgrade would be to use a larger 320x240 screen like the 2" and 2.8" models available [here](https://www.aliexpress.com/item/1005003797803015.html) (other product pages on AliExpress, with the same 2" model, describe it as an OLED but I don't see anything on the silkscreen to confirm this).

The sketch has nice features, e.g. the max values seen across the spectrum are decayed over time so, the impact of intermittent events isn't immediately lost.

Breadboard
----------

![breadboard](breadboard/breadboard_bb.png)

As well as the breadboard layout of the various parts, the diagram above also includes larger views of the pinouts for the RP2040-Zero, OLED module and nRF module in order to make the SPI pins involved clearer.

**Warning:** different SPI OLED modules will have the same set of seven pins but there's no consistency in the order of these pins - I've shown the pinouts for both the [Waveshare 1.3" OLED (variant B)](https://www.waveshare.com/1.3inch-OLED-B.htm) and for the OLED module from AliExpress (that's linked to above).

I've soldered the header for pins 9 to 13 of the RP2040-Zero pointing upwards in order to make it possible to plug it into a breadboard. However, if you've only got one RP2040-Zero and plan to solder it to a protoboard then obviously all header should be soldered pointing downward.

RP2040-Zero blink LED
---------------------

The RP2040-Zero is extremely similar to the Raspberry Pi Pico so, there's no real separate documentation for the Zero (beyond what's in the RP2040-Zero [wiki page](https://www.waveshare.com/wiki/RP2040-Zero)) - all the documentation that applies for the Pico applies to the Zero.

Except that the Zero has fewer pins and it has an WS2812 RGB LED (often referred to as a neopixel) rather than the simple LED on the Pico.

A little surprisingly, the Arduino core for the Pico is not maintained by the Raspberry Pi people but by [Earle F. Philhower, III](https://github.com/earlephilhower) in his [arduino-pico repo](https://github.com/earlephilhower/arduino-pico/).

Getting setup is described well in this [tutorial](https://randomnerdtutorials.com/programming-raspberry-pi-pico-w-arduino-ide/) from Random Nerd Tutorials. At the step where a board needs to be chosen, I chose the "Waveshare RP2040 Zero".

Then I went to the IDE's _Sketch_ / _Include Library_ / _Manager Libraries_, searched for "neopixel" and installed the one called "Adafruit NeoPixel". I then went to _File_ / _Examples_, scrolled down right to the end of the list, where there's a separate section for custom libraries, selected _Adafruit Neopixel_ / _simple_ and modified that example for the RP2040-Zero to get:

```
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
```

Note: most example include a `delay` call at the end of `loop` but the updating of the neopixel is slow enough that it acts as a throttle on the whole process - even without a delay, it takes about 20 seconds to cycle through the spectrum (at which point `hue` overflows and returns to 0).

To get the board into a state where you can upload a sketch, you have to unplug USB and press the BOOT button on the board, keep it held down while reconnecting USB and then release.

Note: some tutorials seem to suggest that you only need to plug things in and out while holding down the BOOT button as part of uploading your _first_ Arduino sketch but I found it always necessary before uploading a sketch.

By default, the Arduino IDE expects to upload sketches via a serial port so, you have to _Tools_ / _Port_ and select _UF2 Board_.

Then you can upload the sketch to the board (using the normal _Upload_ button). Once the sketch is uploaded, it's automatically reset such that it's no longer in bootloader mode and starts running your program.

Once restarted it appears as a serial device (see below for `udev` rules if using Linux) and you can connect to it using the IDE's serial monitor and use standard Arduino serial calls like:

```
void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("Hello world");
}
```

The switching back and forward between UF2 bootloader mode and serial port mode, as you upload sketches and then want to look at their serial output, is a bit inconvenient. The IDE itself is a little confused by this and pops up warnings every so often that e.g. the serial port can't be found.

The first time I used the IDE's _Serial Monitor_, I had to actively go to _Tools_ / _Port_ and manually select the relevant serial port but after that it seemed to correctly flip back and forward between _UF2 Board_ and the appropriate serial device as needed.

Waveshare OLED
--------------

In the Arduino, IDE open the `waveshare/OLED_1in3/OLED_1in3.ino` included in this repo.

Make sure "Waveshare RP2040 Zero" is selected as the board.

Hold down the _BOOT_ button, connect to USB, release _BOOT_ button.

Make sure _UF2 Board_ is selected under _Tools_ / _Port_.

Click the _Upload_ button. That's it - the OLED should cycle through various graphics.

### Notes

The `OLED_1in3.ino` and related files were extracted from the `OLED_Module_Code.7z` that was downloaded (in June 2024) using the "Sample Program (New)" link in the [_Resources_ section](https://www.waveshare.com/wiki/1.3inch_OLED_(B)#Resources) of the Waveshare wiki page for the 1.3" OLED (B) module.

However, Waveshare don't seem to have kept this code up-to-date with the SPI libary included in the latest Arduino IDE (and also seem to have relied on a more forgiving version of `gcc`). So, I had to make some minor changes - these can be seen in commit [`688c550`](https://github.com/george-hawkins/wifi-spectrum-analyzer/commit/688c550).

In particular, I switched to the newer approach to setting clock frequency, bit order and SPI mode using `SPISettings`. I set the SPI clock frequency to 8MHz - this is equivalent to the speed that would have been set when using the old `SPI_CLOCK_DIV2` value with a classic 16MHz Arduino UNO.

From reading the datasheet for the SH1106 chip, it should be possible to use far higher speeds (the Pico supports up to 62.5MHz) but 8MHz seems to be a safe upper limit for breadboards (beyond 8MHz you may experience issues due to jumper-wire lengths or characteristics of the breadboard).

SPI pin names
-------------

There's some inconsistency in labelling SPI pins on various breakouts, particularly ones where some of the pins can be used for both SPI and I2C.

E.g. on the boards used in this project, you may see these alternative names for the various SPI pins:

* MISO / SPI RX / SCL
* MOSI / SPI TX / SDA
* CLK / SCLK / SCK
* CSn / CS / SS

For SPI, I'd use the names MISO, MOSI, CLK and CSn. And I'd view the names SCL, SDA and SCK as names for similar functions in I2C which ideally shouldn't be used when describing an SPI interface.

Then there's CS, SS and CSn (and even nCS and CSN) which are all valid names for the same SPI function and their use just seems to depend on manufacturer preferences.

Finally, there's MISO vs SPI RX and MOSI vs SPI TX - the "MI" is MISO stands for "master input" and the "MO" in MOSI stands for "master output" so, you can see how they can be equated with RX and and TX. However, while understood, using RX and TX seems vanishingly rare in the hobbyist domain and I've no idea why Wavecom uses them in their pinout diagrams for the RP2040-Zero.

nRF24L01+ breadboard adapter
----------------------------

I think you're better off plugging jumper wires straight into the pins on the nRF24L01+ module but if you really want to mount it on a breadboard, you'll need an adapter.

You can just get a plain DIP-8 breadboard adapter like this [one](https://www.chipquik.com/store/product_info.php?products_id=2700080) from Chip Quik that Digikey sell for around US$5.80.

A cheaper alternative is to use an adapter meant for the common ESP8266 modules that also use a 2x4 pin layout. The part up above all come from one store on AliExpress and that store also has an adapter for ESP8266 modules [here](https://www.aliexpress.com/item/32840204126.html) for US$0.27.

The adapter comes with a little capacitor for reducing noise on the power pins but as those pins aren't in the same location on the ESP8266 module and the nRF24L01+ module, you'd need to desolder it and short the pads it was connected to with a small blob of solder.

If you don't like desoldering SMD components, other stores also carry a version of the board with a thru-hole capacitor, e.g. [here](https://www.aliexpress.com/item/1005002597510818.html).

TODO: of course, you can avoid this issue by buying the nRF24L01+ SMD module where the pins are in a single row (like on the OLED module).

Udev rules
----------

According to the introduction [section](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html#your-first-binaries) of the Pico documentation (which applies to the RP2040-Zero etc.), you have to hold down the BOOTSEL (labelled BOOT on the RP2040-Zero) while plugging in your USB cable.

Doing so makes no difference to the output seen in `/var/log/syslog`, i.e. you see the same output whether the button is held down or not:

```
Jun 16 11:45:24 joebloggs-machine kernel: [159291.908184] usb 1-7.4: new full-speed USB device number 96 using xhci_hcd
Jun 16 11:45:24 joebloggs-machine kernel: [159292.009267] usb 1-7.4: New USB device found, idVendor=2e8a, idProduct=0003, bcdDevice= 1.00
Jun 16 11:45:24 joebloggs-machine kernel: [159292.009271] usb 1-7.4: New USB device strings: Mfr=1, Product=2, SerialNumber=3
Jun 16 11:45:24 joebloggs-machine kernel: [159292.009273] usb 1-7.4: Product: RP2 Boot
Jun 16 11:45:24 joebloggs-machine kernel: [159292.009274] usb 1-7.4: Manufacturer: Raspberry Pi
Jun 16 11:45:24 joebloggs-machine kernel: [159292.009275] usb 1-7.4: SerialNumber: E0C9125B0D9B
Jun 16 11:45:24 joebloggs-machine kernel: [159292.019963] usb-storage 1-7.4:1.0: USB Mass Storage device detected
Jun 16 11:45:24 joebloggs-machine kernel: [159292.020414] scsi host4: usb-storage 1-7.4:1.0
Jun 16 11:45:24 joebloggs-machine mtp-probe: checking bus 1, device 96: "/sys/devices/pci0000:00/0000:00:14.0/usb1/1-7/1-7.4"
Jun 16 11:45:24 joebloggs-machine mtp-probe: bus: 1, device: 96 was not an MTP device
Jun 16 11:45:25 joebloggs-machine kernel: [159293.025105] scsi 4:0:0:0: Direct-Access     RPI      RP2              3    PQ: 0 ANSI: 2
Jun 16 11:45:25 joebloggs-machine kernel: [159293.025837] sd 4:0:0:0: Attached scsi generic sg1 type 0
Jun 16 11:45:25 joebloggs-machine kernel: [159293.026257] sd 4:0:0:0: [sdb] 262144 512-byte logical blocks: (134 MB/128 MiB)
Jun 16 11:45:25 joebloggs-machine kernel: [159293.026981] sd 4:0:0:0: [sdb] Write Protect is off
Jun 16 11:45:25 joebloggs-machine kernel: [159293.026987] sd 4:0:0:0: [sdb] Mode Sense: 03 00 00 00
Jun 16 11:45:25 joebloggs-machine kernel: [159293.028437] sd 4:0:0:0: [sdb] No Caching mode page found
Jun 16 11:45:25 joebloggs-machine kernel: [159293.028445] sd 4:0:0:0: [sdb] Assuming drive cache: write through
Jun 16 11:45:25 joebloggs-machine kernel: [159293.037361]  sdb: sdb1
Jun 16 11:45:25 joebloggs-machine kernel: [159293.037612] sd 4:0:0:0: [sdb] Attached SCSI removable disk
Jun 16 11:45:25 joebloggs-machine systemd-udevd[525667]: sdb: Process '/usr/bin/unshare -m /usr/bin/snap auto-import --mount=/dev/sdb' failed with exit code 1.
Jun 16 11:45:27 joebloggs-machine systemd-udevd[525667]: sdb1: Process '/usr/bin/unshare -m /usr/bin/snap auto-import --mount=/dev/sdb1' failed with exit code 1.
Jun 16 11:45:27 joebloggs-machine udisksd[980]: Mounted /dev/sdb1 at /media/joebloggs/RPI-RP2 on behalf of uid 1000
```

The two lines about `auto-import` failing with exit code 1 seems to be a long known but harmless `snapd` issue that's covered by issue [1968883](https://bugs.launchpad.net/ubuntu/+source/snapd/+bug/1968883).

It seems as if a serial device is only created if the program running on the RP2040 actively requests this functionality (and as far as I can see, the internal Arduino setup logic always does this). To make this port easily accessible (and to tell `mtp-probe` to ignore it):

```
$ sudo vim /etc/udev/rules.d/50-serial-ports.rules
```

And add the lines:

```
# RP2040
SUBSYSTEM=="tty", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="0003", \
    SYMLINK+="rp2-boot", MODE="0660", TAG+="uaccess"
ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="0003", ENV{MTP_NO_PROBE}="1"
```

When a serial device is created it will be accessible as `/dev/rp2-boot`.

The RP2040 is automatically mounted as a removeable drive called `RPI-RP2` and contains a HTML file that redirects to the [Pico documentation](https://raspberrypi.com/device/RP2?version=E0C9125B0D9B) and an `INFO_UF2.TXT` that provides details of the UF2 bootloader:

```
$ cat /media/$USER/RPI-RP2/INFO_UF2.TXT
UF2 Bootloader v3.0
Model: Raspberry Pi RP2
Board-ID: RPI-RP2
```

As this bootloader is in ROM, there's no way to update it.

Waveshare
---------

The RP2040-Zero modules found on AliExpress are generally copies of the same named module available from Waveshare - I bought mine before I was aware of the Waveshare origin - and from the device's [wiki page](https://www.waveshare.com/wiki/RP2040-Zero), it's clear Waveshare aren't happy about these copies, i.e. even though they've released schematics etc. for it, they don't view it as open-source hardware.

If you'd rather buy from a name-brand like Waveshare rather than AliExpress, here are links to some of the parts on the Waveshare site:

* [RP2040-Zero](https://www.waveshare.com/rp2040-zero.htm)
* [128x64 1.3" blue OLED module](https://www.waveshare.com/1.3inch-oled-b.htm)

Credits
-------

The breadboard diagram above was created using [Fritzing](https://fritzing.org/).

The Fritzing Waveshare 1.3" OLED part was created by A. Horneffer and was found [here](https://forum.fritzing.org/t/waveshare-1-3-inch-oled-with-sh1106/4848/6).

The Fritzing Waveshare RP2040-Zero part was created by Peter Van Epp and was found [here](https://forum.fritzing.org/t/part-request-waveshare-rp2040-zero/16705/2).

And as already noted above, the main `scannerGraphic.ino` sketch was created by [2bndy](https://github.com/2bndy5).

The pinout diagrams, seen in the main breadboard diagram above, were created using the Fritzing breadboard SVGs for the relevand parts and the template that's linked to and described in this [video](https://youtu.be/ndVs1UvK6AE) by ATMakers on how to create pinout diagrams.
