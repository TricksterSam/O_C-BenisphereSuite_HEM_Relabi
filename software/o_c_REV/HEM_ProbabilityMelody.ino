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

#define HEM_PROB_MEL_MAX_WEIGHT 10
#define HEM_PROB_MEL_MAX_RANGE 60
#define HEM_PROB_MEL_MAX_LOOP_LENGTH 32

class ProbabilityMelody : public HemisphereApplet {
public:

    const char* applet_name() {
        return "ProbMeloD";
    }

    void Start() {
        down = 1;
        up = 12;
        pitch = 0;
    }

    void Controller() {
        loop_linker->RegisterMelo(hemisphere);

        // looping enabled from ProbDiv
        if (loop_linker->IsLooping() && !isLooping) {
            isLooping = true;
            GenerateLoop();
        }

        // reseed from ProbDiv
        if (loop_linker->ShouldReseed()) {
            GenerateLoop();
        }
        
        isLooping = loop_linker->IsLooping();


        int downCv = DetentedIn(0);
        int oldDown = down;
        if (downCv < 0) down = 1;        
        if (downCv > 0) {
            down = constrain(ProportionCV(downCv, HEM_PROB_MEL_MAX_RANGE + 1), 1, up);
        }

        int upCv = DetentedIn(1);
        int oldUp = up;
        if (upCv < 0) up = 1;        
        if (upCv > 0) {
            up = constrain(ProportionCV(upCv, HEM_PROB_MEL_MAX_RANGE + 1), down, HEM_PROB_MEL_MAX_RANGE);
        }

        // reseed loop if range has changed due to CV
        if (isLooping && (oldDown != down || oldUp != up)) {
            GenerateLoop();
        }

        if (Clock(0) || loop_linker->Ready()) {
            if (isLooping) {
                pitch = loop[loop_linker->GetLoopStep()] + 60;
            } else {
                pitch = GetNextWeightedPitch() + 60;
            }
            if (pitch != -1) {
                Out(0, MIDIQuantizer::CV(pitch));
                pulse_animation = HEMISPHERE_PULSE_ANIMATION_TIME_LONG;
            } else {
                Out(0, 0);
            }
        }

        if (pulse_animation > 0) {
            pulse_animation--;
        }

        // animate value changes
        if (value_animation > 0) {
          value_animation--;
        }
    }

    void View() {
        gfxHeader(applet_name());
        DrawParams();
        DrawKeyboard();
    }

    void OnButtonPress() {
        isEditing = !isEditing;
        ResetCursor();
    }

    void OnEncoderMove(int direction) {
        if (!isEditing) {
            cursor += direction;
            if (cursor < 0) cursor = 13;
            if (cursor > 13) cursor = 0;
            ResetCursor();
        } else {
            if (cursor < 12) {
                // editing note probability
                weights[cursor] = constrain(weights[cursor] += direction, 0, HEM_PROB_MEL_MAX_WEIGHT);
                value_animation = HEMISPHERE_CURSOR_TICKS;
            } else {
                // editing scaling
                if (cursor == 12) down = constrain(down += direction, 1, up);
                if (cursor == 13) up = constrain(up += direction, down, 60);
            }
            if (isLooping) {
                GenerateLoop(); // regenerate loop on any param changes
            }
        }
    }
        
    uint64_t OnDataRequest() {
        uint64_t data = 0;
        Pack(data, PackLocation {0, 4}, weights[0]);
        Pack(data, PackLocation {4, 4}, weights[1]);
        Pack(data, PackLocation {8, 4}, weights[2]);
        Pack(data, PackLocation {12, 4}, weights[3]);
        Pack(data, PackLocation {16, 4}, weights[4]);
        Pack(data, PackLocation {20, 4}, weights[5]);
        Pack(data, PackLocation {24, 4}, weights[6]);
        Pack(data, PackLocation {28, 4}, weights[7]);
        Pack(data, PackLocation {32, 4}, weights[8]);
        Pack(data, PackLocation {36, 4}, weights[9]);
        Pack(data, PackLocation {40, 4}, weights[10]);
        Pack(data, PackLocation {44, 4}, weights[11]);
        Pack(data, PackLocation {48, 6}, down);
        Pack(data, PackLocation {54, 6}, up);
        return data;
    }

