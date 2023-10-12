# Diy mechanical keyboard

This is a dump of all the files that I used to build my custom mech keyboard.

## Layout

To prepare the base layout I used [keyboard-layout-editor](http://www.keyboard-layout-editor.com/). 

I ended-up with [15x5 layout with split space](https://gist.github.com/krzys9876/6718b1eb58f1728975fdd1076bed487b). 
This was my key requirement since what I really wanted was a keyboard with mouse functions built-in. Now, using arrows as mouse movements and split keyboard as left-mid-right mouse buttons I do not need mouse at all. 
Well, maybe this is not the most usefull and ergonomic way to handle mouse but hey, this is all for fun, right?

I tweaked the layout several times since than. The only constraint is the location of the 4 2u keys: space(s) and Enter.

## PCB

I created a PCB and schematic sketch using excellent [klepcbgen tool](https://github.com/jeroen94704/klepcbgen). This was a good starting point to tweak the design to match my ideas. 
I had some experience in designing PCBs and electonic circuits in general (see my [ZX Spectrum Diy project](https://github.com/krzys9876/ZX_Spectrum_diy) so it was quite easy. 

But all in all it was not that great - I totally failed with LEDs since I did not use proper footprint for W2812B-2020 leds. So after I got the boards from JLCPBC it turned out that I cannot use LEDs at all. 
What you see now is a revised version that I did not (yet) put into production.

Other than that the board worked perfectly.

## Firmware

This is a fun project so I would not use any ready-to-use firmware. Anyway I don't think that any of available firmwares would allow me to use mouse. 
So I wrote my own simple version of keyboard handler using Arduino Micro templates and examples. The firmware uses [HID-project](https://github.com/NicoHood/HID).