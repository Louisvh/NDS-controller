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
#include "selectsprite.h"

void updateSelectSprite() {
    int i;
    oamSet(
        &oamMain, //main display
        0, //oam entry to set
        selector.x_bar, selector.y_bar, //position
        0, //priority
        selector.paletteAlpha_bar,
        selector.size_bar,
        selector.format_bar,
        selector.gfx_bar,
        0, //rot index
        true, //double the size of rotated sprites
        selector.hidden_bar,
        false, false, //vflip, hflip
        false //apply mosaic
    );
    oamRotateScale(&oamMain, 0, 0, (1 << 7), (1<<9));

    for (i=0; i<4; i++) {
        oamSet(
            &oamMain, //main display
            i+2, //oam entry to set
            selector.x_cor[i], selector.y_cor[i], //position
            0, //priority
            selector.paletteAlpha_cor,
            selector.size_cor,
            selector.format_cor,
            selector.gfx_cor[0],
            i+1, //rot index
            false, //double the size of rotated sprites
            selector.hidden_cor,
            false,//hflip
            false, //vflip
            false //apply mosaic
        );
        oamRotateScale(&oamMain, i+1, degreesToAngle(-i*90), (1 << 8), (1<<8));
    }
    oamUpdate(&oamMain);
}

void initSelectSprite() {
    int i;

    oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);

    REG_BLDALPHA = (8) | (8<<8);
    REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_SPRITE | BLEND_DST_BG2 | BLEND_DST_BG3;

    selector.size_bar = SpriteSize_64x32;
    selector.format_bar = SpriteColorFormat_Bmp;
    selector.paletteAlpha_bar = 5;
    selector.rot_bar = 0;
    selector.gfx_bar = oamAllocateGfx(&oamMain, selector.size_bar, selector.format_bar);
    dmaFillHalfWords(ARGB16(1,31,31,31), selector.gfx_bar, 64*32*2);

    selector.size_cor = SpriteSize_16x16;
    selector.format_cor = SpriteColorFormat_Bmp;
    selector.paletteAlpha_cor = 15;
    for(i=0; i<4; i++) {
        selector.gfx_cor[i] = oamAllocateGfx(&oamMain, selector.size_cor, selector.format_cor);
        dmaCopy(selection_cornerBitmap, selector.gfx_cor[i], 16*16*2);
    }
}

void hideSelectSprite() {
    selector.selection = 0;
    selector.y_bar = 0;
    selector.hidden_bar = true;
    selector.hidden_cor = true;
    updateSelectSprite();
}

void placeSelectionSprite(Box *boxes, int n_boxes) {
    if(selector.selection == -1) {
        selector.hidden_bar = true;
        selector.hidden_cor = true;
        selector.prev_sel = selector.selection;
    } else if (selector.selection < n_boxes) {
        selector.hidden_bar = false;
        selector.hidden_cor = false;
        selector.x_bar = boxes[selector.selection].left-1;
        selector.y_bar = (selector.y_bar + 3) % 230;
        if (selector.prev_sel != selector.selection) {
            selector.prev_sel = selector.selection;
            selector.y_bar = 0;
        }
        if (selector.y_bar < boxes[selector.selection].top-32 ||
                selector.y_bar > boxes[selector.selection].bot-32) {
            selector.hidden_bar = true;
        }
        selector.x_cor[0] = boxes[selector.selection].left-3;
        selector.y_cor[0] = boxes[selector.selection].top-5;
        selector.x_cor[1] = boxes[selector.selection].right-13;
        selector.y_cor[1] = boxes[selector.selection].top-5;
        selector.x_cor[2] = boxes[selector.selection].right-13;
        selector.y_cor[2] = boxes[selector.selection].bot-13;
        selector.x_cor[3] = boxes[selector.selection].left-3;
        selector.y_cor[3] = boxes[selector.selection].bot-13;
    } else {
        selector.selection = -1;
    }
    updateSelectSprite();
}
