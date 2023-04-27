Welcome to Benisphere Suite
===

## An active fork expanding upon Hemisphere Suite.

To download and install the latest release, head to [releases](https://github.com/benirose/O_C-BenisphereSuite/releases).

To learn more about what makes this fork different than the original Hemisphere Suite, head to [the wiki](https://github.com/benirose/O_C-BenisphereSuite/wiki).

Benisphere takes the Hemisphere Suite in a new direction, adding new applets and some enhancements to existing ones, while also removing o_C style apps to make space for these changes.

### An alternate firmware to an alternate firmware??

Yes! This fork was an opportunity for me add stuff I wanted to see on my o_C without having to worry about running up against any legacy issues for other Hemipshere users. When I shared some of the progress I was making in my fork, people seemed excited and interested in trying it out, so I have made it an official fork rather than just my own personal project. I try to give back any improvements I make to the [main Hemispheres repo](https://github.com/Chysn/O_C-HemisphereSuite) when possible, but 1) it is not clear if that repo is still active and 2) there is barely any room remaining on the Teensy for improvements. By deciding that this version of the suite will only contain Hemisphere applets I was able to make room for new applets and other improvements. I do accept feature request issues and pull request, but the things that will make it in to this fork are still largely up to my discression.

### Ok, so what's changed?

As of v1.0, I have added two new applets inspired by very popular modules ([Mutable Instruments Grids](https://mutable-instruments.net/modules/grids/) and [Noise Engineering Mimetic Digitalis](https://noiseengineering.us/products/mimetic-digitalis)) as well as some minor improvements to existing apps. I've also removed all of the full-width o_C style apps to make space for these changes. To see all of the changes in detail, visit the [wiki](https://github.com/benirose/O_C-BenisphereSuite/wiki).

TricksterSam: This version modifies Runglbook to be more useful in building a Benjolin. CV2 is automatically XOR against the next bit in the shift register. Out2 provides this XOR value.

TricksterSam: This version also includes a new Applet called Relabi, which generates a complex waveform from four LFOs and a gate derived from a threshold on that waveform. Use this app to generate deterministic CV that feels chaotic on Output 1 and a gate that feels like a rhythm that is always slipping the pulse on Output 2. Run the relabi wave through Hemisphere's Schmidt trigger for more broken rhythms. The principle and purpose of relabi (music of the self-erasing pulse) is detailed [here](http://www.johnberndt.org/relabi/). The code is a complete C++ reimagining of Pure Data software designed for creating relabi. One function not yet available include setting the phase of the LFOs which they reset to when receiving a pulse in the first Gate input. Another function not available is the ability to save the current settings of the Relabi app.

### How do I try it?

Head over to the [releases](https://github.com/benirose/O_C-BenisphereSuite/releases) section and download the latest release. Follow the ["Method A" instructions](https://ornament-and-cri.me/firmware/#method_a) from the Ornament and Crime website, except use the hex file you downloaded from the releases section.

Alternatively, you can follow instructions in [this video](https://www.youtube.com/watch?v=dg_acylaMZU).

Note: you can reload any other previous firmware you had installed (stock o_C or Hemisphere Suite) just as easily.

### How do I build it?

You can download this repo and build the code following the ["Method B" instruction](https://ornament-and-cri.me/firmware/#method_b) from the Ornament and Crime website. Very specific legacy versions of the Arduino IDE and Teensyduino add-on are required to build, and are not installable on 64-bit only systems, like Mac OS. You must use an older version (Mojave or before) or a VM to install these versions.

### What's with the name?

Beta tester [@jroo](https://github.com/jroo) jokingly called it that, and it kind of stuck! It's supposed to be a bit tongue in cheek, so I hope it's not taken too seriously! Also it has a nicer ringer than "Hemisphere Suite BR Fork".

### Credits

This is a fork of [Hemisphere Suite](https://github.com/Chysn/O_C-HemisphereSuite) by Jason Justian (aka chysn). I could not have built Hemisphere Suite, so a million thanks to Jason for doing the true hard work and keeping it open source.

ornament**s** & crime**s** is a collaborative project by Patrick Dowling (aka pld), mxmxmx and Tim Churches (aka bennelong.bicyclist) (though mostly by pld and bennelong.bicyclist). it **(considerably) extends** the original firmware for the o_C / ASR eurorack module, designed by mxmxmx.

http://ornament-and-cri.me/
