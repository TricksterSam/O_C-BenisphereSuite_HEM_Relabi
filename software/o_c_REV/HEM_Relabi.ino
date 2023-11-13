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
        freq[0] = 30;
        freq[1] = 50;
        freq[2] = 70;
        freq[3] = 110;
        xmod[0] = 20;
        xmod[1] = 20;
        xmod[2] = 20;
        xmod[3] = 20;
        for (uint8_t count = 0; count < 4; count++) {
            if (freq[count] > 2000) {freqKnob[count] = (freq[count] - 2000) / 100 + 380;}
            if (freq[count] < 2000) {freqKnob[count] = (freq[count] - 200) / 10 + 200;}
            if (freq[count] < 200) {freqKnob[count] = freq[count];}
            xmodKnob[count] = xmod[count];
            osc[count] = WaveformManager::VectorOscillatorFromWaveform(35);
            osc[count].SetFrequency(freq[count]);
            #ifdef BUCHLA_4U
              osc[count].Offset((12 << 7) * 4);
              osc[count].SetScale((12 << 7) * 4);
            #else
              osc[count].SetScale((12 << 7) * 6);
            #endif
        }
    }

    void Controller() {
        simfloat cvIn = (10*In(0)/4096 + 1);
        if (oldClock != Clock(0)) {
            if (oldClock = 1) {
                for (uint8_t pcount = 0; pcount < 4; pcount++) {
                    if (Clock(0) > 0) {
                        int setPhase = round(phase[pcount] / 100 * 12);
                        osc[pcount].SetPhase(setPhase);
                    }
                }
            }
        }
        oldClock = Clock(0);
        /*if (Clock(0) > 0) {
            for (uint8_t pcount = 0; pcount < 4; pcount++) {
                if (Clock(0) > 0) {
                    int setPhase = round(phase[pcount] / 100 * 12);
                    osc[pcount].SetPhase(setPhase);
                }
            }
        }*/
        if (clkDiv == 0) {
            for (uint8_t count = 0; count < 4; count++) {
                simfloat setFreq = (freq[count] * cvIn * xmod[count] / 100 * (sample[(count + 3) % 4]) / 400 + 0.5) + (freq[count] * cvIn);
                osc[count].SetFrequency(setFreq);
                sample[count] = 4608 + (osc[count].Next()/ 2);
            }
        
        int wave1 = sample[0];
        int wave2 = sample[2];
        int relabiWave = (sample[0] + sample[1] + sample[2] + sample[3]) / 4;
        Out(0, wave1);
        Out(1, wave2);
        }
        clkDiv++;
        clkDiv = clkDiv % 4;
    }

    void View() {     
        // gfxPrint(sample[0]);
        gfxHeader(applet_name());

        // Display OSC label and value
        gfxPrint(15, 15, "OSC");
        gfxPrint(35, 15, selectedChannel);
        
        // Display FREQ label and value
        gfxPrint(1, 26, "FREQ");
        simfloat fDisplay = freq[selectedChannel];
        gfxPrint(1, 35, ones(fDisplay));
        if (fDisplay < 2000) {
            if (fDisplay < 199) {
                gfxPrint(".");
                int h = hundredths(fDisplay);
                if (h < 10) {gfxPrint("0");}
                gfxPrint(h);
            }
            else {
                gfxPrint(".");
                int t = hundredths(fDisplay);
                t = (t / 10) % 10;
                gfxPrint(t);
            }
        
        }  

        
        

        
        // Display MOD label and value
        gfxPrint(31, 26, "XMOD");
        gfxPrint(31, 35, xmod[selectedChannel]);
        
        // Display PHAS label and value
        gfxPrint(1, 46, "PHAS");
        gfxPrint(1, 55, phase[selectedChannel]);
        



        if (clkDiv == 0) {
                gfxRect(1, 63, sample[0] / 400, 1);
                gfxRect(31, 63, sample[2] / 400, 1);
            }



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
        cursor = cursor % 4;
        selectedParam = cursor;
        ResetCursor();
    }

    void OnEncoderMove(int direction) {
        switch (selectedParam) {
        case 0: // Cycle through parameters when selecting OSC
            selectedChannel = selectedChannel + direction + 4;
            selectedChannel = selectedChannel % 4;
            break;
        case 1: // FREQ (0-20.0)
            freqKnob[selectedChannel] += direction;
            if (freqKnob[selectedChannel] < 0) {freqKnob[selectedChannel] = 510;}
            if (freqKnob[selectedChannel] > 510) {freqKnob[selectedChannel] = 0;}
            if (freqKnob[selectedChannel] < 200) {
                freq[selectedChannel] = freqKnob[selectedChannel];
            }
            else if (freqKnob[selectedChannel] < 380) {
                freq[selectedChannel] = 200 + ((freqKnob[selectedChannel] - 200) * 10);
            }
            else {
                    freq[selectedChannel] = 2000 + ((freqKnob[selectedChannel] - 380) * 100);
                }
            break;
        case 2: // XMOD (0-100)
            xmodKnob[selectedChannel] += (direction);
            xmodKnob[selectedChannel] = xmodKnob[selectedChannel] + 101;
            xmodKnob[selectedChannel] = xmodKnob[selectedChannel] % 101;
            xmod[selectedChannel] = xmodKnob[selectedChannel];
            break;
        case 3: // PHAS (0-100)
            phase[selectedChannel] += direction;
            phase[selectedChannel] = phase[selectedChannel] + 101;
            phase[selectedChannel] = phase[selectedChannel] % 101;
            break;

        }

    }
        
    uint64_t OnDataRequest() {
        // We need to store freq[ch], xmod[ch], and phase[ch].
        uint64_t data = 0;
        // Pack(data, PackLocation {0, 9}, freq[0]);
        // Pack(data, PackLocation {10, 9}, freq[1]);
        // Pack(data, PackLocation {20, 9}, freq[2]);
        // Pack(data, PackLocation {30, 9}, freq[3]);
        // Pack(data, PackLocation {40, 7}, xmod[0]);
        // Pack(data, PackLocation {48, 7}, xmod[1]);
        // Pack(data, PackLocation {56, 7}, xmod[2]);
        // Pack(data, PackLocation {63, 7}, xmod[3]);
        return data;
    }
    
    void OnDataReceive(uint64_t data) {
        // freq[0] = Unpack(data, PackLocation {0, 9});
        // freq[1] = Unpack(data, PackLocation {10, 9});
        // freq[2] = Unpack(data, PackLocation {20, 9});
        // freq[3] = Unpack(data, PackLocation {30, 9});
        // xmod[0] = Unpack(data, PackLocation {40, 7});
        // xmod[1] = Unpack(data, PackLocation {48, 7});
        // xmod[2] = Unpack(data, PackLocation {56, 7});
        // xmod[3] = Unpack(data, PackLocation {63, 7});
    }

