#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "trioe_image.h"
#include "index.h"
#include <math.h>

#define BUTTON_PIN 18
#define START_BUTTON 19
#define RESTART_BUTTON 23
#define BUZZER 26
#define OLED_WIDTH 128

U8G2_SSD1327_MIDAS_128X128_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

unsigned long lastBlinkTime = 0;
bool isBlinking = false;

// BUZZER 
void softBeep(int duration) {
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(500);
    digitalWrite(BUZZER, LOW);
    delayMicroseconds(4000);
  }
}

// ROBOT FACE 
void drawHead() {
  u8g2.drawRFrame(10, 30, 108, 70, 12);
}

void drawEyes(bool blink) {
  int leftEyeX  = 44;
  int rightEyeX = 84;
  int eyeY      = 55;

  if (blink) {
    u8g2.drawBox(leftEyeX - 14, eyeY, 28, 4);
    u8g2.drawBox(rightEyeX - 14, eyeY, 28, 4);
  } else {
    u8g2.drawDisc(leftEyeX, eyeY, 14);
    u8g2.drawDisc(rightEyeX, eyeY, 14);

    u8g2.setDrawColor(0);
    u8g2.drawDisc(leftEyeX, eyeY, 5);
    u8g2.drawDisc(rightEyeX, eyeY, 5);
    u8g2.setDrawColor(1);
  }
}

void drawMouth() {
  int centerX = 64;
  int mouthY  = 70;

  u8g2.drawCircle(centerX, mouthY, 8, U8G2_DRAW_LOWER_LEFT | U8G2_DRAW_LOWER_RIGHT);
  u8g2.drawCircle(centerX, mouthY+1, 8, U8G2_DRAW_LOWER_LEFT | U8G2_DRAW_LOWER_RIGHT);
  u8g2.drawCircle(centerX, mouthY+2, 8, U8G2_DRAW_LOWER_LEFT | U8G2_DRAW_LOWER_RIGHT);

  u8g2.drawDisc(28, 75, 3);
  u8g2.drawDisc(100, 75, 3);
}

// WiFi
const char* ssid = "Pomodoro_Timer";
const char* password = "12345678";
WebServer server(80);

unsigned long totalRemaining = 0;
unsigned long phaseStartTime = 0;
unsigned long phaseDuration = 0;
unsigned long lastInputTime = 0;
unsigned long pauseElapsed = 0;

bool timerRunning = false;
bool isBreak = false;
bool studyDone = false;
bool timeSaved = false;
bool isPaused = false;
bool displayState = false;

bool lastButtonState = HIGH;
bool lastStartButtonState = HIGH;
bool lastRestartButtonState = HIGH;

unsigned long totalStudyTime = 0;
unsigned long focusDuration = 0;
unsigned long breakDuration = 0;

String userNameOLED = "";

// WEB DASHBOARD
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleSave() {
  int h = server.hasArg("studyHour") ? server.arg("studyHour").toInt() : 0;
  int m = server.hasArg("studyMinute") ? server.arg("studyMinute").toInt() : 0;
  int f_h = server.hasArg("focusHour") ? server.arg("focusHour").toInt() : 0;
  int f_m = server.hasArg("focusMinute") ? server.arg("focusMinute").toInt() : 0;
  int b_h = server.hasArg("breakHour") ? server.arg("breakHour").toInt() : 0;
  int b_m = server.hasArg("breakMinute") ? server.arg("breakMinute").toInt() : 0;

  unsigned long tempTotalStudy = (h * 3600UL + m * 60UL) * 1000UL;
  unsigned long tempFocus = (f_h * 3600UL + f_m * 60UL) * 1000UL;
  unsigned long tempBreak = (b_h * 3600UL + b_m * 60UL) * 1000UL;

   if (tempFocus + tempBreak > tempTotalStudy) {
    server.send(200, "text/plain", "ERROR");
    return;   
  }

  totalStudyTime = tempTotalStudy;
  totalRemaining = totalStudyTime;
  lastInputTime = totalRemaining;

  focusDuration = tempFocus;
  breakDuration = tempBreak;

  timerRunning = false;
  studyDone = false;
  isBreak = false;
  isPaused = false;
  timeSaved = true;

  userNameOLED = server.hasArg("userName") ? server.arg("userName") : "";

  server.send(200, "text/plain", "OK");

}