    void OnDataReceive(uint64_t data) {
        weights[0] = Unpack(data, PackLocation {0,4});
        weights[1] = Unpack(data, PackLocation {4,4});
        weights[2] = Unpack(data, PackLocation {8,4});
        weights[3] = Unpack(data, PackLocation {12,4});
        weights[4] = Unpack(data, PackLocation {16,4});
        weights[5] = Unpack(data, PackLocation {20,4});
        weights[6] = Unpack(data, PackLocation {24,4});
        weights[7] = Unpack(data, PackLocation {28,4});
        weights[8] = Unpack(data, PackLocation {32,4});
        weights[9] = Unpack(data, PackLocation {36,4});
        weights[10] = Unpack(data, PackLocation {40,4});
        weights[11] = Unpack(data, PackLocation {44,4});
        down = Unpack(data, PackLocation{48,6});
        up = Unpack(data, PackLocation{54,6});
    }

protected:
    void SetHelp() {
        //                               "------------------" <-- Size Guide
        help[HEMISPHERE_HELP_DIGITALS] = "1=Clock";
        help[HEMISPHERE_HELP_CVS]      = "1=LowRng 2=HighRng";
        help[HEMISPHERE_HELP_OUTS]     = "A=Out";
        help[HEMISPHERE_HELP_ENCODER]  = "Push to edit value";
        //                               "------------------" <-- Size Guide
    }
    
private:
    int cursor;
    bool isEditing = false;
    int weights[12] = {10,0,0,2,0,0,0,2,0,0,4,0};
    int up;
    int down;
    int pitch;
    bool isLooping = false;
    int loop[HEM_PROB_MEL_MAX_LOOP_LENGTH];

    ProbLoopLinker *loop_linker = loop_linker->get();

    int pulse_animation = 0;
    int value_animation = 0;
    const uint8_t x[12] = {2, 7, 10, 15, 18, 26, 31, 34, 39, 42, 47, 50};
    const uint8_t p[12] = {0, 1,  0,  1,  0,  0,  1,  0,  1,  0,  1,  0};
    const char* n[12] = {"C", "C", "D", "D", "E", "F", "F", "G", "G", "A", "A", "B"};

    int GetNextWeightedPitch() {
        int total_weights = 0;

        for(int i = down-1; i < up; i++) {
            total_weights += weights[i % 12];
        }

        int rnd = random(0, total_weights + 1);
        for(int i = down-1; i < up; i++) {
            int weight = weights[i % 12];
            if (rnd <= weight && weight > 0) {
                return i;
            }
            rnd -= weight;
        }
        return -1;
    }

    void GenerateLoop() {
        // always fill the whole loop to make things easier
        for (int i = 0; i < HEM_PROB_MEL_MAX_LOOP_LENGTH; i++) {
            loop[i] = GetNextWeightedPitch();
        }
    }

    void DrawKeyboard() {
        // Border
        gfxFrame(0, 27, 63, 32);

        // White keys
        for (uint8_t x = 0; x < 8; x++)
        {
            if (x == 3 || x == 7) {
                gfxLine(x * 8, 27, x * 8, 58);
            } else {
                gfxLine(x * 8, 43, x * 8, 58);
            }
        }

        // Black keys
        for (uint8_t i = 0; i < 6; i++)
        {
            if (i != 2) { // Skip the third position
                uint8_t x = (i * 8) + 6;
                gfxInvert(x, 28, 5, 15);
            }
        }
    }

