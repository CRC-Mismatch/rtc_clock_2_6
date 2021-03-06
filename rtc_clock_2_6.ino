#include <stdio.h>
#include <DS1302.h>
#include <TimedAction.h>
#include <FL7Dl.h>
#include <NewTone.h>
#include <EEPROM.h>

#define MAX_DELAY 30U
#define MAX_BRITE 500
#define SSP A5
#define DTP 12
#define CLP 13
#define alP 2
#define tickI 1000U
#define rtcI 5000U
#define alChk 2000U
#define alI 500U

FL7Dl disp = FL7Dl();
DS1302 rtc = DS1302(SSP, DTP, CLP);
TimedAction ticker = TimedAction(tickI, tick);
TimedAction rtcUpdater = TimedAction(rtcI, rtcUpdate);
TimedAction dispNew = TimedAction(10, dispNum);
TimedAction checkAlUpd = TimedAction(alChk, chkAlUpdate);
TimedAction checkAlarm = TimedAction(alI, alarmDo);
TimedAction iniSet = TimedAction(10, setting);

boolean hasSet = false;
int now = 0;
int alarm = 2500;
byte alState = 0;
byte ddelay = 10;
byte sss = 0;
int brite = 500;
bool alOn = false;

void tick() {
  if (sss > 59) {
    now++;
    sss=0;
  }
  disp.tick();
  sss++;
  if (now % 100 > 59) {
    now += 100;
    now -= now % 100;
  }
  if (now / 100 > 23) {
    now = 0;
  }
  if (now >= alarm - 1) {
    checkAlarm.enable();
  } else if (now > alarm + 2 && now <= alarm + 3) {
    checkAlarm.disable();
  }
}

void rtcUpdate() { 
  Time t = rtc.time();
  now = (t.hr * 100) + t.min;
  sss = t.sec;
}

void dispNum() {
  disp.showTime(now);
}

void chkAlUpdate() {
  if (Serial.peek() == '@' && Serial.available() >= 15) {
    rtc.writeProtect(false);
    Serial.read();
    char buffer[16];
    int year, mon, day, hr, min, sec, week;
    Serial.readBytes(buffer, 2);
    day = atoi(buffer);
    Serial.readBytes(buffer, 2);
    mon = atoi(buffer);
    Serial.readBytes(buffer, 4);
    year = atoi(buffer);
    buffer[2] = '\0';
    Serial.readBytes(buffer, 2);
    hr = atoi(buffer);
    Serial.readBytes(buffer, 2);
    min = atoi(buffer);
    Serial.readBytes(buffer, 2);
    sec = atoi(buffer);
    buffer[1] = '\0';
    Serial.readBytes(buffer, 1);
    week = atoi(buffer);
    Time newT = Time(year, mon, day, hr, min, sec, Time::Day(week));
    rtc.time(newT);
    char buf2[50];
    rtc.writeProtect(true);
    Time t = rtc.time();
    snprintf(buf2, sizeof(buf2), "\nTime set to: %01d, %02d/%02d/%04d - %02d:%02d:%02d", t.day, t.date, t.mon, t.yr, t.hr, t.min, t.sec);
    Serial.println(buf2);
    now = (t.hr * 100) + t.min;
    sss = t.sec;
    if (now >= alarm - 1) {
      checkAlarm.enable();
    } else if (now > alarm + 2 && now <= alarm + 3) {
      checkAlarm.disable();
    }
    disp.alarm(alOn);
  }
  if (Serial.peek() == '#' && Serial.available() >= 5) {
    Serial.read();
    char buffer1[5];
    Serial.readBytes(buffer1, 4);
    int newAl = atoi(buffer1);
    alarm = newAl;
    EEPROM.write(0xa1, newAl % 100);
    newAl /= 100;
    if (alarm > 0 && alarm < 2400) alOn = true;
    newAl |= alOn << 7;
    EEPROM.write(0xa2, newAl);
    char buf2[50];
    snprintf(buf2, sizeof(buf2), "\nAlarm set to: %02d:%02d", alarm / 100, alarm % 100);
    Serial.println(buf2);
    int n = 0;
    disp.tick(ON);
    while (n < (2000 / (ddelay + (brite*4/1000)))) {
      disp.showTime(alarm);
      n++;
      delay(ddelay);
    }
    disp.tick(OFF);
  }
  if (Serial.peek() == '!' && Serial.available() >= 4) {
    Serial.read();
    char buffer2[4];
    buffer2[3] = '\0';
    Serial.readBytes(buffer2, 3);
    brite = atoi(buffer2);
    brite = (brite > MAX_BRITE) ? MAX_BRITE : brite;
    EEPROM.write(0xb1, brite % 100);
    EEPROM.write(0xb2, brite / 100);
    disp.setBrightness(brite);
    disp.tick(OFF);
    int n = 0;
    char buf2[50];
    snprintf(buf2, sizeof(buf2), "\nBrightness set to: %03d", brite);
    Serial.println(buf2);
    while (n < (2000 / (ddelay + (brite*4/1000)))) {
      disp.showTime(brite);
      n++;
      delay(ddelay);
    }
    disp.tick(ON);
  }
  if (Serial.peek() == '$' && Serial.available() >= 2) {
    Serial.read();
    char buffer3[3];
    buffer3[2] = '\0';
    Serial.readBytes(buffer3, 2);
    int dela = atoi(buffer3);
    dela = (dela > MAX_DELAY) ? MAX_DELAY : dela;
    EEPROM.write(0xde, dela);
    ddelay = dela;
    dispNew.setInterval(ddelay);
    disp.tick(OFF);
    int n = 0;
    char buf2[50];
    snprintf(buf2, sizeof(buf2), "\nDelay set to: %02d", dela);
    Serial.println(buf2);
    while (n < (2000 / (ddelay + (brite*4/1000)))) {
      disp.showTime(ddelay);
      n++;
      delay(ddelay);
    }
    disp.tick(ON);    
  }
  Serial.flush();
  if (alarm >= 0 && alarm <= 2359) {
    disp.alarm(ON);
  }
}

