// ============================================================
// Clock UI — implementation
// ============================================================
#include "clock_ui.h"
#include <TJpg_Decoder.h>

// --- Extern dependencies defined in main.cpp ---
extern TFT_eSPI tft;
extern Number digitPainter;
extern PrinterState pr;

extern bool isPrintingState(const String& s);
extern bool isPreparingState(const String& s);
extern bool isPausedState(const String& s);
extern bool isFinishedState(const String& s);
extern bool isFailedState(const String& s);
extern uint16_t statusColor();

// --- State variables (defined here, declared extern in clock_ui.h) ---
bool clockMode = false;
bool clockBaseDrawn = false;

uint8_t prevHourTens = 100;
uint8_t prevHourOnes = 100;
uint8_t prevMinTens  = 100;
uint8_t prevMinOnes  = 100;
uint8_t prevSecTens  = 100;
uint8_t prevSecOnes  = 100;
uint16_t prevDayKey  = 0;

// ------------------------------------------------------------
// shouldShowClock — returns true when clock UI should be shown
// ------------------------------------------------------------
bool shouldShowClock() {
  if (isPrintingState(pr.status) || isPreparingState(pr.status) || isPausedState(pr.status)) return false;
  return true;
}

// ------------------------------------------------------------
// resetClockState — reset all trackers (call on mode switch)
// ------------------------------------------------------------
void resetClockState() {
  clockBaseDrawn = false;
  prevHourTens = 100; prevHourOnes = 100;
  prevMinTens  = 100; prevMinOnes  = 100;
  prevSecTens  = 100; prevSecOnes  = 100;
  prevDayKey   = 0;
}

// ------------------------------------------------------------
// renderClock — draw the clock UI (called every ~1s when in clock mode)
// ------------------------------------------------------------
void renderClock() {
  time_t now = time(nullptr);
  bool timeValid = (now >= 1700000000);
  struct tm* info = timeValid ? localtime(&now) : nullptr;

  tft.startWrite();

  if (!clockBaseDrawn) {
    // === Static part: drawn once per clock-mode entry ===
    tft.fillScreen(BG_BLACK);

    // Colon — two filled squares between hO and mT
    tft.fillRect(COLON_X, COLON_TOP_Y, COLON_SIZE, COLON_SIZE, C_TEXT);
    tft.fillRect(COLON_X, COLON_BOT_Y, COLON_SIZE, COLON_SIZE, C_TEXT);

    // Seconds progress bar — grey bg ticks + rounded border
    for (uint8_t i = 0; i < SB_TICKS; i++) {
      tft.fillRect(SB_X_START + i * SB_SLOT, SB_Y, SB_TICK_W, SB_H, SB_COLOR_BG);
    }
    tft.drawRoundRect(SB_BORDER_X, SB_BORDER_Y,
                      SB_BORDER_RECT_W, SB_BORDER_RECT_H,
                      SB_BORDER_R, SB_COLOR_BORDER);

    // Bottom status hint
    String hint;
    if (isFinishedState(pr.status)) hint = "PRINT DONE";
    else if (isFailedState(pr.status)) hint = "ERROR";
    else hint = "IDLE";
    tft.setTextDatum(MC_DATUM);
    tft.setTextPadding(0);
    tft.setTextFont(CLOCK_HINT_FONT);
    tft.setTextColor(statusColor(), BG_BLACK);
    tft.drawString(hint, 120, CLOCK_HINT_Y);

    // Reset per-digit trackers so first tick draws everything
    resetClockState();

    clockBaseDrawn = true;
  }

  // === Dynamic part: per-digit change detection ===
  if (timeValid && info) {
    uint8_t h = info->tm_hour;
    uint8_t m = info->tm_min;
    uint8_t s = info->tm_sec;

    // --- HOURS (white 36x60) ---
    uint8_t hT = h / 10, hO = h % 10;
    if (hT != prevHourTens) {
      tft.fillRect(HOUR_X_TENS, DIGIT_Y, DIGIT_W, DIGIT_H, BG_BLACK);
      digitPainter.printfW3660(HOUR_X_TENS, DIGIT_Y, hT);
      prevHourTens = hT;
    }
    if (hO != prevHourOnes) {
      tft.fillRect(HOUR_X_ONES, DIGIT_Y, DIGIT_W, DIGIT_H, BG_BLACK);
      digitPainter.printfW3660(HOUR_X_ONES, DIGIT_Y, hO);
      prevHourOnes = hO;
    }

    // --- MINUTES (orange 36x60) ---
    uint8_t mT = m / 10, mO = m % 10;
    if (mT != prevMinTens) {
      tft.fillRect(MIN_X_TENS, DIGIT_Y, DIGIT_W, DIGIT_H, BG_BLACK);
      digitPainter.printfO3660(MIN_X_TENS, DIGIT_Y, mT);
      prevMinTens = mT;
    }
    if (mO != prevMinOnes) {
      tft.fillRect(MIN_X_ONES, DIGIT_Y, DIGIT_W, DIGIT_H, BG_BLACK);
      digitPainter.printfO3660(MIN_X_ONES, DIGIT_Y, mO);
      prevMinOnes = mO;
    }

    // --- SECONDS progress bar (EVA-style segmented ticks) ---
    // Each tick represents 60/SB_TICKS seconds; paint ticks elapsed this minute
    {
      static uint8_t lastTicksDrawn = 100;
      uint8_t ticks = s * SB_TICKS / 60;
      if (ticks != lastTicksDrawn) {
        // Rolled over to 0 — repaint all bg first
        if (ticks < lastTicksDrawn) {
          for (uint8_t i = 0; i < SB_TICKS; i++) {
            tft.fillRect(SB_X_START + i * SB_SLOT, SB_Y, SB_TICK_W, SB_H, SB_COLOR_BG);
          }
          lastTicksDrawn = 0;
        }
        // Paint ticks from last drawn to current
        for (uint8_t i = lastTicksDrawn; i < ticks; i++) {
          tft.fillRect(SB_X_START + i * SB_SLOT, SB_Y, SB_TICK_W, SB_H, SB_COLOR_FILLED);
        }
        lastTicksDrawn = ticks;
      }
    }

    // --- DATE + WEEKDAY — changed once per day ---
    uint16_t dayKey = ((info->tm_mon + 1) << 8) | info->tm_mday;
    if (dayKey != prevDayKey) {
      tft.setTextDatum(MC_DATUM);
      tft.setTextPadding(0);
      tft.setTextFont(DATE_FONT);
      tft.fillRect(20, 150, 200, 32, BG_BLACK);

      const char* weekNames[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
      char dateBuf[32];
      snprintf(dateBuf, sizeof(dateBuf), "%02d-%02d  %s",
               info->tm_mon + 1, info->tm_mday,
               weekNames[info->tm_wday]);
      tft.setTextColor(C_DIM, BG_BLACK);
      tft.drawString(dateBuf, DATE_CX, DATE_Y);
      prevDayKey = dayKey;
    }
  } else {
    // NTP not yet synced
    tft.setTextDatum(MC_DATUM);
    tft.setTextPadding(0);
    tft.setTextFont(DATE_FONT);
    tft.setTextColor(C_DIM, BG_BLACK);
    tft.fillRect(60, 90, 120, 50, BG_BLACK);
    tft.drawString("Syncing...", 120, 115);
  }

  tft.endWrite();
}
