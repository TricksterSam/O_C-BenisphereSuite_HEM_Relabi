// Copyright (c) 2018, Jason Justian
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "vector_osc/HSVectorOscillator.h"
#include "vector_osc/WaveformManager.h"

class Relabi : public HemisphereApplet {
public:

    const char* applet_name() {
        return "Relabi";
    }

    void Start() {
        freq[0] = 450;
        freq[1] = 300;
        freq[2] = 100;
        freq[3] = 50;
        xmod[0] = 18;
        xmod[1] = 18;
        xmod[2] = 18;
        xmod[3] = 18;
        threshold = 35;
        for (uint8_t count = 0; count < 4; count++) {
            freqKnob[count] = 600.0 * pow((freq[count]/30000.0), 1.0/6.0);
            xmodKnob[count] = xmod[count];
            osc[count] = WaveformManager::VectorOscillatorFromWaveform(35);
            osc[count].SetFrequency(freq[count]);
            #ifdef BUCHLA_4U
              osc[count].Offset((12 << 7) * 4);
              osc[count].SetScale((12 << 7) * 4);
            #else
              osc[count].SetScale((12 << 7) * 3);
            #endif
        }
    }

    void Controller() {
        for (uint8_t count = 0; count < 4; count++) {
            osc[count].SetFrequency(freq[count] + (freq[count] * xmod[count] / 100 * (sample[(count + 3) % 4]) / 10000 + 0.5));
            sample[count] = osc[count].Next();
        }
        int relabiWave = (sample[0] + sample[1] + sample[2] + sample[3]) / 4;
        int threshGate = 0;
        if (relabiWave > (threshold * 46.08)) {threshGate = 4608;} else {threshGate = 0;}
        Out(0, relabiWave);
        Out(1, threshGate);
    }

    void View() {     
        gfxHeader(applet_name());
        // Display OSC label and value
        gfxPrint(15, 15, "OSC");
        gfxPrint(35, 15, selectedChannel);
        
        // Display FREQ label and value
        gfxPrint(1, 26, "FREQ");
        simfloat fDisplay = freq[selectedChannel];
        gfxPrint(1, 35, ones(fDisplay));
        if (fDisplay < 10000) {
          gfxPrint(".");
          int h = hundredths(fDisplay);
          if (h < 10) gfxPrint("0");
          gfxPrint(h);  
        };

        
        // Display MOD label and value
        gfxPrint(31, 26, "XMOD");
        gfxPrint(31, 35, xmod[selectedChannel]);
        
        // Display PHAS label and value
        gfxPrint(1, 46, "PHAS");
        gfxPrint(1, 55, phase[selectedChannel]);
        
        // Display THRS label and value
        gfxPrint(31, 46, "THRS");
        gfxPrint(31, 55, threshold);



        switch (selectedParam) {
        case 0:
            gfxCursor(15, 23, 30);
            break;
        case 1:
            gfxCursor(1, 43, 30);
            break;
        case 2:
            gfxCursor(31, 43, 30);
            break;
        case 3:
            gfxCursor(1, 63, 30);
            break;
        case 4:
            gfxCursor(31, 63, 30);
            break;
        }
    }

    void OnButtonPress() {
        ++cursor;
        cursor = cursor % 5;
        selectedParam = cursor;
        ResetCursor();
    }

    void OnEncoderMove(int direction) {
        switch (selectedParam) {
        case 0: // Cycle through parameters when selecting OSC
            selectedChannel = selectedChannel + direction + 4;
            selectedChannel = selectedChannel % 4;
            break;
        case 1: // FREQ
            freqKnob[selectedChannel] += (direction);
            freqKnob[selectedChannel] = constrain(freqKnob[selectedChannel], 0, 600);
            freq[selectedChannel] = (30000.0f * pow((freqKnob[selectedChannel]/ 600.0f), 6.0f));
            break;
        case 2: // XMOD
            xmodKnob[selectedChannel] += (direction);
            xmodKnob[selectedChannel] = constrain(xmodKnob[selectedChannel], 0, 100);
            xmod[selectedChannel] = xmodKnob[selectedChannel];
            break;
        case 3: // PHAS
            phase[selectedChannel] += direction;
            phase[selectedChannel] = constrain(phase[selectedChannel], 0, 100);
            break;
        case 4: // THRS
            threshold += direction;
            threshold = constrain(threshold, 0, 100);
            break;
        }

    }
        