void alarmDo() {
  if (now >= alarm && now < alarm + 2 && alOn) {
  switch (alState) {
    case 0:
      NewTone(alP, 4000, 100);
      disp.alarm(ON);
      disp.yellow(ON);
      disp.green(ON);
      disp.setBrightness(500);
      dispNew.setInterval(10);
      checkAlarm.setInterval(150);
      alState++;
      break;
    case 1:
      NewTone(alP, 4000, 100);
      disp.alarm(OFF);
      disp.yellow(OFF);
      disp.green(OFF);
      disp.setBrightness(50);
      dispNew.setInterval(25);
      alState++;
      break;
    case 2:
      NewTone(alP, 4000, 100);
      disp.alarm(ON);
      disp.yellow(ON);
      disp.green(ON);
      disp.setBrightness(500);
      dispNew.setInterval(10);
      alState++;
      break;
    case 3:
      NewTone(alP, 4000, 100);
      disp.alarm(OFF);
      disp.yellow(OFF);
      disp.green(OFF);
      disp.setBrightness(brite);
      dispNew.setInterval(ddelay);
      alState++;
      break;
    case 4:
      noNewTone(alP);
      checkAlarm.setInterval(400);
      alState = 0;
      break;
  }
  } else if (now >= alarm + 2 && now < alarm + 3) {
      disp.alarm(OFF);
      disp.yellow(OFF);
      disp.green(OFF);
    checkAlarm.setInterval(alI);
      dispNew.setInterval(ddelay);    
      disp.setBrightness(brite);
    alState = 0;
    noNewTone(alP);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.write("!?!");
  ddelay = EEPROM.read(0xde);
  if (ddelay > MAX_DELAY) {
    ddelay = MAX_DELAY;
    EEPROM.write(0xde, MAX_DELAY);
  }
  dispNew.setInterval(ddelay);
  byte lo, hi;
  lo = EEPROM.read(0xa1);
  hi = EEPROM.read(0xa2) & B01111111;
  alarm = (hi * 100) + lo;
  lo = EEPROM.read(0xb1);
  hi = EEPROM.read(0xb2);
  brite = (hi * 100) + lo;
  if (brite > MAX_BRITE) {
    brite = MAX_BRITE;
    EEPROM.write(0xb1, MAX_BRITE % 100);
    EEPROM.write(0xb2, MAX_BRITE / 100);
  }
  disp.setBrightness(brite);
  now = 0;
  disp.tick(OFF);
  int w = 0;
  iniSet.reset();
  while (w < (2000 / (ddelay + (brite*4 / 1000))) && !hasSet) {
    disp.showString("sett");
    iniSet.check();
    w++;
    delay(ddelay);
  }
  rtcUpdate();
  chkAlUpdate();
  if (!hasSet) {
    disp.tick(ON);
    int n = 0;
    while (n < (1000 / (ddelay + (brite * 4 / 1000)))) {
      disp.showTime(alarm);
      n++;
      delay(ddelay);
    }
    disp.tick(OFF);
  }
  if (now < alarm || now > alarm + 2) checkAlarm.disable();
}

void setting() {
  if (Serial.peek() == '@' && Serial.available() >= 21) {
    hasSet = true;
    iniSet.disable();
  }
}
  

void loop() {
  checkAlUpd.check();
  checkAlarm.check();
  rtcUpdater.check();
  dispNew.check();
  ticker.check();
}
