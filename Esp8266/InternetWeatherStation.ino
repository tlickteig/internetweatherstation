#include <ESP8266WiFi.h>
#include <Time.h>
#include <WiFiClientSecure.h>
//#include <WiFi.h>
#include <JSON_Decoder.h>
#include <DarkSkyWeather.h>

//DarkSky accout information
const String api = "apikey";
const String latitude = "latitude";
const String longitude = "longitude";
const String units = "us";
const String language = " ";

//Wifi Details
#define ssid "ssid"
#define password "password"

//Data variables
String timeData[] = {"year", "month", "day", "weekday", "hour", "minute", "second"};
String weatherData [] = {
  "temperature"
  , "humidity"
  , "pressure"
  , "windSpeed"
  , "precipProbability"
  , "moonPhase"
  , "overallSummary"
  , "summary"
  , "cloudCover"
  , "windBearing"
                         };

//Other variables
DS_Weather dsw;
int del = 0;
const int timezone = -5;

void setup() {

  //Setup serial
  Serial.begin(9600);
  Serial.setDebugOutput(false);
  Serial.setTimeout(50);
  
  //Connect the ESP to wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  //Sync time and weather
  configTime(timezone * 3600, 0, "pool.ntp.org", "time.nist.gov");
  getWeather();

}

void loop() {

  int serialMode = 0;

  //Sync the time and get
  if(del == 10000) {
    configTime(timezone * 3600, 0, "pool.ntp.org", "time.nist.gov");
    del = 0;
  }
  getTime();

  //Sync the weather
  int intMinute = timeData[5].toInt();
  int intSecond = timeData[6].toInt();
  if(((intMinute % 10) == 0) and (intSecond < 2)) {
    getWeather();
  }

  //Process data requests
  if(Serial.available()) {
    serialMode = Serial.readString().toInt();
  }

  //Check for a time request
  if(serialMode == 1) {
    for(int i = 0; i < 7; i++) {
      Serial.println(timeData[i]);
    }    
  }

  //Check for a weather request
  if(serialMode == 2) {
    for(int i = 0; i < 10; i++) {
      Serial.println("#" + weatherData[i]);
    }
  }

  //Check for a status request
  if(serialMode == 3) {
    if(WiFi.status() == WL_CONNECTED) {
      Serial.println("1");
    }
    else {
      Serial.println("2");
    }
  }

  del++;

}

void getTime() {

  //Get the time
  time_t now = time(nullptr);
  String current = (String)(ctime(&now));  

  //Parse the data
  timeData[3] = current.substring(0, 3);
  timeData[1] = current.substring(4, 7);
  timeData[2] = current.substring(8, 10);
  timeData[4] = current.substring(11, 13);
  timeData[5] = current.substring(14, 16);
  timeData[6] = current.substring(17, 19);
  timeData[0] = current.substring(20, 24); 
  
}

void getWeather() {

  //Create objects to hold weather data
  DSW_current *current = new DSW_current;
  DSW_hourly *hourly = new DSW_hourly;
  DSW_daily *daily = new DSW_daily;

  //Get the weather
  dsw.getForecast(current, hourly, daily, api, latitude, longitude, units, language);

  //Begin assigning values to the weather array
  weatherData[0] = (String)(current->temperature);
  weatherData[1] = (String)(current->humidity);
  weatherData[2] = (String)(current->pressure);
  weatherData[3] = (String)(current->windSpeed);
  weatherData[4] = (String)(current->precipProbability);
  weatherData[5] = (String)(daily->moonPhase[0]);
  weatherData[6] = (String)(daily->summary[0]);
  weatherData[7] = (String)(current->summary);
  weatherData[8] = (String)(current->cloudCover);
  weatherData[9] = (String)(current->windBearing);
  
}
