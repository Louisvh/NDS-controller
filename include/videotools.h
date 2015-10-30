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
#ifndef VIDEOTOOLS_H
#define VIDEOTOOLS_H

#include <nds.h>
#include <stdio.h>
#include "selectsprite.h"

#include "bg_top1.h"
#include "bg_bot1.h"
#include "bg_bot21.h"
#include "bg_bot22.h"

#define OFFXTILE 2*256
#define OFFYTILE 2*256

#define ANIM_FRAMES 50

PrintConsole top_screen, bot_screen;
Keyboard *kbd;
int bg_bot[4], bg_top[4];
bg_scroll bg_scroll_table[8];

void clearConsoles();

/**
 * Takes a touch position and four arrays of l,r,t,b bounds as input
 * Returns the index of clicked box (index of bounds array) or -1
 * Boxes assumed to not overlap
 */
int inBox(int n_boxes, Box *box_arr, touchPosition touch);

/**
 * Standard keyboard callback placeholder
 */
void OnKeyPressed(int key);

/**
 * Initialize the backgrounds and consoles
 */
void videoInit();

void loadMainMenu();

/**
 * Scroll one layer to (x,y) in specified n frames
 * If allow_skip is 1, the animation is skipped on keysDown()
 */
void animScrollBgAbs(int id, int new_x, int new_y, int frames, int allow_skip);

/**
 * animScrollBgAbs with frames = ANIM_FRAMES and allow_skip = 0
 */
void animScroll(int id, int new_x, int new_y);

/**
 * Scroll one layer to (x_cur+x,y_cur+y) in specified n frames
 * If allow_skip is 1, the animation is skipped on keysDown()
 */
void animScrollBgRel(int id, int x, int y, int frames, int allow_skip);

#endif //VIDEOTOOLS_H
