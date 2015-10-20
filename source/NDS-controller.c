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


void OnKeyPressed(int key) {
    if(key > 0) {
        iprintf("%c", key);
    }
}

int main() {
    PrintConsole top_screen;
    PrintConsole bot_screen;
    Keyboard *kbd;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&top_screen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bot_screen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
    consoleSetWindow(&bot_screen, 1,1,31,13); // Don't overwrite keyboard, small frame

    kbd = keyboardInit(0,1,BgType_Text4bpp, BgSize_T_256x256, 3,1,false,true);
    kbd->OnKeyPressed = OnKeyPressed;

    consoleSelect(&bot_screen);

    #ifndef EMULATOR
    while(1) { //breaks when connected properly
        consoleClear();
        iprintf("\nPress (A) to connect using WFC \n\n");
        iprintf("\nPress (B) to connect manually  \n");
        int pressed = 0;
        swiWaitForVBlank();
        scanKeys();
        pressed = keysDown();
        if(pressed&KEY_A) {
            consoleClear();
            if(WFCConnect(&bot_screen)) {
                consoleClear();
                iprintf("Failed to connect. Retrying...\n");
                continue;
            } else {
                break;
            }
        }
        if(pressed&(KEY_B)) {
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

    while(1) {
        consoleClear();
        iprintf("\nWe're connected! :)\n");
        swiWaitForVBlank();
    }

    return 0;
}