    void DrawParams() {
        int note = pitch % 12;
        int octave = (pitch - 60) / 12;

        for (uint8_t i = 0; i < 12; i++)
        {
            uint8_t xOffset = x[i] + (p[i] ? 1 : 2);
            uint8_t yOffset = p[i] ? 31 : 45;

            if (pulse_animation > 0 && note == i) {
                gfxRect(xOffset - 1, yOffset, 3, 10);
            } else {
                if (isEditing && i == cursor) {
                    // blink line when editing
                    if (CursorBlink()) {
                        gfxLine(xOffset, yOffset, xOffset, yOffset + 10);
                    } else {
                        gfxDottedLine(xOffset, yOffset, xOffset, yOffset + 10);
                    }
                } else {
                    gfxDottedLine(xOffset, yOffset, xOffset, yOffset + 10);    
                }
                gfxLine(xOffset - 1, yOffset + 10 - weights[i], xOffset + 1, yOffset + 10 - weights[i]);
            }
        }

        // cursor for keys
        if (!isEditing) {
            if (cursor < 12) {
                gfxCursor(x[cursor] - 1, p[cursor] ? 24 : 60, p[cursor] ? 5 : 6);
                gfxCursor(x[cursor] - 1, p[cursor] ? 25 : 61, p[cursor] ? 5 : 6);
            }
            if (cursor == 12) gfxCursor(7, 23, 22);
            if (cursor == 13) gfxCursor(37, 23, 22);
        }

        // scaling params

        gfxIcon(0, 13, DOWN_BTN_ICON);
        gfxPrint(8, 15, ((down - 1) / 12) + 1);
        gfxPrint(13, 15, ".");
        gfxPrint(17, 15, ((down - 1) % 12) + 1);

        gfxIcon(30, 16, UP_BTN_ICON);
        gfxPrint(38, 15, ((up - 1) / 12) + 1);
        gfxPrint(43, 15, ".");
        gfxPrint(47, 15, ((up - 1) % 12) + 1);

        if (isEditing) {
            if (cursor == 12) gfxInvert(9, 14, 21, 9);
            if (cursor == 13) gfxInvert(38, 14, 21, 9);
        }

        if (pulse_animation > 0) {
        //     int note = pitch % 12;
        //     int octave = (pitch - 60) / 12;

        //     gfxRect(x[note] + (p[note] ? 0 : 1), p[note] ? 29 : 54, 3, 2);
            gfxRect(58, 54 - (octave * 6), 3, 3);
        }

        if (value_animation > 0 && cursor < 12) {
          gfxRect(1, 15, 60, 10);
          gfxInvert(1, 15, 60, 10);

          gfxPrint(18, 16, n[cursor]);
          if (p[cursor]) {
            gfxPrint(24, 16, "#");
          }
          gfxPrint(34, 16, weights[cursor]);
          gfxInvert(1, 15, 60, 10);
        }
    }
};

ProbabilityMelody ProbabilityMelody_instance[2];

void ProbabilityMelody_Start(bool hemisphere) {ProbabilityMelody_instance[hemisphere].BaseStart(hemisphere);}
void ProbabilityMelody_Controller(bool hemisphere, bool forwarding) {ProbabilityMelody_instance[hemisphere].BaseController(forwarding);}
void ProbabilityMelody_View(bool hemisphere) {ProbabilityMelody_instance[hemisphere].BaseView();}
void ProbabilityMelody_OnButtonPress(bool hemisphere) {ProbabilityMelody_instance[hemisphere].OnButtonPress();}
void ProbabilityMelody_OnEncoderMove(bool hemisphere, int direction) {ProbabilityMelody_instance[hemisphere].OnEncoderMove(direction);}
void ProbabilityMelody_ToggleHelpScreen(bool hemisphere) {ProbabilityMelody_instance[hemisphere].HelpScreen();}
uint64_t ProbabilityMelody_OnDataRequest(bool hemisphere) {return ProbabilityMelody_instance[hemisphere].OnDataRequest();}
void ProbabilityMelody_OnDataReceive(bool hemisphere, uint64_t data) {ProbabilityMelody_instance[hemisphere].OnDataReceive(data);}
