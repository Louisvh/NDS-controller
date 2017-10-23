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
#include "wificonnect.h"

int WFCConnect(PrintConsole *console) {
    consoleSelect(console);
    consoleClear();

    animScroll(bg_bot[2], OFFXTILE, OFFYTILE);
    bgUpdate();

    iprintf("\nLoading connection info \nfrom WFC storage \n\n(set-up using commercial rom)");
    iprintf("\n\nConnecting...");

    if(!Wifi_InitDefault(WFC_CONNECT)) {
        iprintf("Failed to connect!");
        animScroll(bg_bot[2], OFFXTILE, OFFYTILE+192);
        bgUpdate();
        return -1;
    } else {
        clearConsoles();
        loadMainMenu();
        animScroll(bg_bot[2], OFFXTILE, OFFYTILE+192);
        return 0;
    }
}

int ManualConnect(PrintConsole *top_screen, PrintConsole *bot_screen) {
    int status = ASSOCSTATUS_DISCONNECTED, wepmode = WEPMODE_NONE;
    char wepkey[64];

    animScroll(bg_bot[2], OFFXTILE, OFFYTILE);
    bgUpdate();

    consoleSelect(top_screen);
    iprintf("\n\nScanning...");
    consoleSelect(bot_screen);

    Wifi_InitDefault(INIT_ONLY);
    Wifi_AccessPoint* ap = findAP(top_screen, bot_screen);
    consoleClear();
    swiWaitForVBlank();

    iprintf(" Connecting to %.12s\n", ap->ssid);
    bot_screen->cursorY = 7;

    Wifi_SetIP(0,0,0,0,0);

    if (ap->flags & WFLAG_APDATA_WEP) {
        iprintf("Enter Wep Key\n");
        while (wepmode == WEPMODE_NONE) {
            scanf("%s",wepkey);
            if (strlen(wepkey)==13) {
                wepmode = WEPMODE_128BIT;
            } else if (strlen(wepkey) == 5) {
                wepmode = WEPMODE_40BIT;
            } else {
                int i=keysCurrent();
                while(!(i&KEY_A || i&KEY_B)) {
                    i=keysCurrent();
                    consoleClear();
                    iprintf("Error\n");
                    bot_screen->cursorY = 7;
                    iprintf("Only 5-digit and 13-digit \nWEP keys are supported!");
                    swiWaitForVBlank();
                }
                clearConsoles();
                animScroll(bg_bot[2], OFFXTILE, OFFYTILE+192);
                return ManualConnect(top_screen, bot_screen); //try again
            }
        }
        Wifi_ConnectAP(ap, wepmode, 0, (u8*)wepkey);
    } else {
        Wifi_ConnectAP(ap, WEPMODE_NONE, 0, 0);
    }
    consoleClear();
    while(status != ASSOCSTATUS_ASSOCIATED && status != ASSOCSTATUS_CANNOTCONNECT) {
        status = Wifi_AssocStatus();
        consoleClear();
        iprintf("Connecting...");
        scanKeys();
        if(keysDown() & KEY_B) {
            break;
        }
        swiWaitForVBlank();
    }
    bgUpdate();
    if(status == ASSOCSTATUS_ASSOCIATED) {
        clearConsoles();
        loadMainMenu();
        animScroll(bg_bot[2], OFFXTILE, OFFYTILE+192);
        return 0;
    } else {
        animScroll(bg_bot[2], OFFXTILE, OFFYTILE+192);
        return -1;
    }
}

Wifi_AccessPoint* findAP(PrintConsole *top_screen, PrintConsole *bot_screen) {
    int i;
    int selected = 0;
    int count = 0, displaytop = 0, displayend;
    int pressed = 0;
    static Wifi_AccessPoint ap;
    Wifi_ScanMode();
    do {
        scanKeys();
        pressed = keysDown();

        count = Wifi_GetNumAP();
        consoleClear();
        iprintf("%d Available APs Detected\n", count);
        bot_screen->cursorY = 5;
        displayend = displaytop + 4;
        if (displayend > count) {
            displayend = count;
        }
        for(i = displaytop; i < displayend; i++) {
            Wifi_AccessPoint tap;
            Wifi_GetAPData(i, &tap);
            if(tap.flags & WFLAG_APDATA_WPA) {
                iprintf("%s %.26s\n    WPA Sig:%i\n\n", i == selected ? "-X" : "  ", tap.ssid,
                 tap.rssi * 100 / 0xD0);
            } else {
                iprintf("%s %.28s\n    %s Sig:%i\n\n", i == selected ? "->" : "  ", tap.ssid,
                        tap.flags & WFLAG_APDATA_WEP ? "WEP  " : "Open ", tap.rssi * 100 / 0xD0);
            }
        }
        if(pressed & KEY_UP) {
            if(selected > 0) {
                selected--;
            }
            if(selected<displaytop) {
                displaytop = selected;
            }
        }
        if(pressed & KEY_DOWN) {
            if(selected+1 < count) {
                selected ++;
            }
            displaytop = selected - 3;
            if (displaytop<0) {
                displaytop = 0;
            }
        }
        if(pressed & KEY_B) {
            return NULL;
        }
        swiWaitForVBlank();
        Wifi_GetAPData(selected, &ap);
    } while(!(pressed & KEY_A) || ap.flags & WFLAG_APDATA_WPA);

    return &ap;
}

// Helper function for setupConnection()
void printConnectionLoopText() {
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
}

void setupConnection() {
    int pressed = 0;
        touchPosition touch_loc;
        Box main_menu_boxes[4] = {  {8,122,35,158},
                                    {136,248,35,158},
                                    {8,122,172,188},
                                    {136,248,172,188}};

    while(1) { //while not connected
        BG_PALETTE_SUB[255] = RGB15(6,7,14);
        BG_PALETTE[255] = RGB15(31,31,31);

        placeSelectionSprite(main_menu_boxes, 2);

        //populate menu with text
        printConnectionLoopText();

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
                selector.selection = (pressed&KEY_RIGHT)!=0;
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
        if (pressed & KEY_SELECT) { // TODO: REMOVE THIS DEBUG OPTION
            hideSelectSprite();
            clearConsoles();
            animScroll(bg_bot[2], OFFXTILE, OFFYTILE);
            loadMainMenu();
            animScroll(bg_bot[2], OFFXTILE, OFFYTILE+192);
            break;
        }
    } // while not connected

    clearConsoles();

    consoleSelect(&top_screen);
    iprintf("\nConnection successful! :)\n");
}
