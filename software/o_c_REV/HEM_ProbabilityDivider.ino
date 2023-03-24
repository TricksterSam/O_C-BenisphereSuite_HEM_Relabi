// Copyright (c) 2022, Benjamin Rosenbach
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

#include "HSProbLoopLinker.h" // singleton for linking ProbDiv and ProbMelo

#define HEM_PROB_DIV_MAX_WEIGHT 15
#define HEM_PROB_DIV_MAX_LOOP_LENGTH 32

class ProbabilityDivider : public HemisphereApplet {
public:

    const char* applet_name() {
        return "ProbDiv";
    }

    void Start() {
        weight_1 = 0;
        weight_2 = 0;
        weight_4 = 0;
        weight_8 = 0;
        loop_length = 0;
        loop_index = 0;
        loop_step = 0;
        skip_steps = 0;
        ForEachChannel(ch) {
            GateOut(ch, false);
        }
    }

    void Controller() {
        loop_linker->RegisterDiv(hemisphere);

        // CV 1 control over loop length
        int lengthCv = DetentedIn(0);
        if (lengthCv < 0) loop_length = 0;        
        if (lengthCv > 0) {
            loop_length = constrain(ProportionCV(lengthCv, HEM_PROB_DIV_MAX_LOOP_LENGTH + 1), 0, HEM_PROB_DIV_MAX_LOOP_LENGTH);
        }

        loop_linker->SetLooping(loop_length > 0);

        // reset
        if (Clock(1)) {
            loop_step = 0;
            loop_index = 0;
            skip_steps = 0;
            reset_animation = HEMISPHERE_PULSE_ANIMATION_TIME_LONG;
        }

        if (Clock(0)) {
            int reseed = DetentedIn(1);
            // trigger reseed if CV2 is > 2.5v
            if (reseed > (HEMISPHERE_MAX_CV >> 1) && !reseed_high) {
                GenerateLoop(false);
                loop_linker->Reseed();
                reseed_high = true;
                reseed_animation = HEMISPHERE_PULSE_ANIMATION_TIME_LONG;
            }

            if (reseed < (HEMISPHERE_MAX_CV >> 1) && reseed_high) {
                reseed_high = false;
            }

            // reset loop
            if (loop_length > 0 && loop_step >= loop_length) {
                loop_step = 0;
                loop_index = 0;
                skip_steps = 0;
                reset_animation = HEMISPHERE_PULSE_ANIMATION_TIME_LONG;
            } 

            loop_linker->SetLoopStep(loop_index);

            // continue with active division
            if (--skip_steps > 0) {
                if (loop_length > 0) {
                    loop_step++;
                }
                ClockOut(1);
                return;
            }

            // get next weighted div or next div from loop
            if (loop_length > 0) {
                skip_steps = GetNextLoopDiv();
            } else {
                skip_steps = GetNextWeightedDiv();
            }

            // no weights are set, don't do anything
            if (skip_steps == 0) {
                return;
            }

            ClockOut(0);
            loop_linker->Trigger();
            pulse_animation = HEMISPHERE_PULSE_ANIMATION_TIME;
        }

        if (pulse_animation > 0) {
            pulse_animation--;
        }
        if (reseed_animation > 0) {
            reseed_animation--;
        }
        if (reset_animation > 0) {
            reset_animation--;
        }
    }

    void View() {
        gfxHeader(applet_name());
        DrawInterface();
    }

    void OnButtonPress() {
        if (++cursor > 4) cursor = 0;
    }

    void OnEncoderMove(int direction) {
        if (cursor == 0) weight_1 = constrain(weight_1 += direction, 0, HEM_PROB_DIV_MAX_WEIGHT);
        if (cursor == 1) weight_2 = constrain(weight_2 += direction, 0, HEM_PROB_DIV_MAX_WEIGHT);
        if (cursor == 2) weight_4 = constrain(weight_4 += direction, 0, HEM_PROB_DIV_MAX_WEIGHT);
        if (cursor == 3) weight_8 = constrain(weight_8 += direction, 0, HEM_PROB_DIV_MAX_WEIGHT);
        if (cursor == 4) {
            int old = loop_length;
            loop_length = constrain(loop_length += direction, 0, HEM_PROB_DIV_MAX_LOOP_LENGTH);
            if (old == 0 && loop_length > 0) {
                // seed loop
                GenerateLoop(true);
            }
        }
        if (cursor < 4 && loop_length > 0) {
          GenerateLoop(false);
        }
    }
        
    uint64_t OnDataRequest() {
        uint64_t data = 0;
        // example: pack property_name at bit 0, with size of 8 bits
        Pack(data, PackLocation {0,4}, weight_1); 
        Pack(data, PackLocation {4,4}, weight_2); 
        Pack(data, PackLocation {8,4}, weight_4); 
        Pack(data, PackLocation {12,4}, weight_8); 
        Pack(data, PackLocation {16,8}, loop_length);
        return data;
    }