protected:
    void SetHelp() {
        //                               "------------------" <-- Size Guide
        help[HEMISPHERE_HELP_DIGITALS] = "1=Reset 2=NA";
        help[HEMISPHERE_HELP_CVS]      = "1=AllFreq 2=NA";
        help[HEMISPHERE_HELP_OUTS]     = "A=Wave B=Gate";
        help[HEMISPHERE_HELP_ENCODER]  = "Freq/XMod/Phase";
        //                               "------------------" <-- Size Guide
    }
    
private:
    static constexpr int pow10_lut[] = { 1, 10, 100, 1000 };
    int cursor; // 0=Freq A; 1=Cross Mod A; 2=Phase A; 3=Freq B; 4=Cross Mod B; etc.
    VectorOscillator osc[4];
    constexpr static uint8_t ch = 4;
    constexpr static uint8_t numParams = 5;
    uint8_t selectedOsc;
    simfloat freq[ch]; // in centihertz
    uint16_t xmod[ch];
    uint8_t selectedXmod;
    uint16_t phase[ch];
    int selectedChannel = 0;
    uint8_t selectedParam = 0;
    int sample[ch];
    simfloat outFreq[ch];
    simfloat freqKnob[4];
    uint16_t xmodKnob[4];
    uint8_t countLimit = 0;
    int waveform_number[4];    
    int ones(int n) {return (n / 100);}
    int hundredths(int n) {return (n % 100);}
    int valueToDisplay;
    uint16_t clkDiv = 0; // clkDiv allows us to calculate every other tick to save cycles
    uint8_t oldClock = 0;
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
