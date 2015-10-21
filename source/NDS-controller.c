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
    int bg_main, bg_sub;
    u16* videoMemoryMain, videoMemorySub;

    videoSetModeSub(MODE_5_2D);
    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    bg_main = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 2,0);
    decompress(bg_tempBitmap, bgGetGfxPtr(bg_main), LZ77Vram);

    bg_sub = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 5,0);
    decompress(bg_temp_subBitmap, bgGetGfxPtr(bg_sub), LZ77Vram);
    dmaCopy(bg_temp_subPal, BG_PALETTE_SUB, 512);

    consoleInit(&top_screen, 0,BgType_Text4bpp, BgSize_T_256x256, 4, 0, true, true);
    consoleInit(&bot_screen, 0,BgType_Text4bpp, BgSize_T_256x256, 4, 0, false, true);
    consoleSetWindow(&bot_screen, 1,1,30,22);
    consoleSetWindow(&top_screen, 3,13,25,10);

    kbd = keyboardInit(0,1,BgType_Text4bpp, BgSize_T_256x256, 5,3,false,true);
    kbd->OnKeyPressed = OnKeyPressed;

    #ifndef EMULATOR
    while(1) { //breaks when connected properly
        consoleSelect(&top_screen);
        consoleClear();
        BG_PALETTE[255] = RGB15(6,7,14);
        top_screen.cursorY = 9;
        iprintf("Select connection method\n\n");
        int pressed = 0;
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
