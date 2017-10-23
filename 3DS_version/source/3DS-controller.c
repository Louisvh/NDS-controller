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

#define PHONE_ADDR "192.168.234.177"
#define DEST_PORT 3210

static u32 *SOC_buffer = NULL;
s32 sock = -1, csock = -1;

__attribute__((format(printf,1,2)))
void failExit(const char *fmt, ...);

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

	memset (&server, 0, sizeof (server));

	server.sin_family = AF_INET;
	server.sin_port = htons (DEST_PORT);
    if (inet_aton(PHONE_ADDR, &server.sin_addr) == 0) {
        failExit("inet_aton() failed\n");
    }

	printf("Spamming %s:%d with UDP packets\n", PHONE_ADDR, DEST_PORT);
    
	fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);

	while (aptMainLoop()) {
        u32 kHeld, kDown, kUp;
        u32 sendBuf[2]= {0,0};
        u32 ID = 0; // TODO changeable playerID
        int i=0;

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
            printf("\x1b[4;1HkHeld: ");
            for(i=31; i>-1; i--){
                printf("%d", (kHeld & (1 << i)) != 0);
            }
        }

		if (kHeld & KEY_START && kHeld & KEY_SELECT && 
                kHeld & KEY_L && kHeld & KEY_R) break;

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
