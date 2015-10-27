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
#ifndef SELECTSPRITE_H
#define SELECTSPRITE_H

#include <nds.h>

#include "selection_corner.h"

typedef struct {
    int left, right, top, bot;
} Box;

typedef struct {
    u16 *gfx_bar;
    SpriteSize size_bar;
    SpriteColorFormat format_bar;
    int rot_bar;
    int paletteAlpha_bar;
    int x_bar;
    int y_bar;
    int hidden_bar;

    u16 *gfx_cor[4];
    SpriteSize size_cor;
    SpriteColorFormat format_cor;
    int rot_cor[4];
    int paletteAlpha_cor;
    int x_cor[4];
    int y_cor[4];
    int hidden_cor;
    int selection;
    int prev_sel;
} BoxSelectionSprite;

BoxSelectionSprite selector;

void initSelectSprite();

void updateSelectSprite();

void hideSelectSprite();

void placeSelectionSprite(Box *boxes, int n_boxes);

#endif // SELECTSPRITE_H
