#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//Setup LCD
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3,  POSITIVE);

//Declare constants for I/O
#define buttonPin 2
#define tempGuage 5
#define humidityGuage 6
#define airPressureGuage 9
#define moonGuage 3
#define windSpeedGuage 10

//Declare variables for data
int wifiStatus = 0;
int displayMode = 0;
String timeData[] = {
                   "year"
                   , "month"
                   , "day"
                   , "weekday"
                   , "hour"
                   , "minute"
                   , "second"
                      };
String weatherData[] = {
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

void setup() {

  //Setup serial
  Serial.begin(9600);
  Serial.setTimeout(50);

  //Enable analog output
  pinMode(tempGuage, OUTPUT);
  pinMode(airPressureGuage, OUTPUT);
  pinMode(humidityGuage, OUTPUT);
  pinMode(moonGuage, OUTPUT);
  pinMode(windSpeedGuage, OUTPUT);

  //Test analog display
  analogWrite(tempGuage, 255);
  analogWrite(airPressureGuage, 255);
  analogWrite(humidityGuage, 255);
  analogWrite(moonGuage, 255);
  analogWrite(windSpeedGuage, 255);

  //Setup button control
  pinMode(buttonPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), press, RISING);

  //Setup display
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);

  //Wait for wifi to be connected
  lcd.print("Connecting....");
  while(wifiStatus != 1) {
    Serial.println("3");
    if(Serial.available()) {
      wifiStatus = Serial.readString().toInt();
    }
    delay(300);
  }
  lcd.setCursor(0, 0);
  lcd.print("Connected      ");

}

void loop() {

  //Retreive data
  retreiveData();

  if(displayMode >= 5) {
    displayMode = 0;
  }

  //Write to the digital display
  writeToDigitalDisplay();

  //Write to the analog display
  int moonPhasePercent = calculateMoon(weatherData[5]).toInt();
  writeToAnalogDisplay(moonPhasePercent);

}

void requestData() {
  
  Serial.println("1");
  delay(50);
  Serial.println("2");
  delay(50);
  Serial.println("3");
  delay(50);
}

void retreiveData() {

  //Declare variables
  String rawWeather;
  String rawTime;
  int index1 = 0;
  int index2 = 0;
  char parsedWeather[] = {};

  //Retreive clock data
  Serial.println("1");
  delay(50);
  rawTime = Serial.readString();

  //Retreive weather data
  Serial.println("2");
  delay(50);
  rawWeather = Serial.readString();

  //Parse weather data into a char array
  for(int i = 0; i < rawWeather.length(); i++) {
    parsedWeather[i] = rawWeather.charAt(i);
  }

  //Parse the time data
  timeData[0] = rawTime.substring(0, 4);
  timeData[1] = rawTime.substring(6, 9);
  timeData[2] = rawTime.substring(11, 13);
  timeData[3] = rawTime.substring(15, 18);
  timeData[4] = rawTime.substring(20, 22);
  timeData[5] = rawTime.substring(24, 26);
  timeData[6] = rawTime.substring(28, 30);

  //Parse the weather data into its respective array
  index1 = 1;
  weatherData[0] = rawWeather.substring(0, rawWeather.indexOf("#", index1));
  
  index1 = rawWeather.indexOf("#", index1 + 1);
  weatherData[1] = rawWeather.substring(index1, rawWeather.indexOf("#", index1 + 1));
  
  index1 = rawWeather.indexOf("#", index1 + 1);
  weatherData[2] = rawWeather.substring(index1, rawWeather.indexOf("#", index1 + 1));
  
  index1 = rawWeather.indexOf("#", index1 + 1);
  weatherData[3] = rawWeather.substring(index1, rawWeather.indexOf("#", index1 + 1));

  index1 = rawWeather.indexOf("#", index1 + 1);
  weatherData[4] = rawWeather.substring(index1, rawWeather.indexOf("#", index1 + 1));

  index1 = rawWeather.indexOf("#", index1 + 1);
  weatherData[5] = rawWeather.substring(index1, rawWeather.indexOf("#", index1 + 1));

  index1 = rawWeather.indexOf("#", index1 + 1);
  weatherData[6] = rawWeather.substring(index1, rawWeather.indexOf("#", index1 + 1));

  index1 = rawWeather.indexOf("#", index1 + 1);
  weatherData[7] = rawWeather.substring(index1, rawWeather.indexOf("#", index1 + 1));

  index1 = rawWeather.indexOf("#", index1 + 1);
  weatherData[8] = rawWeather.substring(index1, rawWeather.indexOf("#", index1 + 1));

  index1 = rawWeather.indexOf("#", index1 + 1);
  weatherData[9] = rawWeather.substring(index1, rawWeather.indexOf("#", index1 + 1));
  

  //Trim excess characters from the data
  for(int i = 0; i < 10; i++) {
    weatherData[i] = weatherData[i].substring(1, weatherData[i].length() - 2);
  }
  
}

