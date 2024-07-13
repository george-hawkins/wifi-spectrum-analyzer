EasyEDA
=======

Getting started:

* Electronoobs' [EasyEDA Full TUTORIAL + Create Component + TIPS](https://www.youtube.com/watch?v=utBQqcuOt9U). See in particular, his bit on using _Net Port_ elements to avoid to many wires going all over the place in the schematic view.
* The EasyEDA [Tutorial 2020 playlist](https://www.youtube.com/playlist?list=PLbKMtvtYbdPMZfzGuVTdc0MWKrFvU4nsu), in particular part 12 on PCB layout.

Notes:

* Never make a 90&deg; turn when laying down a trace - instead make two 45&deg; turns.
* In the PCB view, snapping only seems be turned on if you right-click on the background and select a snap size. Then to disable snapping, click on the background (so nothing else is selected) and then in the right-hand panel, set _Snap_ to _No_.
* Dragging with the right-mouse button works far better than using the middle-mouse button (which is apparently handled by the browser rather than directly by EasyEDA and only sort-of works).

Unconnected pins
----------------

Use the _No Connect Flag_ (the _X_ icon) to mark any pins that are deliberately left unconnected.

Connecting GND to copper pour
-----------------------------

When auto-routing exclude the GND net from auto-routing and then once everything else is routed connect all GND net flags by creating a copper pour as covered by EasyEDA's [012 PCB Layout](https://youtu.be/l30ziiFeJJE) at 6m 16s.

Click _Design Manager_ (left-hand gutter) and check there are no unconnected nets shown.

### Thermal relief

When pins are connected to a large copper pour they can become impossible to solder as the pour sucks away all the heat.

To avoid this _thermal relief_ pads need to be used - EasyEDA does this by default - compare the pads in the PCB view, in a layer with a copper pour, with those shown in the JLC article on [copper pour basics](https://jlcpcb.com/blog/237-pcb-copper-pour-basics).

Capabilities
------------

Set track width for normal and power lines and via hole and diameter size - Sayanee Basu's [Online PCB manufacturing](https://youtu.be/Xhl9gOPUSWg) at 6m 18s.

Under _Design_ / _Design Rule_, I added an additional row for Power and used these settings (copied from Sayanee and checked against the JLC capabilities):

| Rule | Track width | Clearance | Via diameter | Via drill diameter |
|------|-------------|-----------|--------------|--------------------|
| Default | 0.25 | 0.2 | 0.8 | 0.4 |
| Power | 0.4 | 0.3 | 0.8 | 0.5 |

And changed the rule for all the power nets to the new _Power_ one.

Header
------

In the end, in the schematic view, I just put down blocks of header rather than trying to find component.

Rather than use the generic header in the _Common Library_ (which has some odd limitations, e.g. any header from 1x1 to 1x6 can be selected but after that there are gaps, e.g. there's no 1x7), I went to [LCSC](https://www.lcsc.com/) and hunted around for a brand whose parts are stocked and are cheap (for the unlikely case that I ever wanted the header included with an order).

The brand I chose for simple header was Zhouri, e.g. [1x7 female header](https://www.lcsc.com/product-detail/Female-Headers_ZHOURI-PM2-54-1-7_C5371993.html).

And for DIP sockets, I chose XKB, e.g. [0.6" wide DIP-18 (2x9) socket](https://www.lcsc.com/product-detail/IC-Transistor-Socket_XKB-Connection-X5621FV-2x09-C1524D7430_C2684766.html).

Note: that the pins of the XKB DIP socket are round and slightly smaller than the square pins of the simple Zhouri header. But the holes drilled for them _should_ still be large enough to take parts with standard square pins.

Once you've found what you want, just copy the relevant _LCSC Part #_ and use it in the EasyEDA search, e.g. "C2977588" for a piece of 2x4 header.

Update: I noticed later that even the generic header sizes in the _Common Library_ have LCSC part numbers. Surprisingly, they're not all for the same manufacturer, e.g. the 1x5 male header is a Mintron part, the 1x3 female header is a CKMTW part and the 1x8 female header is a CJT part (that's even marked "not recommended for new designs").

New schematic symbol
--------------------

Say you want a schematic that has the same pin layout as a 2x4 piece of header...

First find a 2x4 header (as described above), place it and note down its _Footprint_ value (right-hand side panel), e.g. "HDR-TH_8P-P2.54-V-F-R2-C4-S2.54".

Select _File_ / _New_ / _Symbol_, press `ctrl-S` and e.g. enter "nRF24" as the _Title_.

Then select _Tools_ / _Symbol Wizard_ and select DIP-8 or SIP.

Note: for the pin order to match up with 2xN header, you have to select DIP-8 rather than DIP-A.

Then, e.g. for a DIP-8, enter:

```
1, GND
2, 3V3
3, GPIO
4, CN
5, CLK
6, MOSI
7, MISO
8, NC
```

Then in the right-hand side panel, click in the _Footprint_ field and under _Search Footprint_ enter the _Footprint_ value noted above, i.e. "HDR-TH_8P-P2.54-V-F-R2-C4-S2.54" and select _LCSC_ in the _Classes_ field. And select the exact match.

Then in the left and right panels, showing the schematic and PCB footprint images, press space until each is rotated such that the wires between the two line up.

To use the new part, go to the schematic sheet of a project and select _Library_ on the left-hand gutter and click _Work Space_ for _Classes_ and find the part there.

For more on creating a schematic symbol and a PCB footprint from scratch, see EasyEDA's [005 Create Schematic Symbol](https://www.youtube.com/watch?v=fOEKxSjfnpk) and [006 Create PCB Footprint](https://www.youtube.com/watch?v=Tb60LwCyzp4). And there's also [007 Bind or Create 3D Model for Footprint](https://www.youtube.com/watch?v=LzmCEQC650o).

Deleting schematic symbols and footprints
-----------------------------------------

Deleting symbols and footprints, that you've created, isn't completely obvious.

Click on _Library_ (left-hand gutter), then click _Symbol_ or _Footprints_ as _Types_, set _Classes_ to _Workspace_ and select the item to be removed. Go to the _More_ button / dropdown and select _Delete_.

Ordering boards
---------------

It doesn't seem to be possible to order less than 5 boards.

Remember to select _HASL lead free_, otherwise leave all settings as they are.

To upload directly to JLC, go to _Fabrication_ / _Gerber_.

To upload to PCBWAY, do the same but then select _Generate Gerber_ - then go to _PCB Instant Quote_, it's a bit confusing - to upload a Gerber immediately (rather than entering dimensions), select the _Quick-order PCB_ link.
