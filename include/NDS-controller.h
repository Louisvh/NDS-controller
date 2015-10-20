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

#define EMULATOR 1 //TODO automate flipping this in the makefile

/**
 * Interactively lists WiFi connections, allow the user to select one and connect.
 * Supports open connections and WEP secured ones.
 */
int ManualConnect(PrintConsole *top_screen, PrintConsole *bot_screen);
/**
 * Connect using credentials in WFC bank.
 * Writes output to supplied terminal, acts silently if none is provided.
 */
int WFCConnect(PrintConsole *console);


/**
 * Standard keyboard callback placeholder
 */
void OnKeyPressed(int key);

/**
 * Entry point of the application
 */
int main();
