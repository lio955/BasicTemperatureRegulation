//*****************************************//
// Author : Lionel Protin
// Date : 23/03/2023
// Designed for Arduino Nano 
// Temperature regulation for beer fermentation
//*****************************************//

// For DS18B20 sensor
#include <OneWire.h>
#include <DallasTemperature.h>

// DS18B20 sensor is on PIN 2 of the Arduino
#define ONE_WIRE_BUS 2

// Solid state relay is on PINs 3 and4 of the Arduino
#define RELAY_PIN_COLD 3  
#define RELAY_PIN_HEAT 4  

const double TOLERANCE = 1.0;
const double temperatureTarget = 19; //Celsius degree
const bool SERIAL_OUTPUT_DEBUG = false;

// Sensor objects
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Variables
double temperatureSensor;
unsigned long  lastPrintTime;

/** Function that controls activation or deactivation
    of the relay. The LED built into the Arduino is on when
    the relay is active, otherwise it is off.*/
void relay_state_heat(bool state) {
  digitalWrite(RELAY_PIN_HEAT, state);
  digitalWrite(LED_BUILTIN, state);
}

/** Set To LOW or HIGH the cooling relay
*/
void relay_state_cold(bool state) {
  digitalWrite(RELAY_PIN_COLD, state);
  digitalWrite(LED_BUILTIN, state);
}

/** Set To LOW or HIGH the heating relay
*/
void printTemperatureEverySec(double temp) {
  if ((millis() - lastPrintTime) > 5000) {
    lastPrintTime = millis();
    Serial.println(temp);
  
  }
}


// Setup function that runs once at startup
void setup() {
  // Define relay and led pins as output
  pinMode(RELAY_PIN_HEAT, OUTPUT);
  pinMode(RELAY_PIN_COLD, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Set the relay to low state
  relay_state_heat(LOW);
  relay_state_cold(LOW);

  Serial.begin(9600);
  if (SERIAL_OUTPUT_DEBUG) {
    lastPrintTime = millis();
  }
}


// Main loop function
void loop() {
  // Get the temperature from the sensor
  sensors.requestTemperatures();
  temperatureSensor = sensors.getTempCByIndex(0);
  
  if (SERIAL_OUTPUT_DEBUG) {
    printTemperatureEverySec(temperatureSensor);
  }
  //--------------------------------------------
  // Manage heating function
  //--------------------------------------------
  if (temperatureSensor < temperatureTarget) {
   relay_state_heat(HIGH);
    // Avoid relay histeresis for at least 3 minutes
    for (int i = 0;i < 180;i++) {   
      delay(1000);   
    }  
  } else {
    relay_state_heat(LOW);
  }

  //--------------------------------------------
  // Manage cooling function
  //--------------------------------------------
  if (temperatureSensor > temperatureTarget + TOLERANCE) {
   relay_state_cold(HIGH);
  } else {
    relay_state_cold(LOW);
  }  
}