void writeToDigitalDisplay() {

  switch(displayMode) {
    
    case 0:
      printLCD(centerText(getName(timeData[3]), 20), 0);
      printLCD(centerText(getName(timeData[1]), 20), 1);
      printLCD("      " + timeData[4] + ":" + timeData[5] + ":" + timeData[6], 2);
      printLCD("     " + getMonthNumber(timeData[1]) + "/" + timeData[2] + "/" + timeData[0], 3);
      break;
    case 1: 
      printLCD("Temperature: " + roundNumber(weatherData[0]) + " *F", 0);
      printLCD("Humidity: " + weatherData[1] + "%", 1);
      printLCD("Pressure: " + roundNumber(weatherData[2]) + " mb", 2);
      printLCD("Wind Speed: " + roundNumber(weatherData[3]) + " mph", 3);
      break;
    case 2:
      printLCD("Wind->: " + calculateWindDirection(weatherData[9]), 0);
      printLCD("Moisture: " + calculateMoisture(weatherData[1]), 1);
      printLCD("Moon: " + calculateMoonPhase(weatherData[5]), 2);
      printLCD(weatherData[7], 3);
      break;
    case 3:
      printLCD("Precip Chance: " + roundNumber(weatherData[3]) + "%", 0);
      printLCD("Cloud Cover: " + weatherData[8] + "%", 1);
      printLCD("Moon: " + calculateMoon(weatherData[5]) + " %", 2);
      printLCD("Wind angle: " + weatherData[9] + "*", 3);
      break; 
    case 4:
      String blankText = "";

      //Create whitespace in negative porportion to string length
      for(int i = 0; i < (80 - weatherData[6].length()); i++) {
        blankText += " ";
      }

      //Add whitespace to end of the string
      weatherData[6] = weatherData[6] + blankText;

      //Divide up the string 20 chars per line
      printLCD(weatherData[6].substring(0, 20), 0);
      printLCD(weatherData[6].substring(20, 40), 1);
      printLCD(weatherData[6].substring(40, 60), 2);
      printLCD(weatherData[6].substring(60, 80), 3);
      break;   
  }
  
}

void writeToAnalogDisplay(int moonPhasePercent) {

  //Declare variables
  float mFactor[] = {1.48, 1.58, 2.48, 2.43, 2.66};
  float bFactor[] = {49.5, -1456, 7.5, 5.5, -1.42};
  float weatherInfo[] = {0, 0, 0, 0, 0};
  float data[] = {0, 0, 0, 0, 0};

  //Retreive data from global variable
  weatherInfo[0] = weatherData[0].toDouble();
  weatherInfo[1] = weatherData[2].toDouble();
  weatherInfo[2] = weatherData[1].toDouble();
  weatherInfo[3] = (float)moonPhasePercent;
  weatherInfo[4] = weatherData[3].toDouble();

  //Map data to the analog pins
  for(int i = 0; i < 5; i++) {
    data[i] = (weatherInfo[i] * mFactor[i]) + bFactor[i];
  }

  //Write to the guages
  analogWrite(tempGuage, data[0]);
  analogWrite(airPressureGuage, data[1]);
  analogWrite(humidityGuage, data[2]);
  analogWrite(moonGuage, data[3]);
  analogWrite(windSpeedGuage, data[4]);

}

/*
 * Other functions
 */
void printLCD(String data, int line) {
  
  String clearLine = "                    ";
  lcd.setCursor(0, line);
  lcd.print(clearLine);
  lcd.setCursor(0, line);
  lcd.print(data);
  
}


void press() {
  displayMode++;

  int n = 0;
  for(int i = 0; i < 30000; i++) {
    n = i;
  }
}

