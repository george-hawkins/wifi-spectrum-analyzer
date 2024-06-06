Cheap US$10 2.4Ghz band spectrum analyzer
=========================================

First off, the cheapest readily available device, that I found, that can handle the 2.4GHz band and that can credibly be called a spectrum analyzer is the [tinySA Ultra](https://www.tinysa.org/wiki/) at around US$120 (plus US$15 shipping).

The DIY setup described below may not really be a spectrum analyzer but it can help find the clearest part of the 2.4GHz band - a task for which one would normally otherwise need a spectrum analyzer.

The parts, including shipping, come to around US$10.

While I came up with the particular hardware selection below, all the real work was done by [2bndy](https://github.com/2bndy5) who wrote the [`scannerGraphic.ino`](https://github.com/nRF24/RF24/commits/master/examples/scannerGraphic/scannerGraphic.ino) example that's part of the Arduino [RF24 library](https://github.com/nRF24/RF24).

Parts:

* [NRF24L01+ DIP module](https://www.aliexpress.com/item/32330779943.html)
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

Waveshare
---------

The RP2040-Zero modules found on AliExpress are generally copies of the same named module available from Waveshare - I bought mine before I was aware of the Waveshare origin - and from the device's [wiki page](https://www.waveshare.com/wiki/RP2040-Zero), it's clear Waveshare aren't happy about these copies, i.e. even though they've released schematics etc. for it, they don't view it as open-source hardware.

If you'd rather buy from a name-brand like Waveshare rather than AliExpress, here are links to some of the parts on the Waveshare site:

* [RP2040-Zero](https://www.waveshare.com/rp2040-zero.htm)
* [128x64 1.3" blue OLED module](https://www.waveshare.com/1.3inch-oled-b.htm)
