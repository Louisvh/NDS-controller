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

int inBox(int n_boxes, Box *box_arr, touchPosition touch) {
    int i;
    int px = touch.px, py = touch.py;
    for (i = 0; i<n_boxes; i++) {
        if (px > box_arr[i].left && px < box_arr[i].right
            && py > box_arr[i].top && py < box_arr[i].bot) {
            return i;
        }
    }
    return -1;
}

void animScrollBgAbs(int id, int new_x, int new_y, int frames, int allow_skip) {
    int prev_x = bg_scroll_table[id].x;
    int prev_y = bg_scroll_table[id].y;
    int diff_x = new_x - prev_x;
    int diff_y = new_y - prev_y;
    int frame = 0;
    int div = (frames*frames*frames);

    bg_scroll_table[id].x = new_x;
    bg_scroll_table[id].y = new_y;

    /*
     * Use cubic convergence in both dimensions.
     */
    do {
        int mult = (frames-frame)*(frames-frame)*(frames-frame);
        bgSetScroll(id, new_x - mult * diff_x / div,
                    new_y - mult * diff_y / div);
        frame++;
        bgUpdate();
        scanKeys();
        swiWaitForVBlank();
    } while(!(keysDown() && allow_skip) && frame < frames);

    bgSetScroll(id,new_x,new_y);
    bgUpdate();
    swiWaitForVBlank();
}

void animScroll(int id, int new_x, int new_y) {
    animScrollBgAbs(id, new_x, new_y, ANIM_FRAMES, 0);
}

void animScrollBgRel(int id, int x, int y, int frames, int allow_skip) {
    int cur_x = bgScrollTable[id]->x;
    int cur_y = bgScrollTable[id]->y;
    animScrollBgAbs(id, x+cur_x, y+cur_y, frames, allow_skip);
}

void clearConsoles() {
    consoleSelect(&top_screen);
    consoleClear();
    consoleSelect(&bot_screen);
    consoleClear();
}

void loadMainMenu() {
    dmaCopy(bg_bot22Tiles, bgGetGfxPtr(bg_bot[2]), bg_bot22TilesLen);
    dmaCopy(bg_bot22Map, bgGetMapPtr(bg_bot[2]), bg_bot22MapLen);
    vramSetBankE(VRAM_E_LCD);
    dmaCopy(bg_bot22Pal, &VRAM_E_EXT_PALETTE[2][0], bg_bot21PalLen);
    vramSetBankE(VRAM_E_BG_EXT_PALETTE);
}

void videoInit() {
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
    bg_bot[2] = bgInit (2, BgType_Text8bpp, BgSize_T_256x512, 22, 4);
    bg_bot[3] = bgInit (3, BgType_Text8bpp, BgSize_T_256x256, 5, 3);
    bg_top[3] = bgInitSub(3, BgType_Text8bpp, BgSize_T_256x256, 7, 1);

    // copy graphics to vram
    dmaCopy(bg_bot21Tiles, bgGetGfxPtr(bg_bot[2]), bg_bot21TilesLen);
    dmaCopy(bg_bot1Tiles, bgGetGfxPtr(bg_bot[3]), bg_bot1TilesLen);
    dmaCopy(bg_top1Tiles, bgGetGfxPtr(bg_top[3]), bg_top1TilesLen);

    // copy maps to vram
    dmaCopy(bg_bot21Map, bgGetMapPtr(bg_bot[2]), bg_bot21MapLen);
    dmaCopy(bg_bot1Map, bgGetMapPtr(bg_bot[3]), bg_bot1MapLen);
    dmaCopy(bg_top1Map, bgGetMapPtr(bg_top[3]), bg_top1MapLen);

    // access extended palettes in LCD mode
    vramSetBankE(VRAM_E_LCD); // main engine
    vramSetBankH(VRAM_H_LCD); // sub engine

    // copy palettes to extended palette area
    dmaCopy(bg_bot21Pal, &VRAM_E_EXT_PALETTE[2][0], bg_bot21PalLen);
    dmaCopy(bg_bot1Pal, &VRAM_E_EXT_PALETTE[3][0], bg_bot1PalLen);
    // same for sub engine
    dmaCopy(bg_top1Pal, &VRAM_H_EXT_PALETTE[3][0], bg_top1PalLen);

    // map vram banks to extended palettes
    vramSetBankE(VRAM_E_BG_EXT_PALETTE);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    consoleInit(&top_screen, 0,BgType_Text4bpp, BgSize_T_256x256, 4, 0, false, true);
    consoleInit(&bot_screen, 0,BgType_Text4bpp, BgSize_T_256x256, 4, 0, true, true);
    consoleSetWindow(&bot_screen, 2,1,28,24);
    consoleSetWindow(&top_screen, 3,13,25,10);

    // init the selection sprite
    vramSetBankF(VRAM_F_MAIN_SPRITE);
    initSelectSprite();

    //todo make some sort of keypad that fits in
    kbd = keyboardInit(kbd,1,BgType_Text4bpp, BgSize_T_256x256, 2,1,true,true);
    kbd->OnKeyPressed = OnKeyPressed;

    bgSetScroll(bg_bot[2],0,OFFYTILE+192);
    bg_scroll_table[bg_bot[2]].x = 0;
    bg_scroll_table[bg_bot[2]].y = OFFYTILE+192;
    bgUpdate();
    swiWaitForVBlank();
    animScrollBgAbs(bg_bot[2],OFFXTILE, OFFYTILE+192, 60, 1);
}
