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
    oamSet(
        &oamMain, //main display
        0, //oam entry to set
        select_sprite.x_bar, select_sprite.y_bar, //position
        0, //priority
        select_sprite.paletteAlpha_bar,
        select_sprite.size_bar,
        select_sprite.format_bar,
        select_sprite.gfx_bar,
        select_sprite.rot_bar,
        true, //double the size of rotated sprites
        select_sprite.hidden_bar,
        false, false, //vflip, hflip
        false //apply mosaic
    );
    oamRotateScale(&oamMain, 0, 0, (1 << 7), (1<<9));
    oamUpdate(&oamMain);
}

void initSelectSprite() {
    oamInit(&oamMain, SpriteMapping_1D_128, false);

    REG_BLDALPHA = (8) | (8<<8);
    REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_SPRITE | BLEND_DST_BG2 | BLEND_DST_BG3;

    select_sprite.size_bar = SpriteSize_64x32;
    select_sprite.format_bar = SpriteColorFormat_Bmp;
    select_sprite.paletteAlpha_bar = 5;
    select_sprite.rot_bar = 00;
    select_sprite.gfx_bar = oamAllocateGfx(&oamMain, select_sprite.size_bar, select_sprite.format_bar);
    dmaFillHalfWords(ARGB16(1,31,31,31), select_sprite.gfx_bar, 64*32*2);

    select_sprite.size_cor = SpriteSize_16x16;
    select_sprite.format_cor = SpriteColorFormat_Bmp;
    select_sprite.paletteAlpha_cor = 15;
    select_sprite.rot_cor[0] = 0;
    select_sprite.gfx_cor = oamAllocateGfx(&oamMain, select_sprite.size_cor, select_sprite.format_cor);
    dmaFillHalfWords(ARGB16(1,31,31,31), select_sprite.gfx_cor, 64*32*2);
}

void hideSelectSprite() {
    select_sprite.y_bar = 0;
    select_sprite.hidden_bar = true;
    select_sprite.hidden_cor = true;
    updateSelectSprite();
}
