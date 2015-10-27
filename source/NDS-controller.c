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


int main() {
    int pressed = 0;
    touchPosition touch_loc;
    Box main_menu_boxes[4] = {  {8,122,35,158},
                                {136,248,35,158},
                                {8,122,172,188},
                                {136,248,172,188}};
    videoInit();

#ifndef EMULATOR
    while(1) { //while not connected
        BG_PALETTE_SUB[255] = RGB15(6,7,14);
        BG_PALETTE[255] = RGB15(31,31,31);

        placeSelectionSprite(main_menu_boxes, 2);

        consoleSelect(&bot_screen);
        consoleClear();
        iprintf("Controller Connection Setup");
        bot_screen.cursorY = 9;
        bot_screen.cursorX = 0;
        iprintf(" Search for ");
        bot_screen.cursorX = 18;
        iprintf("Automatic \n\n");
        iprintf("Access Point");
        bot_screen.cursorX = 20;
        iprintf("(WFC)");

        consoleSelect(&top_screen);
        consoleClear();
        top_screen.cursorY = 7;
        iprintf("Select connection method");
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
        if(pressed&KEY_LEFT || pressed&KEY_RIGHT) {
            if(selector.selection < 0) {
                selector.selection = pressed&KEY_RIGHT;
            } else {
                selector.selection = !selector.selection;
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
    } // while not connected
#endif // EMULATOR

    clearConsoles();

    consoleSelect(&top_screen);
    iprintf("\nConnection successful! :)\n");

    do {
        scanKeys();
        pressed = keysDown();
        swiWaitForVBlank();
    } while (! (pressed&KEY_START));

    return 0;
}