String calculateMoonPhase(String input) {
  
  int number = input.toInt();
  String result = "Unknown";

  if((number >= 0) and (number < 5)) {
    result = "New Moon";
  }
  else if((number >= 5) and (number < 20)) {
    result = "Waxing Crescent";
  }
  else if((number >= 20) and (number < 30)) {
    result = "First Quarter";
  }
  else if((number >= 30) and (number < 45)) {
    result = "Waxing Gibbous";
  }
  else if((number >= 45) and (number < 55)) {
    result = "Full Moon";
  }
  else if((number >= 55) and (number < 70)) {
    result = "Waning Gibbous";
  }
  else if((number >= 70) and (number < 80)) {
    result = "Third Quarter";
  }
  else if((number >= 80) and (number < 95)) {
    result = "Waning Crescent";
  }
  else if(number >= 95) {
    result = "New Moon";
  }

  return result;
}

String calculateWindDirection(String input) {

  int angle = input.toInt();
  String result = "Unknown";

  if(angle < 10) {
    result = "South";
  }
  else if((angle >= 10) and (angle < 80)) {
    result = "Southwest";
  }
  else if((angle >= 80) and (angle < 100)) {
    result = "West";
  }
  else if((angle >= 100) and (angle < 170)) {
    result = "Northwest";
  }
  else if((angle >= 170) and (angle < 190)) {
    result = "North";
  }
  else if((angle >= 190) and (angle < 260)) {
    result = "Northeast";
  }
  else if((angle >= 260) and (angle < 280)) {
    result = "East";
  }
  else if((angle >= 280) and (angle < 350)) {
    result = "Southeast";
  }
  else if(angle >= 350) {
    result = "South";
  }

  return result;  
}

String calculateMoisture(String input) {

  int number = input.toInt();
  String output = "Unknown";

  if(number < 17) {
    output = "Very Dry";
  }
  else if((number >= 17) and (number < 35)) {
    output = "Dry";
  }
  else if((number >= 35) and (number < 65)) {
    output = "Normal";
  }
  else if((number >= 65) and (number < 82)) {
    output = "Humid";
  }
  else if(number >= 82) {
    output = "Very Humid";
  }
  
  return output;
}

String calculateMoon(String input) {

  int number = input.toInt();
  int percentage;

  if(number <= 50) {
    percentage = number * 2;
  }
  else {
    percentage = (100 - number) * 2;
  }
  
  return (String)percentage;
}

String getName(String input) {

  String output = "Unknown";

  if(input == "Sun") {
    output = "Sunday";
  }
  else if(input == "Mon") {
    output = "Monday";
  }
  else if(input == "Tue") {
    output = "Tuesday";
  }
  else if(input == "Wed") {
    output = "Wednesday";
  }
  else if(input == "Thu") {
    output = "Thursday";
  }
  else if(input == "Fri") {
    output = "Friday";
  }
  else if(input == "Sat") {
    output = "Saturday";
  }
  else if(input == "Jan") {
    output = "January";
  }
  else if(input == "Feb") {
    output = "February";
  }  
  else if(input == "Mar") {
    output = "March";
  }  
  else if(input == "Apr") {
    output = "April";
  }  
  else if(input == "May") {
    output = "May";
  } 
  else if(input == "Jun") {
    output = "June";
  }  
  else if(input == "Jul") {
    output = "July";
  }  
  else if(input == "Aug") {
    output = "August";
  } 
  else if(input == "Sep") {
    output = "September";
  }  
  else if(input == "Oct") {
    output = "October";
  }  
  else if(input == "Nov") {
    output = "November";
  }  
  else if(input == "Dec") {
    output = "December";
  }

  return output;
}

String getMonthNumber(String input) {
  
  String output = "00";

    if(input == "Jan") {
      output = "01";
    }
    else if(input == "Feb") {
      output = "02";
    }  
    else if(input == "Mar") {
      output = "03";
    }  
    else if(input == "Apr") {
      output = "04";
    }  
    else if(input == "May") {
      output = "05";
    } 
    else if(input == "Jun") {
      output = "06";
    }  
    else if(input == "Jul") {
      output = "07";
    }  
    else if(input == "Aug") {
      output = "08";
    } 
    else if(input == "Sep") {
      output = "09";
    }  
    else if(input == "Oct") {
      output = "10";
    }  
    else if(input == "Nov") {
      output = "11";
    }  
    else if(input == "Dec") {
      output = "12";
    }
  return output;  
}

String roundNumber(String input) {

  float number = input.toFloat();
  return (String)round(number);
}

String centerText(String input, int width) {

  int len = input.length();
  int space = (width - len) / 2;
  String whiteSpace = "";

  for(int i = 0; i < space; i++) {
    whiteSpace = whiteSpace + " ";
  }
  
  return whiteSpace + input;
  
}
