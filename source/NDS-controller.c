/*
 * Copyright (c) 2015 Louis van Harten
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include "NDS-controller.h"


int main() {
    int pressed = 0, i;

    videoInit();

#ifndef EMULATOR
    while(1) { //breaks when connected properly
        consoleSelect(&top_screen);
        consoleClear();
        BG_PALETTE_SUB[255] = RGB15(6,7,14);
        BG_PALETTE[255] = RGB15(31,31,31);
        top_screen.cursorY = 7;
        iprintf("Select connection method");
        swiWaitForVBlank();
        scanKeys();
        pressed = keysDown();
        if(pressed&KEY_A) {
            consoleClear();
            if(WFCConnect(&top_screen)) {
                consoleClear();
                iprintf("Failed to connect. Retrying...\n");
                continue;
            } else {
                break;
            }
        }
        if(pressed&KEY_B) {
            consoleClear();
            if(ManualConnect(&top_screen, &bot_screen)) {
                consoleSelect(&bot_screen);
                consoleClear();
                iprintf("Failed to connect. Retrying...\n");
                continue;
            } else {
                break;
            }
        }
    }
#endif // EMULATOR

    consoleSelect(&top_screen);
    consoleClear();

    for(i=0; i<200; i+=10) {
        cpuStartTiming(1);
        timerStop(1);
        bgSetScroll(2,0,-i);
        bgUpdate();
        consoleClear();
        consoleSelect(&top_screen);
        iprintf("\nwaiting...");
        swiWaitForVBlank();
    }

    do {
        scanKeys();
        pressed = keysDown();
        consoleClear();
        consoleSelect(&top_screen);
        iprintf("\nWe're connected! :)\n");
        swiWaitForVBlank();
    } while (! (pressed&KEY_START));

    return 0;
}
