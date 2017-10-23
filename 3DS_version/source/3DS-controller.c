#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

#include <fcntl.h>

#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <3ds.h>

#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

#define DEST_PORT 3210

char dest_addr[16] = "192.168.001.001";

static u32 *SOC_buffer = NULL;
s32 sock = -1, csock = -1;

__attribute__((format(printf,1,2)))
void failExit(const char *fmt, ...);

//---------------------------------------------------------------------------------
void setIP() {
//---------------------------------------------------------------------------------
    int charsel=0;

    consoleClear();
	while (aptMainLoop()) {
        u32 kHeld, kDown, kUp;
        int i=0;

        printf("\x1b[1;1HUse d-pad to change address\n");
        printf("Press the X button to confirm\n");

		hidScanInput();

        kDown = hidKeysDown();
        kUp = hidKeysUp();
		kHeld = hidKeysHeld();

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
            if(charsel%4)
                dest_addr[charsel] = ((dest_addr[charsel]-'0'+1)%10)+'0';
            else
                dest_addr[charsel] = ((dest_addr[charsel]-'0'+1)%3)+'0';
            consoleClear();
        }
        if ( kDown & KEY_DOWN) {
            if(charsel%4)
                dest_addr[charsel] = ((dest_addr[charsel]-'0'-1+10)%10)+'0';
            else
                dest_addr[charsel] = ((dest_addr[charsel]-'0'-1+3)%3)+'0';
            consoleClear();
        }

        printf("\x1b[6;1HIP: %s", dest_addr);
        printf("\x1b[7;%dH^", 5+charsel);


		if (kHeld & KEY_X) {
            consoleClear();
            return;
        }

		gspWaitForVBlank();
	}

}

//---------------------------------------------------------------------------------
void socShutdown() {
//---------------------------------------------------------------------------------
	printf("waiting for socExit...\n");
	socExit();
}

//---------------------------------------------------------------------------------
u32 revEndian(u32 number) {
//---------------------------------------------------------------------------------
    u32 b0, b1, b2, b3;

    b0 = (number & 0x000000ff) << 24u;
    b1 = (number & 0x0000ff00) << 8u;
    b2 = (number & 0x00ff0000) >> 8u;
    b3 = (number & 0xff000000) >> 24u;
    return (b0 | b1 | b2 | b3);
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------
	int ret;


	u32	slen;
	struct sockaddr_in server;

	gfxInitDefault();
	atexit(gfxExit);

	consoleInit(GFX_TOP, NULL);

	// allocate buffer for SOC service
	SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

	if(SOC_buffer == NULL) {
		failExit("memalign: failed to allocate\n");
	}

	// Now intialise soc:u service
	if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0) {
    	failExit("socInit: 0x%08X\n", (unsigned int)ret);
	}

	// register socShutdown to run at exit
	// atexit functions execute in reverse order so this runs before gfxExit
	atexit(socShutdown);

	// libctru provides BSD sockets so most code from here is standard
	slen = sizeof(server);

	sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock < 0) {
		failExit("socket: %d %s\n", errno, strerror(errno));
	}

    // ask for and link to the destination IP
    setIP();

	memset (&server, 0, sizeof (server));

	server.sin_family = AF_INET;
	server.sin_port = htons (DEST_PORT);
    if (inet_aton(dest_addr, &server.sin_addr) == 0) {
        failExit("inet_aton() failed\n");
    }

	fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);

	while (aptMainLoop()) {
        u32 kHeld, kDown, kUp;
        u32 sendBuf[2]= {0,0};
        u32 ID = 0; // TODO changeable playerID
        int i=0;

        printf("\x1b[1;1HSending to %s:%d", dest_addr, DEST_PORT);
        printf("\x1b[8;1HPress Start+Select+down to change IP address");
        printf("\x1b[18;1HPress Start+Select+L+R to quit");
    
		hidScanInput();

        kDown = hidKeysDown();
        kUp = hidKeysUp();
		kHeld = hidKeysHeld();

        if ( kDown || kUp ) {
            // We're sending from big to little endian architectures
            sendBuf[0] = revEndian(kHeld);
            sendBuf[1] = revEndian(ID);
            if (sendto(sock, &sendBuf, 8 , 0 , (struct sockaddr *) &server, slen)==-1)
            {
                failExit("sendto()\n");
            }
            printf("\x1b[4;1HStatus: ");
            for(i=31; i>-1; i--){
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

		gspWaitForVBlank();
	}

	close(sock);

	return 0;
}

//---------------------------------------------------------------------------------
void failExit(const char *fmt, ...) {
//---------------------------------------------------------------------------------

	if(sock>0) close(sock);
	if(csock>0) close(csock);

	va_list ap;

	printf(CONSOLE_RED);
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	printf(CONSOLE_RESET);
	printf("\nPress B to exit\n");

	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kHeld = hidKeysHeld();
		if (kHeld & KEY_B) exit(0);
	}
}