    uint64_t OnDataRequest() {
        // We need to store freq[ch], xmod[ch], phase[ch], and thresh.
        uint64_t data = 0;
        // for (int i = 0; i < 4; ++i) {
        //   int exponent = 0;
        //   if (freq[i] > 250) exponent++;
        //   if (freq[i] > 1000) exponent++;
        //   if (freq[i] > 10000) exponent++;
        //   Pack(data, PackLocation {0 + i * 10, 2}, exponent);

        //   int mantissa = xmod[i] / pow10_lut[exponent];
        //   Pack(data, PackLocation {0 + i * 10 + 2, 8}, mantissa);
        // }
        
        return data;
    }
    
    void OnDataReceive(uint64_t data) {
    //     for (int i = 0; i < 2; ++i) {
    //       int exponent = Unpack(data, PackLocation {12 + i * 10, 2});
    //       int mantissa = Unpack(data, PackLocation {12 + i * 10 + 2, 8});
          
    //       freq[i] = mantissa * pow10_lut[exponent];
    //     }
    //     SwitchWaveform(0, Unpack(data, PackLocation {0,6}));
    //     SwitchWaveform(1, Unpack(data, PackLocation {6,6}));
    }

protected:
    void SetHelp() {
        //                               "------------------" <-- Size Guide
        help[HEMISPHERE_HELP_DIGITALS] = "1=Reset 2=NA";
        help[HEMISPHERE_HELP_CVS]      = "1=Freq 2=Thresh";
        help[HEMISPHERE_HELP_OUTS]     = "A=Wave B=Gate";
        help[HEMISPHERE_HELP_ENCODER]  = "Frq/Mod/Phse/Thrs";
        //                               "------------------" <-- Size Guide
    }
    
private:
    // static constexpr int pow10_lut[] = { 1, 10, 100, 1000 };
    // int cursor; // 0=Freq A; 1=Cross Mod A; 2=Phase A; 3=Freq B; 4=Cross Mod B; etc.
    // VectorOscillator osc[4];
    // constexpr static uint8_t ch = 4;
    // constexpr static uint8_t numParams = 5;
    // simfloat freq[ch]; // in centihertz
    // uint16_t xmod[ch];
    // uint16_t phase[ch];
    // uint16_t threshold;
    // uint8_t selectedChannel = 0;
    // uint8_t selectedParam = 0;
    // int sample[ch];
    // uint32_t outFreq[ch];
    // simfloat freqKnob[4];
    // uint16_t xmodKnob[4];
    // uint8_t countLimit = 0;
    // uint16_t waveform_number[4];
    static constexpr int pow10_lut[] = { 1, 10, 100, 1000 };
    int cursor; // 0=Freq A; 1=Cross Mod A; 2=Phase A; 3=Freq B; 4=Cross Mod B; etc.
    VectorOscillator osc[4];
    constexpr static uint8_t ch = 4;
    constexpr static uint8_t numParams = 5;
    uint16_t threshold;
    uint8_t selectedOsc;
    simfloat freq[ch]; // in centihertz
    uint16_t xmod[ch];
    uint8_t selectedXmod;
    uint16_t phase[ch];
    int selectedChannel = 0;
    uint8_t selectedParam = 0;
    int sample[ch];
    uint32_t outFreq[ch];
    simfloat freqKnob[4];
    simfloat xmodKnob[4];
    uint8_t countLimit = 0;
    // Settings
    int waveform_number[4];    
    int ones(int n) {return (n / 100);}
    int hundredths(int n) {return (n % 100);}
};



////////////////////////////////////////////////////////////////////////////////
//// Hemisphere Applet Functions
///
///  Once you run the find-and-replace to make these refer to Relabi,
///  it's usually not necessary to do anything with these functions. You
///  should prefer to handle things in the HemisphereApplet child class
///  above.
////////////////////////////////////////////////////////////////////////////////
Relabi Relabi_instance[2];

void Relabi_Start(bool hemisphere) {Relabi_instance[hemisphere].BaseStart(hemisphere);}
void Relabi_Controller(bool hemisphere, bool forwarding) {Relabi_instance[hemisphere].BaseController(forwarding);}
void Relabi_View(bool hemisphere) {Relabi_instance[hemisphere].BaseView();}
void Relabi_OnButtonPress(bool hemisphere) {Relabi_instance[hemisphere].OnButtonPress();}
void Relabi_OnEncoderMove(bool hemisphere, int direction) {Relabi_instance[hemisphere].OnEncoderMove(direction);}
void Relabi_ToggleHelpScreen(bool hemisphere) {Relabi_instance[hemisphere].HelpScreen();}
uint64_t Relabi_OnDataRequest(bool hemisphere) {return Relabi_instance[hemisphere].OnDataRequest();}
void Relabi_OnDataReceive(bool hemisphere, uint64_t data) {Relabi_instance[hemisphere].OnDataReceive(data);}
