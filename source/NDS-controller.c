/*
 * TODO add copyright notice and GPL licence thingey
 *
 *
 *
 */
#include <nds.h>
#include <stdio.h>
#include <time.h>

#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define EMULATOR 1 //TODO zero this before deployment. Alt TODO: make dependent on build target

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

/*
 * Show a list of WiFi connections, allow the user to select one and connect.
 * Supports open connections and WEP secured ones.
 */
int ManualConnect(PrintConsole *console) {
    return -1;
}

void OnKeyPressed(int key) {
    if(key > 0) {
        iprintf("%c", key);
    }
}

int main() {
    PrintConsole top_screen;
    PrintConsole bot_screen;
    Keyboard *kbd;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&top_screen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bot_screen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
    consoleSetWindow(&bot_screen, 1,1,31,13); // Don't overwrite keyboard, small frame

    kbd = keyboardInit(0,1,BgType_Text4bpp, BgSize_T_256x256, 3,1,false,true);
    kbd->OnKeyPressed = OnKeyPressed;

    consoleSelect(&bot_screen);

    iprintf("\nPress (A) to connect using WFC \n(Set-up using commercial game)\n");
    iprintf("\nPress (B) to connect manually \n");
    while(1) {
        int pressed = 0;
        swiWaitForVBlank();
        scanKeys();
        pressed = keysDown();
        if(pressed&KEY_A) {
            iprintf("PRESSED (A)\n");
            if(WFCConnect(&bot_screen)) {
                consoleClear();
                iprintf("Failed to connect. Retrying...\n");
                iprintf("\n\nPress (A) to connect using WFC \n(Set-up using commercial game)\n");
                iprintf("\nPress (B) to connect manually \n");
            } else {
                break;
            }
        }
        if(pressed&(KEY_B)) {
            iprintf("PRESSED (B)\n");
            if(ManualConnect(&bot_screen)) {
                consoleClear();
                iprintf("Failed to connect. Retrying...\n");
                iprintf("\n\nPress (A) to connect using WFC \n(Set-up using commercial game)\n");
                iprintf("\nPress (B) to connect manually \n");
            } else {
                break;
            }
        }
    }

    while(1) {
        swiWaitForVBlank();
    }

    return 0;
}
