Welcome to Benisphere Suite with Relabi App
===

## An active fork expanding upon Hemisphere and Benisphere Suite for the Ornament and Crime.

Benisphere takes the Hemisphere Suite in a new direction, adding new applets and some enhancements to existing ones, while also removing o_C style apps to make space for these changes.

### What's different?

This version modifies Runglbook to be more useful in building a Benjolin. CV2 is automatically XOR against the next bit in the shift register. Out2 provides this XOR value.

This version also includes a new Applet called Relabi, which generates a complex waveform from four LFOs and a gate derived from a threshold on that waveform. Use this app to generate deterministic CV that feels chaotic on Output 1 and a gate that feels like a rhythm that is always slipping the pulse on Output 2. Run the relabi wave through Hemisphere's Schmidt trigger for more broken rhythms. The principle and purpose of relabi (music of the self-erasing pulse) is detailed [here](http://www.johnberndt.org/relabi/). The code is a complete C++ reimagining of Pure Data software designed for creating relabi. One function not yet available include setting the phase of the LFOs which they reset to when receiving a pulse in the first Gate input. Another function not available is the ability to save the current settings of the Relabi app.

### How do I try it?

Download the latest release. Benispheres has switched to using [platform.io](https://github.com/benirose/O_C-BenisphereSuite/issues/67) to compile the firmware. The Arduino IDE will fail. You can [easily add](https://www.youtube.com/watch?v=dany7ae_0ks) the PlatformIO IDE and Arduino extensions to Visual Studio Code and it will add all the tools you need.

Note: you can reload any other previous firmware you had installed (stock o_C or Hemisphere Suite) just as easily.

### How do I build it?

You can download this repo and build the code following the ["Method B" instruction](https://ornament-and-cri.me/firmware/#method_b) from the Ornament and Crime website. Very specific legacy versions of the Arduino IDE and Teensyduino add-on are required to build, and are not installable on 64-bit only systems, like Mac OS. You must use an older version (Mojave or before) or a VM to install these versions.

### Credits

This is a fork of benirose's fork of [Hemisphere Suite](https://github.com/Chysn/O_C-HemisphereSuite) by Jason Justian (aka chysn).

ornament**s** & crime**s** is a collaborative project by Patrick Dowling (aka pld), mxmxmx and Tim Churches (aka bennelong.bicyclist) (though mostly by pld and bennelong.bicyclist). it **(considerably) extends** the original firmware for the o_C / ASR eurorack module, designed by mxmxmx.

http://ornament-and-cri.me/
