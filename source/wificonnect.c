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

    iprintf("\nLoading connection info \nfrom WFC storage \n\n(set-up using commercial rom)");
    iprintf("\n\nConnecting...");

    if(!Wifi_InitDefault(WFC_CONNECT)) {
        iprintf("Failed to connect!");
        return -1;
    } else {
        iprintf("Done\n");
        return 0;
    }
}

int ManualConnect(PrintConsole *top_screen, PrintConsole *bot_screen) {
    int status = ASSOCSTATUS_DISCONNECTED, wepmode = WEPMODE_NONE;
    char wepkey[64];

    consoleSelect(bot_screen);
    iprintf("\n\nScanning...");
    //consoleSelect(top_screen); TODO decide where to put it all
    consoleClear();

    Wifi_InitDefault(INIT_ONLY);
    Wifi_AccessPoint* ap = findAP();

    iprintf("Connecting to %s\n", ap->ssid);

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
                iprintf("Only 5-digit and 13-digit WEP keys are supported!\n");
                return -1;
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
        iprintf("Connecting. \n\nPress (B) to cancel");
        scanKeys();
        if(keysDown() & KEY_B) {
            break;
        }
        swiWaitForVBlank();
    }
    if(status == ASSOCSTATUS_ASSOCIATED) {
        return 0;
    } else {
        return -1;
    }
}

Wifi_AccessPoint* findAP() {
    int i,j=0,skip_n;
    int selected = 0;
    int *fWPA_buff, *bWPA_buff;
    int count = 0, displaytop = 0;
    int pressed = 0;
    static Wifi_AccessPoint ap;
    Wifi_ScanMode();
    do {
        scanKeys();
        pressed = keysDown();

        count = Wifi_GetNumAP();
        fWPA_buff = (int*) malloc(count*sizeof(int));
        bWPA_buff = (int*) malloc(count*sizeof(int));
        // check how many WPA connections there are at what places
        for(i = count-1, skip_n = 0, j=0; i >= 0; i--) {
            Wifi_AccessPoint tap;
            Wifi_GetAPData(i, &tap);
            if(tap.flags & WFLAG_APDATA_WPA) {
                skip_n++;
                j++; //track n WPA
                fWPA_buff[i] = skip_n;
            } else {
                skip_n = 0;
            }
        }
        for(i = 0, skip_n = 0; i < count; i++) {
            Wifi_AccessPoint tap;
            Wifi_GetAPData(i, &tap);
            if(tap.flags & WFLAG_APDATA_WPA) {
                skip_n++;
                bWPA_buff[i] = skip_n;
            } else {
                skip_n = 0;
            }
        }

        consoleClear();
        iprintf(" %d WEP/Open APs detected\n\n\n", count-j);
        int displayend = displaytop + 6;
        if (displayend > count) {
            displayend = count;
        }
        for(i = displaytop; i < displayend; i++) {
            Wifi_AccessPoint tap;
            Wifi_GetAPData(i, &tap);
            if(tap.flags & WFLAG_APDATA_WPA) {
                continue;
            } else {
                iprintf(" %s %.28s\n %s Sig:%i\n\n", i == selected ? "->" : "  ", tap.ssid,
                        tap.flags & WFLAG_APDATA_WEP ? "WEP  " : "Open ", tap.rssi * 100 / 0xD0);
            }
        }
        if(pressed & KEY_UP) {
            if(selected-1-bWPA_buff[selected -1]  >= 0) {
                selected -= 1+bWPA_buff[selected -1];
            }
            if(selected<displaytop) {
                displaytop = selected;
            }
        }
        if(pressed & KEY_DOWN) {
            if(selected+1+fWPA_buff[selected+1] < count) {
                selected += 1+fWPA_buff[selected+1];
            }
            displaytop = selected - 6;
            if (displaytop<0) {
                displaytop = 0;
            }
        }
        swiWaitForVBlank();
    } while(!(pressed & KEY_A));
    Wifi_GetAPData(selected, &ap);

    free(fWPA_buff);
    free(bWPA_buff);

    return &ap;
}