    void OnDataReceive(uint64_t data) {
        // example: unpack value at bit 0 with size of 8 bits to property_name
        weight_1 = Unpack(data, PackLocation {0,4});
        weight_2 = Unpack(data, PackLocation {4,4});
        weight_4 = Unpack(data, PackLocation {8,4});
        weight_8 = Unpack(data, PackLocation {12,4});
        loop_length = Unpack(data, PackLocation {16,8});
        if (loop_length > 0) {
            // seed loop
            GenerateLoop(true);
        }
    }

protected:
    void SetHelp() {
        //                               "------------------" <-- Size Guide
        help[HEMISPHERE_HELP_DIGITALS] = "1=Clock  2=Reset";
        help[HEMISPHERE_HELP_CVS]      = "1=Length 2=Reseed";
        help[HEMISPHERE_HELP_OUTS]     = "A=Div    B=Skips";
        help[HEMISPHERE_HELP_ENCODER]  = "Weights/Loop";
        //                               "------------------" <-- Size Guide
    }
    
private:
    int cursor;
    int weight_1;
    int weight_2;
    int weight_4;
    int weight_8;
    int loop_length;
    int loop[HEM_PROB_DIV_MAX_LOOP_LENGTH];
    int loop_index;
    int loop_step;
    // used to keep track of reseed cv inputs so it only reseeds on rising edge
    bool reseed_high;

    int skip_steps;
    int pulse_animation = 0;
    int reseed_animation = 0;
    int reset_animation = 0;

    ProbLoopLinker *loop_linker = loop_linker->get();

    // pointer arrays that make loops easier
    const int *weights[4] = {&weight_1, &weight_2, &weight_4, &weight_8};
    const int divs[4] = {1, 2, 4, 8};
    
    void DrawInterface() {
        // divisions
        for(int i = 0; i < 4; i++) {
          gfxPrint(1, 15 + (i*10), "/");
          gfxPrint(divs[i]);
          DrawKnobAt(20, 15 + (i*10), 40, *weights[i], cursor == i);

          // flash division when triggered
          if (pulse_animation > 0 && skip_steps == divs[i]) {
            gfxInvert(1, 15 + (i*10), 12, 8);
          }
        }

        // loop
        gfxIcon(4, 55, LOOP_ICON);
        // flash icon on reseed
        if (reseed_animation > 0) {
            gfxInvert(4, 55, 12, 8);
        }
        if (loop_length == 0) {
            gfxPrint(19, 55, "off");
        } else {
            gfxPrint(19, 55, loop_length);
        }
        if (cursor == 4) gfxCursor(19, 63, 18);

        if (reset_animation > 0) {
            gfxPrint(52, 55, "R");
        }
    }

    void DrawKnobAt(byte x, byte y, byte len, byte value, bool is_cursor) {
        byte p = is_cursor ? 1 : 3;
        byte w = Proportion(value, HEM_PROB_DIV_MAX_WEIGHT, len);
        gfxDottedLine(x, y + 4, x + len, y + 4, p);
        gfxRect(x + w, y, 2, 7);
    }

    int GetNextWeightedDiv() {
        int total_weights = 0;

        for(int i = 0; i < 4; i++) {
            total_weights += *weights[i];
        }

        int rnd = random(0, total_weights + 1);
        for(int i = 0; i < 4; i++) {
          if (rnd <= *weights[i] && *weights[i] > 0) {
            return divs[i];
          }
          rnd -= *weights[i];
        }
        return 0;
    }

    void GenerateLoop(bool restart) {
        memset(loop, 0, sizeof(loop)); // reset loop
        if (restart) {
            loop_step = 0;
            loop_index = 0;
        }
        int index = 0;
        int counter = 0;
        while (counter < HEM_PROB_DIV_MAX_LOOP_LENGTH) {
            int div = GetNextWeightedDiv();
            if (div == 0) {
                break;
            }
            loop[index] = div;
            index++;
            counter += div;
        }
    }

    int GetNextLoopDiv() {
        int value = loop[loop_index];
        if (value == 0) {
            // we reseeded loop and now nothing is at this index, start over
            loop_index = 0;
            loop_step = 0;
            value = loop[loop_index];
        }
        loop_index++;
        loop_step++;
        return value;
    }
};


////////////////////////////////////////////////////////////////////////////////
//// Hemisphere Applet Functions
///
///  Once you run the find-and-replace to make these refer to ProbabilityDivider,
///  it's usually not necessary to do anything with these functions. You
///  should prefer to handle things in the HemisphereApplet child class
///  above.
////////////////////////////////////////////////////////////////////////////////
ProbabilityDivider ProbabilityDivider_instance[2];

void ProbabilityDivider_Start(bool hemisphere) {ProbabilityDivider_instance[hemisphere].BaseStart(hemisphere);}
void ProbabilityDivider_Controller(bool hemisphere, bool forwarding) {ProbabilityDivider_instance[hemisphere].BaseController(forwarding);}
void ProbabilityDivider_View(bool hemisphere) {ProbabilityDivider_instance[hemisphere].BaseView();}
void ProbabilityDivider_OnButtonPress(bool hemisphere) {ProbabilityDivider_instance[hemisphere].OnButtonPress();}
void ProbabilityDivider_OnEncoderMove(bool hemisphere, int direction) {ProbabilityDivider_instance[hemisphere].OnEncoderMove(direction);}
void ProbabilityDivider_ToggleHelpScreen(bool hemisphere) {ProbabilityDivider_instance[hemisphere].HelpScreen();}
uint64_t ProbabilityDivider_OnDataRequest(bool hemisphere) {return ProbabilityDivider_instance[hemisphere].OnDataRequest();}
void ProbabilityDivider_OnDataReceive(bool hemisphere, uint64_t data) {ProbabilityDivider_instance[hemisphere].OnDataReceive(data);}
