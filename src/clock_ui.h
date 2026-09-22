// ============================================================
// Clock UI — renderClock(), shouldShowClock() and all clock layout macros
// ============================================================
#ifndef CLOCK_UI_H
#define CLOCK_UI_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <time.h>
#include "config.h"
#include "font_number.h"

// --- Printer state (shared across all modules) ---
struct PrinterState {
  float progress = -1;
  float nozzleTemp = -1;
  float leftNozzleTemp = -1;
  float rightNozzleTemp = -1;
  float bedTemp = -1;
  float chamberTemp = -1;
  int remainingMin = -1;
  int currentLayer = -1;
  int totalLayers = -1;
  String status = "prepare";
  String displayName = "";
  String model = "";
  String serial = "";
  bool online = true;
  bool dualNozzle = false;
};

// --- Clock seconds progress bar (EVA style) ---
#define SB_X_START   30
#define SB_Y          115
#define SB_H           10
#define SB_TICK_W      3
#define SB_GAP         3
#define SB_SLOT        (SB_TICK_W + SB_GAP)
#define SB_TICKS      30
#define SB_SECONDS_PER_TICK  (60 / SB_TICKS)
#define SB_COLOR_FILLED  0xE650
#define SB_COLOR_BG      C_DIM
#define SB_COLOR_BORDER  C_DIM
#define SB_BORDER_THK    1
#define SB_PAD           8
#define SB_WIDTH         (SB_TICKS * SB_SLOT - SB_GAP)
#define SB_BORDER_X      (SB_X_START - SB_PAD)
#define SB_BORDER_Y      (SB_Y - SB_PAD)
#define SB_BORDER_RECT_W (SB_WIDTH + SB_PAD * 2)
#define SB_BORDER_RECT_H (SB_H + SB_PAD * 2)
#define SB_BORDER_R      4

// --- Clock time digits (W3660/O3660: 36x60 bitmap digits) ---
#define DIGIT_W        36
#define DIGIT_H        60
#define DIGIT_Y         27
#define HOUR_X_TENS     20
#define HOUR_X_ONES     60
#define MIN_X_TENS      144
#define MIN_X_ONES      184
#define COLON_X         117
#define COLON_SIZE      6
#define COLON_TOP_Y     43
#define COLON_BOT_Y     63
#define COLON_GAP       20

// --- Clock date + weekday area ---
#define DATE_Y          166
#define DATE_CX         120     // horizontal center
#define DATE_FONT       4       // Font 4 = 26px

// --- Clock bottom hint area ---
#define CLOCK_HINT_Y    210
#define CLOCK_HINT_FONT 2

// --- Extern state (shared with main.cpp) ---
extern bool clockMode;
extern bool clockBaseDrawn;

// --- Per-digit change detection trackers ---
extern uint8_t prevHourTens;
extern uint8_t prevHourOnes;
extern uint8_t prevMinTens;
extern uint8_t prevMinOnes;
extern uint8_t prevSecTens;
extern uint8_t prevSecOnes;
extern uint16_t prevDayKey;

// --- Core functions ---
void renderClock();
bool shouldShowClock();
void resetClockState();

#endif // CLOCK_UI_H
