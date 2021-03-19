#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>
#include "RTClib.h"
RTC_DS3231 rtc;

// initialize the lcd address for communcation
LiquidCrystal_I2C lcd(0x27,16,2);

// define wifi credentials
const char *ssid     = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

bool initialDateShown = false;
String today = "";
int ymd[] = {0, 0, 0};
String days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// set the ntp client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ro.pool.ntp.org", 7200, 60000);

void setup() {
  lcd.init();
  lcd.backlight();

  showInitMessageLine("Init RTC module...");
  delay(2000);
  if (! rtc.begin()) {
    showInitMessageLine("Couldn't find RTC");
    abort();
  }

  showInitMessageLine("WiFi init ...");
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();

  showInitMessageLine("Adjust time...");
  timeClient.update();
  rtc.adjust(timeClient.getEpochTime());
  WiFi.mode(WIFI_OFF);
  showInitMessageLine("Init done!");
  delay(2000);
  lcd.clear();
  lcd.setCursor(4,0);
}

// small function that will show init messages on the lcd
void showInitMessageLine(String line){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line);
}

// function that gets the current time and returns a string with the formated time
String formattedTime(DateTime now) {
  String result = "";
  if(now.hour() < 10){
    result.concat("0");
  }
  result.concat(now.hour());
  result.concat(":");
  if(now.minute() < 10){
    result.concat("0");
  }
  result.concat(now.minute());
  result.concat(":");
  if(now.second() < 10){
    result.concat("0");
  }
  result.concat(now.second());
  return result;
}


// same here for the date
String formatedDate(DateTime now) {
  String result = "";
  result = result + days[now.dayOfTheWeek()] + " ";
  result.concat(now.day());
  result.concat("-");
  result.concat(now.month());
  result.concat("-");
  result.concat(now.year());
  return result;
}


void loop() {
  DateTime now = rtc.now();
  lcd.print(formattedTime(now));
  lcd.setCursor(1,1);
  // show the date only once a day (and keep it on the lcd until the midnight) in order to avoid the lcd flicker
  if(!initialDateShown){
    today = formatedDate(now);
    initialDateShown = true;
    lcd.print(today);
  } else {
    if(now.hour() == 23 && now.minute() == 59 && now.second()== 59) {
      initialDateShown = false;
    }
  }
  delay(1000); // wait for a second
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(4,0);

}
