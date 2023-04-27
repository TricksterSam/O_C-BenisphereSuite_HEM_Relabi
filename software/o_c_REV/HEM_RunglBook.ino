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

// Modified by Samuel Burt 2023


class RunglBook : public HemisphereApplet {
public:

    const char* applet_name() {
        return "RunglBook";
    }

    void Start() {
        // threshold = (12 << 7) * 2;
        mask = 0x07; // default mask is 0x07 (last three bits)
    }

    void Controller() {
        byte xorbit = 1;
        
        if (Clock(0)) {
            if (Gate(1)) {
                // Digital 2 freezes the buffer, so just rotate left
                reg = (reg << 1) | ((reg >> 7) & 0x01);
            } else {
                byte second_msb = (reg << 1) & 0x01;
                // byte gatebit = (In(0) > threshold);
                // Treshhold was variable. Changed behavior
                gatebit = (In(0) > 3072);
                xorbit = second_msb ^ gatebit; 
                reg = (reg << 1) | xorbit;
                
            }


            bitdepthcv = ((In(1) * 4) / 8192);
            bitdepth = mask + bitdepthcv; 
            bitdepth = constrain(bitdepth, 2, 8);
            shiftedMask = (1 << bitdepth) -1; // create a mask with the desired number of bits
            rungle = Proportion(reg & shiftedMask, shiftedMask, HEMISPHERE_MAX_CV);
            // int xor_out = xorbit ? 8000 : 0;
            scaled_gatebit = gatebit * 20000;
            

            Out(0, rungle);
            Out(1, scaled_gatebit); // high output when gate exceeds threshold, also timed to new cv values

            
        }
    }
    

    void View() {
        gfxHeader(applet_name());
        // gfxPrint(1, 15, "Thr:");
        // gfxPrintVoltage(threshold);
        // App previously allowed a threshold selection. Changing behavior to allow change of bitdepth of the output.
        gfxPrint(1, 15, "Bit:");
        gfxPrint(mask);
        gfxPrint(" ");
        gfxPrint(bitdepth);
        gfxSkyline();
    }

    void OnButtonPress() { }

    void OnEncoderMove(int direction) {
        // threshold += (direction * 128);
        // threshold = constrain(threshold, (12 << 7), (12 << 7) * 5); // 1V - 5V
        // App previously allowed a threshold selection. Changing behavior to allow change of bitdepth of the output.
        mask += direction; // update mask based on encoder direction
        mask = constrain(mask, 3, 8); // constrain mask to the range of 3 to 8 bits
    }
        
    uint64_t OnDataRequest() {
        uint64_t data = 0;
        // Pack(data, PackLocation {0,16}, threshold);
        Pack(data, PackLocation {0,16}, mask);
        return data;
    }
    void OnDataReceive(uint64_t data) {
        // threshold = Unpack(data, PackLocation {0,16});
        mask = Unpack(data, PackLocation {0,16});
    }

protected:
    void SetHelp() {
        //                               "------------------" <-- Size Guide
        help[HEMISPHERE_HELP_DIGITALS] = "1=Clock 2=Freeze";
        help[HEMISPHERE_HELP_CVS]      = "1=Signal 2=Bit Depth";
        help[HEMISPHERE_HELP_OUTS]     = "A=Rungle B=Gate";
        help[HEMISPHERE_HELP_ENCODER]  = "Bit Depth";
        //                               "------------------" <-- Size Guide
    }
    
private:
    byte reg;
    byte gatebit;
    // uint16_t threshold;
    int16_t mask;
    int16_t bitdepth;
    float bitdepthcv;
    uint16_t scaled_gatebit;
    int rungle;
    int shiftedMask;
};


RunglBook RunglBook_instance[2];

void RunglBook_Start(bool hemisphere) {RunglBook_instance[hemisphere].BaseStart(hemisphere);}
void RunglBook_Controller(bool hemisphere, bool forwarding) {RunglBook_instance[hemisphere].BaseController(forwarding);}
void RunglBook_View(bool hemisphere) {RunglBook_instance[hemisphere].BaseView();}
void RunglBook_OnButtonPress(bool hemisphere) {RunglBook_instance[hemisphere].OnButtonPress();}
void RunglBook_OnEncoderMove(bool hemisphere, int direction) {RunglBook_instance[hemisphere].OnEncoderMove(direction);}
void RunglBook_ToggleHelpScreen(bool hemisphere) {RunglBook_instance[hemisphere].HelpScreen();}
uint64_t RunglBook_OnDataRequest(bool hemisphere) {return RunglBook_instance[hemisphere].OnDataRequest();}
void RunglBook_OnDataReceive(bool hemisphere, uint64_t data) {RunglBook_instance[hemisphere].OnDataReceive(data);}