void handleReset() {
  timerRunning = false;
  studyDone = false;
  totalRemaining = 0;
  isBreak = false;
  timeSaved = false;
  isPaused = false;
  pauseElapsed = 0;
  phaseDuration = 0;
  userNameOLED = "";
  server.sendHeader("Location", "/");
  server.send(303);
}

// SETUP 
void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(RESTART_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  Wire.begin(21, 22);
  u8g2.begin();
  u8g2.setContrast(180);

  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.on("/reset", handleReset);
  server.begin();
}

void loop() {
  server.handleClient();
  unsigned long currentMillis = millis();

  // Robot Blink Logic
  if (!isBlinking && currentMillis - lastBlinkTime > 4000) {
    isBlinking = true;
    lastBlinkTime = currentMillis;
  }
  if (isBlinking && currentMillis - lastBlinkTime > 200) {
    isBlinking = false;
    lastBlinkTime = currentMillis;
  }

  bool readingOLED = digitalRead(BUTTON_PIN);
  if (readingOLED == LOW && lastButtonState == HIGH) {
    displayState = !displayState;
  }
  lastButtonState = readingOLED;

  bool readingStart = digitalRead(START_BUTTON);
  if (readingStart == LOW && lastStartButtonState == HIGH) {
    if (timeSaved && !studyDone) {
      if (!timerRunning && !isPaused) {
        timerRunning = true;
        isBreak = false;
        phaseDuration = (totalRemaining >= focusDuration) ? focusDuration : totalRemaining;
        phaseStartTime = currentMillis;
      } 
      else if (timerRunning) {
        timerRunning = false;
        isPaused = true;
        pauseElapsed = currentMillis - phaseStartTime;
      } 
      else if (isPaused) {
        timerRunning = true;
        isPaused = false;
        phaseStartTime = currentMillis - pauseElapsed;
      }
    }
  }
  lastStartButtonState = readingStart;

  // Restart_Button
  bool readingRestart = digitalRead(RESTART_BUTTON);
  if (readingRestart == LOW && lastRestartButtonState == HIGH) {
    totalRemaining = lastInputTime;
    timerRunning = false;
    studyDone = false;
    isBreak = false;
    isPaused = false;
    pauseElapsed = 0;
  }
  lastRestartButtonState = readingRestart;

  if (timerRunning) {
    unsigned long elapsed = currentMillis - phaseStartTime;
    if (elapsed >= phaseDuration) {
      totalRemaining -= phaseDuration;

      // BUZZER ALERT
      if (!isBreak && totalRemaining > 0) {
        softBeep(1000);  
      } 
      else if (isBreak && totalRemaining > 0) {
        softBeep(1000); 
      } 
      else if (totalRemaining <= 0) {
        softBeep(2000);  
      }

      if (totalRemaining <= 0) {
        totalRemaining = 0;
        timerRunning = false;
        studyDone = true;
      } 
      else {
        isBreak = !isBreak;
        phaseDuration = (!isBreak) ?
          ((totalRemaining >= focusDuration) ? focusDuration : totalRemaining)
          :
          ((totalRemaining >= breakDuration) ? breakDuration : totalRemaining);

        phaseStartTime = currentMillis;
      }
    }
  }

  // DISPLAY 
  u8g2.clearBuffer();

  if (displayState) {

    char timeBuffer[10];
    unsigned long displayRemaining;
    unsigned long h, m, s;

    static bool landingShown = false;
    static unsigned long landingStartTime = 0;

    if (!landingShown && landingStartTime == 0) {
      landingStartTime = currentMillis;
    }

    if (!landingShown && !timeSaved) {
      if (currentMillis - landingStartTime < 3000) {
        u8g2.drawXBMP(0, 0, 128, 128, trioe_logo_128x128);
      } else {
        landingShown = true;
      }
    }

    else if (!timeSaved && landingShown) {

      drawHead();
      drawEyes(isBlinking);
      drawMouth();

      u8g2.setFont(u8g2_font_logisoso16_tr);
      u8g2.drawStr(25, 20, "SET TIME");

      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(15, 120, "Pomodoro Timer");
    }

    else if (!timerRunning && !studyDone && !isPaused) {

      displayRemaining = totalRemaining / 1000;
      h = displayRemaining / 3600;
      m = (displayRemaining / 60) % 60;
      s = displayRemaining % 60;
      sprintf(timeBuffer, "%02lu:%02lu:%02lu", h, m, s);

      u8g2.setFont(u8g2_font_logisoso16_tr);
      String greeting = "Hi " + userNameOLED + "!";
      int16_t textWidth = u8g2.getStrWidth(greeting.c_str());
      u8g2.drawStr((128 - textWidth) / 2, 20, greeting.c_str());  

      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(20, 40, "Focus mode: ON");    

      u8g2.setFont(u8g2_font_logisoso24_tr);
      u8g2.drawStr(5, 80, timeBuffer);

      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(15, 120, "Press Start Button");
    }

  // PAUSED 
    else if (isPaused) {

      if (!isBreak)
        displayRemaining = (totalRemaining - pauseElapsed) / 1000;
      else
        displayRemaining = (phaseDuration > pauseElapsed ? 
                            (phaseDuration - pauseElapsed) : 0) / 1000;

      if ((long)displayRemaining < 0) displayRemaining = 0;

  // RADIAL SETTINGS
      int centerX = 64;
      int centerY = 64;
      int radius  = 55;

      int secondInMinute = displayRemaining % 60;

      for (int i = 0; i < 60; i++) {
        float angle = (i * 6 - 90) * PI / 180.0;
        int x1 = centerX + (radius - 7) * cos(angle);
        int y1 = centerY + (radius - 7) * sin(angle);
        int x2 = centerX + radius * cos(angle);
        int y2 = centerY + radius * sin(angle);

        if (i < secondInMinute)
          u8g2.setDrawColor(15); 
        else
          u8g2.setDrawColor(0); 

        u8g2.drawLine(x1, y1, x2, y2);
      }

      h = displayRemaining / 3600;
      m = (displayRemaining % 3600) / 60;
      sprintf(timeBuffer, "%02lu:%02lu", h, m);

      u8g2.setDrawColor(15);
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(18, 85, isBreak ? "BREAK PAUSED" : "STUDY PAUSED");

      u8g2.setFont(u8g2_font_logisoso24_tr);
      int textWidth = u8g2.getStrWidth(timeBuffer);
      u8g2.drawStr(centerX - textWidth / 2, centerY + 5, timeBuffer);
    }

    else if (studyDone || totalRemaining == 0) {

      u8g2.setFont(u8g2_font_logisoso16_tr);
      String line1 = "STUDY DONE";
      u8g2.drawStr((128 - u8g2.getStrWidth(line1.c_str())) / 2, 45, line1.c_str());
      String line2 = userNameOLED;
      u8g2.drawStr((128 - u8g2.getStrWidth(line2.c_str())) / 2, 70, line2.c_str());
      String line3 = "Great Job!";
      u8g2.drawStr((128 - u8g2.getStrWidth(line3.c_str())) / 2, 95, line3.c_str());
    }

    else {

      if (isBreak)
        displayRemaining = (phaseDuration - (currentMillis - phaseStartTime)) / 1000;
      else
        displayRemaining = (totalRemaining - (currentMillis - phaseStartTime)) / 1000;

      if ((long)displayRemaining < 0) displayRemaining = 0;

      int centerX = 64;
      int centerY = 64;
      int radius  = 55;

      int secondInMinute = displayRemaining % 60;

      for (int i = 0; i < 60; i++) {
        float angle = (i * 6 - 90) * PI / 180.0;
        int x1 = centerX + (radius - 7) * cos(angle);
        int y1 = centerY + (radius - 7) * sin(angle);
        int x2 = centerX + radius * cos(angle);
        int y2 = centerY + radius * sin(angle);

        if (i < secondInMinute)
          u8g2.setDrawColor(15);
        else
          u8g2.setDrawColor(0);

        u8g2.drawLine(x1, y1, x2, y2);
      }

      h = displayRemaining / 3600;
      m = (displayRemaining % 3600) / 60;
      sprintf(timeBuffer, "%02lu:%02lu", h, m);

      u8g2.setDrawColor(15);
      u8g2.setFont(u8g2_font_ncenB08_tr);
      String stateText = isBreak ? "BREAK TIME" : "STUDY TIME";
      int stateTextWidth = u8g2.getStrWidth(stateText.c_str());
      u8g2.drawStr((128 - stateTextWidth) / 2, 85, stateText.c_str());

      u8g2.setFont(u8g2_font_logisoso24_tr);
      int textWidth = u8g2.getStrWidth(timeBuffer);
      u8g2.drawStr(centerX - textWidth / 2, centerY + 5, timeBuffer);
    }
  }

  u8g2.sendBuffer();
}