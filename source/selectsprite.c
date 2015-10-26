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
        select_sprite.x_bar, select_sprite.y_bar, //position
        0, //priority
        select_sprite.paletteAlpha_bar,
        select_sprite.size_bar,
        select_sprite.format_bar,
        select_sprite.gfx_bar,
        0, //rot
        true, //double the size of rotated sprites
        select_sprite.hidden_bar,
        false, false, //vflip, hflip
        false //apply mosaic
    );
    oamRotateScale(&oamMain, 0, 0, (1 << 8), (1<<8));

    for (i=0; i<4; i++) {
        oamSet(
            &oamMain, //main display
            i+1, //oam entry to set
            50*i+select_sprite.x_cor[i], 50*i+select_sprite.y_cor[i], //position
            0, //priority
            select_sprite.paletteAlpha_cor,
            select_sprite.size_cor,
            select_sprite.format_cor,
            select_sprite.gfx_cor,
            i*90, //rot
            false, //double the size of rotated sprites
            select_sprite.hidden_cor,
            false,//hflip
            false, //vflip
            false //apply mosaic
        );
    }

    oamUpdate(&oamMain);
}

void initSelectSprite() {
    int i;

    oamInit(&oamMain, SpriteMapping_1D_128, false);

    REG_BLDALPHA = (8) | (8<<8);
    REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_SPRITE | BLEND_DST_BG2 | BLEND_DST_BG3;

    select_sprite.size_bar = SpriteSize_64x32;
    select_sprite.format_bar = SpriteColorFormat_256Color;
    select_sprite.paletteAlpha_bar = 15;
    select_sprite.rot_bar = 0;
    select_sprite.gfx_bar = oamAllocateGfx(&oamMain, select_sprite.size_bar, select_sprite.format_bar);
    dmaFillHalfWords(ARGB16(1,31,31,31), select_sprite.gfx_bar, 64*32*2);

    select_sprite.size_cor = SpriteSize_16x16;
    select_sprite.format_cor = SpriteColorFormat_256Color;
    select_sprite.paletteAlpha_cor = 15;
    for(i=0; i<4; i++) {
        select_sprite.rot_cor[i] = i*90;
    }
    select_sprite.gfx_cor = oamAllocateGfx(&oamMain, select_sprite.size_cor, select_sprite.format_cor);
    dmaCopy(selection_cornerTiles, select_sprite.gfx_cor, 16*16);
    dmaCopy(selection_cornerPal, SPRITE_PALETTE, 512);

    //debug
    dmaCopy(selection_cornerTiles, select_sprite.gfx_bar, 16*16);
}

void hideSelectSprite() {
    select_sprite.y_bar = 0;
    select_sprite.hidden_bar = true;
    select_sprite.hidden_cor = true;
    updateSelectSprite();
}
