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
 */
 #include "videotools.h"


void OnKeyPressed(int key) {
    if(key > 0) {
        iprintf("%c", key);
    }
}

void videoInit() {
    int frames=0;

    // initialize video
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    // enable extended palettes
    bgExtPaletteEnable();
    bgExtPaletteEnableSub();
    lcdSwap(); //put main engine on bottom

    // initialize backgrounds
    bg_bot[2] = bgInit (2, BgType_Text8bpp, BgSize_T_256x256, 6, 3);
    bg_bot[3] = bgInit (3, BgType_Text8bpp, BgSize_T_256x256, 5, 4);
    bg_top[3] = bgInitSub(3, BgType_Text8bpp, BgSize_T_256x256, 7, 1);

    // copy graphics to vram
    dmaCopy(bg_bot2Tiles, bgGetGfxPtr(bg_bot[2]), bg_bot2TilesLen);
    dmaCopy(bg_bot1Tiles, bgGetGfxPtr(bg_bot[3]), bg_bot1TilesLen);
    dmaCopy(bg_top1Tiles, bgGetGfxPtr(bg_top[3]), bg_top1TilesLen);

    // copy maps to vram
    dmaCopy(bg_bot2Map, bgGetMapPtr(bg_bot[2]), bg_bot2MapLen);
    dmaCopy(bg_bot1Map, bgGetMapPtr(bg_bot[3]), bg_bot1MapLen);
    dmaCopy(bg_top1Map, bgGetMapPtr(bg_top[3]), bg_top1MapLen);

    // access extended palettes in LCD mode
    vramSetBankE(VRAM_E_LCD); // main engine
    vramSetBankH(VRAM_H_LCD); // sub engine

    // copy palettes to extended palette area
    dmaCopy(bg_bot2Pal, &VRAM_E_EXT_PALETTE[2][0], bg_bot2PalLen);  // bg 0, slot 0
    dmaCopy(bg_bot1Pal, &VRAM_E_EXT_PALETTE[3][0], bg_bot1PalLen); // bg 1, slot 12
    // same for sub engine
    dmaCopy(bg_top1Pal, &VRAM_H_EXT_PALETTE[3][0], bg_top1PalLen); // bg 1, slot 12

    // map vram banks to extended palettes
    vramSetBankE(VRAM_E_BG_EXT_PALETTE);     // for main engine
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE); // for sub engine

    do {
        swiWaitForVBlank();
        frames++;
        bgUpdate();
        scanKeys();

        // let's make it obvious there are multiple backgrounds
        bgSetScroll(bg_bot[2], frames*2, 0);
    } while(!keysDown() && frames < 128);
    bgSetScroll(bg_bot[2], 0, 0);
    bgUpdate();
    swiWaitForVBlank();

    consoleInit(&top_screen, 0,BgType_Text4bpp, BgSize_T_256x256, 4, 0, false, true);
    consoleInit(&bot_screen, 0,BgType_Text4bpp, BgSize_T_256x256, 4, 0, true, true);
    consoleSetWindow(&bot_screen, 1,1,30,24);
    consoleSetWindow(&top_screen, 3,13,25,10);

    //todo make some sort of keypad that fits in
    kbd = keyboardInit(kbd,1,BgType_Text4bpp, BgSize_T_256x256, 2,1,true,true);
    kbd->OnKeyPressed = OnKeyPressed;
}
