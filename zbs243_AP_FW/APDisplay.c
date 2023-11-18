#define __packed
#include "board.h"

#if (HAS_SCREEN == 1)
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "asmUtil.h"
#include "comms.h"
#include "cpu.h"
#include "printf.h"
#include "../oepl-definitions.h"
#include "../oepl-proto.h"
#include "radio.h"
#include "screen.h"
#include "timer.h"
#include "uart.h"
#include "wdt.h"

uint8_t updateCount = 1;
extern uint8_t __xdata curChannel;
extern uint8_t __xdata curPendingData;
extern uint8_t __xdata curNoUpdate;
extern uint8_t __xdata mSelfMac[8];

#if (SCREEN_WIDTH == 1)
// Segmented display type

bool __xdata showAPDisplay = true;
bool __xdata SisInverted = true;

void epdInitialize() {
    epdEnable();
    pr("Setting up EPD\n");
    if (!epdSetup(true)) pr("EPD setup failed\n");
    epdSetPos(0);
    epdpr("boot");
    setEPDIcon(EPD_ICON_DIAMOND, true);
    epdDraw();
    while (is_drawing()) {
    };
    timerDelay(1333);
}

void epdShowRun() {
    if(!showAPDisplay) return;
    epdClear();
    epdSetPos(0);
    epdpr("run");
    setEPDIcon(EPD_ICON_ARROW | EPD_SIGN_PENCE_SMALL, true);

    epdSetPos(4);
    epdpr("%d", curChannel);

    epdSetPos(6);
    epdpr("%d", curNoUpdate);

    epdSetPos(8);
    epdpr("%d", curPendingData);

    if (epdUpdate()) {
        updateCount++;
        if (updateCount == 10) {
            while (is_drawing()) {
            };
            epdSetup(false);
            SisInverted = false;
        } else if (updateCount == 20) {
            while (is_drawing()) {
            };
            epdSetup(true);
            SisInverted = true;
            updateCount = 0;
        }
    }
}

#endif

#if (SCREEN_WIDTH != 1)

void epdInitialize() {
    epdConfigGPIO(true);
    spiInit();
    epdSetup();
}

void epdShowRun() {
    // return;
    wdt60s();
    selectLUT(EPD_LUT_NO_REPEATS);
    clearScreen();
    setColorMode(EPD_MODE_NORMAL, EPD_MODE_INVERT);
#if (SCREEN_HEIGHT == 296)
    epdPrintBegin(56, 200, EPD_DIRECTION_Y, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
#else
    epdPrintBegin(16, 55, EPD_DIRECTION_X, EPD_SIZE_DOUBLE, EPD_COLOR_BLACK);
#endif
    epdpr("AP Mode");
    epdPrintEnd();

#if (SCREEN_HEIGHT == 296)
    epdPrintBegin(56, 100, EPD_DIRECTION_Y, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
#else
    epdPrintBegin(16, 35, EPD_DIRECTION_X, EPD_SIZE_SINGLE, EPD_COLOR_BLACK);
#endif
    epdpr(
        "%d:%d:%d:%d:%d:%d:%d:%d",
         mSelfMac[0], mSelfMac[1], mSelfMac[2], mSelfMac[3],
         mSelfMac[4], mSelfMac[5], mSelfMac[6], mSelfMac[7],
    );
    epdPrintEnd();
    draw();
}

#endif

#endif
