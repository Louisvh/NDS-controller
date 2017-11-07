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
#include "controller.h"

char dest_addr[16] = "192.168.001.001";

static u32 *SOC_buffer = NULL;
s32 sock = -1, csock = -1;

u32 revEndian(u32 number) {
    u32 b0, b1, b2, b3;

    b0 = (number & 0x000000ff) << 24u;
    b1 = (number & 0x0000ff00) << 8u;
    b2 = (number & 0x00ff0000) >> 8u;
    b3 = (number & 0xff000000) >> 24u;
    return (b0 | b1 | b2 | b3);
}

//---------------------------------------------------------------------------------
void setIP() {
//---------------------------------------------------------------------------------
    int charsel=0, i=0, invalid=0;

    consoleClear();
	while (1) {
        u32 kHeld, kDown;

        consoleSelect(&bot_screen);
        consoleClear();
        consoleSelect(&top_screen);
        consoleClear();
        top_screen.cursorY = 6;
        iprintf("Use d-pad to change IP\n");
        iprintf("Press X to confirm\n");

        scanKeys();

        kDown = keysDown();
		kHeld = keysHeld();

        if ( kDown & KEY_RIGHT) {
            charsel = (charsel+1)%15;
            if(dest_addr[charsel]=='.')
                charsel = (charsel+1)%15;
            consoleClear();
        }
        if ( kDown & KEY_LEFT) {
            charsel = (charsel-1+15)%15;
            if(dest_addr[charsel]=='.')
                charsel = (charsel-1+15)%15;
            consoleClear();
        }
        if ( kDown & KEY_UP) {
            switch(charsel%4) {
                case 1:
                    if(dest_addr[charsel-1] > '1') {
                        dest_addr[charsel] = ((dest_addr[charsel]-'0'+1)%6)+'0';
                    } else {
                        dest_addr[charsel] = ((dest_addr[charsel]-'0'+1)%10)+'0';
                    }
                    break;
                case 2:
                    if(dest_addr[charsel-2] > '1' && dest_addr[charsel-1] > '4') {
                        dest_addr[charsel] = ((dest_addr[charsel]-'0'+1)%6)+'0';
                    } else {
                        dest_addr[charsel] = ((dest_addr[charsel]-'0'+1)%10)+'0';
                    }
                    break;
                default:
                    dest_addr[charsel] = ((dest_addr[charsel]-'0'+1)%3)+'0';
            }
            consoleClear();
        }
        if ( kDown & KEY_DOWN) {
            switch(charsel%4) {
                case 1:
                    if(dest_addr[charsel-1] > '1') {
                        dest_addr[charsel] = ((dest_addr[charsel]-'0'-1+6)%6)+'0';
                    } else {
                        dest_addr[charsel] = ((dest_addr[charsel]-'0'-1+10)%10)+'0';
                    }
                    break;
                case 2:
                    if(dest_addr[charsel-2] > '1' && dest_addr[charsel-1] > '4') {
                        dest_addr[charsel] = ((dest_addr[charsel]-'0'-1+6)%6)+'0';
                    } else {
                        dest_addr[charsel] = ((dest_addr[charsel]-'0'-1+10)%10)+'0';
                    }
                    break;
                default:
                    dest_addr[charsel] = ((dest_addr[charsel]-'0'-1+3)%3)+'0';
            }
            consoleClear();
        }

        //check validity
        invalid=0;
        for(i=0; i<16; i++) {
            switch(i%4) {
                case 1:
                    if(dest_addr[i-1] > '1' && dest_addr[i] > '5') {
                        printf("\x1b[31m");
                        i=16;
                        invalid=1;
                        break;
                    }
                    break;
                case 2:
                    if(dest_addr[i-2] > '1' && 
                            dest_addr[i-1] > '4' && dest_addr[i] > '5') {
                        printf("\x1b[31m");
                        i=16;
                        invalid=1;
                        break;
                    }
                    break;
                default:
                    break;
            }
        }

        top_screen.cursorY = 4;
        iprintf("IP: %s\x1b[0m", dest_addr);
        iprintf("\x1b[5;%dH^", 4+charsel);



		if (kHeld & KEY_X && !invalid) {
            consoleClear();
            return;
        }

        swiWaitForVBlank();
	}
}

void mainControlLoop(int player) {
    int pressed = 0;
    touchPosition touch_loc;
    Box control_menu_boxes[2] = {  {8,122,172,188},
                                {136,248,172,188}};

	u32	slen;
	struct sockaddr_in server;

    animScroll(bg_bot[2], OFFXTILE, OFFYTILE);

	// allocate buffer for SOC service
	SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

	if(SOC_buffer == NULL) {
		iprintf("ERROR: memalign: failed to allocate\n");
	}

	// Now intialise soc:u service
	//if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0) {
    //	printf("ERROR: socInit: 0x%08X\n", (unsigned int)ret);
	//}
	slen = sizeof(server);
	sock = socket (AF_INET, SOCK_DGRAM, 0);

	if (sock < 0) {
		iprintf("ERROR: socket: %d %s\n", errno, strerror(errno));
	}

    // ask for and link to the destination IP
    setIP();

	memset (&server, 0, sizeof (server));

	server.sin_family = AF_INET;
	server.sin_port = htons (DEST_PORT);
    if (inet_aton(dest_addr, &server.sin_addr) == 0) {
        iprintf("ERROR: inet_aton() failed\n");
    }

    while (1) {
        u32 kHeld, kDown, kUp;
        u32 sendBuf[2]= {0,0};
        u32 ID = player; 
        int i=0, inactive=0;

        iprintf("\x1b[1;1HIP: %s:%d", dest_addr, DEST_PORT);
        consoleSelect(&bot_screen);
        iprintf("\x1b[4;1HPress Start+Select+down \n to change IP address");
        iprintf("\x1b[8;1HPress Start+Select+L+R \n to quit");
        consoleSelect(&top_screen);
    
        scanKeys();

        kDown = keysDown();
        kUp = keysUp();
		kHeld = keysHeld();

        if ( kDown || kUp || inactive < REDUNDANT_PACKETS) {
            if (kDown || kUp)
                inactive=0;
            else
                inactive++;

            // We're sending from big to little endian architectures
            sendBuf[0] = revEndian(kHeld);
            sendBuf[1] = revEndian(ID);
            if (sendto(sock, &sendBuf, 8 , 0 , (struct sockaddr *) &server, slen)==-1)
            {
                iprintf("ERROR: sendto()\n");
            }
            iprintf("\x1b[5;1HStatus: ");
            for(i=12; i>-1; i--){
                printf("%d", (kHeld & (1 << i)) != 0);
            }
        }

		if (kHeld & KEY_START && kHeld & KEY_SELECT && 
                kHeld & KEY_L && kHeld & KEY_R) {
            break;
        }
		if (kHeld & KEY_START && kHeld & KEY_SELECT && kHeld & KEY_DOWN ) {
            setIP();
        }
        if(pressed&KEY_TOUCH) {
            int box_click;

            touchRead(&touch_loc);
            box_click = inBox(4, control_menu_boxes, touch_loc);

            switch(box_click) {
                case 0:
                case 1:
                    setIP();
                    break;
            }
        }
    }
}
