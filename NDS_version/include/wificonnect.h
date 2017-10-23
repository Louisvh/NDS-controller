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

#ifndef WIFICONNECT_H
#define WIFICONNECT_H

#include <nds.h>
#include <stdio.h>
#include <time.h>

#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "videotools.h"

/**
 * Interactively lists WiFi connections, allow the user to select one and connect.
 * Supports open connections and WEP secured ones.
 */
int ManualConnect(PrintConsole *top_screen, PrintConsole *bot_screen);
/**
 * Connect using credentials in WFC bank.
 */
int WFCConnect(PrintConsole *console);

/**
 * Helper function for ManualConnect(~)
 */
Wifi_AccessPoint *findAP(PrintConsole *top_screen, PrintConsole *bot_screen);

/**
 * Full connection setup routine - part of the main program flow
 */
void setupConnection();

#endif //WIFICONNECT_H
