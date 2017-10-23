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
#include "mainmenu.h"

// Helper function for mainMenuLoop()
void printMainMenuLoopText() {
        consoleSelect(&bot_screen);
        consoleClear();
        iprintf("Controller Main Menu");
        bot_screen.cursorY = 7;
        bot_screen.cursorX = 1;
        iprintf("Lorem ");
        bot_screen.cursorX = 17;
        iprintf("Ipsum");
        bot_screen.cursorY = 15;
        bot_screen.cursorX = 1;
        iprintf("Dolor");
        bot_screen.cursorX = 17;
        iprintf("Amet");

        consoleSelect(&top_screen);
        consoleClear();
        top_screen.cursorY = 7;
        iprintf("Select connection method");
}

void mainMenuLoop() {
    int pressed = 0;
    touchPosition touch_loc;
    Box main_menu_boxes[6] = {  {8,122,42,90},
                                {136,248,42,90},
                                {8,122,106,154},
                                {136,248,106,154},
                                {8,122,172,188},
                                {136,248,172,188}};

    while(1) { //while not connected
        BG_PALETTE_SUB[255] = RGB15(6,7,14);
        BG_PALETTE[255] = RGB15(31,31,31);

        placeSelectionSprite(main_menu_boxes, 4);

        //populate menu with text
        printMainMenuLoopText();

        swiWaitForVBlank();
        scanKeys();
        pressed = keysDown();
        if(pressed&KEY_B) {
            selector.selection = -1;
        }
        if(pressed&KEY_A) {
            if(selector.selection == 1) {
                clearConsoles();
                hideSelectSprite();
                if(WFCConnect(&top_screen)) {
                    consoleClear();
                    continue;
                } else {
                    break;
                }
            } else if(!selector.selection){
                clearConsoles();
                hideSelectSprite();
                if(ManualConnect(&top_screen, &bot_screen)) {
                    consoleClear();
                    continue;
                } else {
                    break;
                }
            } else {
                selector.selection = 0;
            }
        }
        if(pressed&KEY_LEFT || pressed&KEY_RIGHT || pressed&KEY_UP || pressed&KEY_DOWN) {
            if(selector.selection < 0) {
                if(pressed&KEY_RIGHT) {
                    selector.selection = 1;
                } else {
                    selector.selection = 0;
                }
            } else {
                int addition = 1;
                if(pressed&KEY_LEFT) {
                    addition = 3;
                } else if(pressed&KEY_UP||pressed&KEY_DOWN) {
                    addition = 2;
                }
                selector.selection = (selector.selection + addition) % 4;
            }
        }
        if(pressed&KEY_TOUCH) {
            int box_click;

            touchRead(&touch_loc);
            box_click = inBox(4, main_menu_boxes, touch_loc);

            if(box_click == 0) {
                clearConsoles();
                hideSelectSprite();
                if(ManualConnect(&top_screen, &bot_screen)) {
                    consoleClear();
                    continue;
                } else {
                    break;
                }
            } else if (box_click == 1) {
                clearConsoles();
                hideSelectSprite();
                iprintf("Controller Connection Setup");
                if(WFCConnect(&top_screen)) {
                    consoleClear();
                    continue;
                } else {
                    break;
                }
            } else if (box_click == 3){
                if(selector.selection == 1) {
                    clearConsoles();
                    hideSelectSprite();
                    if(WFCConnect(&top_screen)) {
                        consoleClear();
                        continue;
                    } else {
                        break;
                    }
                } else if(!selector.selection){
                    clearConsoles();
                    hideSelectSprite();
                    iprintf("Controller Connection Setup");
                    if(ManualConnect(&top_screen, &bot_screen)) {
                        consoleClear();
                        continue;
                    } else {
                        break;
                    }
                } else {
                    selector.selection = 0;
                }
            } else if (box_click == 2) {
                selector.selection = -1;
            }
        }
        if (pressed & KEY_SELECT) { // TODO: REMOVE THIS DEBUG OPTION
            hideSelectSprite();
            clearConsoles();
            animScroll(bg_bot[2], OFFXTILE, OFFYTILE);
            loadMainMenu();
            animScroll(bg_bot[2], OFFXTILE, OFFYTILE+192);
            break;
        }
    } // while not connected

}
