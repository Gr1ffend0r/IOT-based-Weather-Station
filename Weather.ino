#include "LCD_Routine.h"

// Pin definitions
#define HUMIDITY_PIN A1         // Pin for Humidity sensor HIH 5030 by Honeywell
#define TEMPERATURE_PIN A0      // Pin for Temperature sensor MPC9700A by Microchip
#define O2_PIN A6               // Pin for O2 sensor
#define PRESSURE_PIN A2         // Pin for Atmospheric Pressure sensor
#define SOLAR_PIN A9            // Pin for Solar Radiation sensor
#define UVOUT A3                // Output from ML8511 UV sensor
#define REF_3V3 A4              // 3.3V power reference
#define VRefer 5                // Voltage of ADC reference

// Variables for temperature and humidity
int sens_hum_val;
int sens_temp_val;
float hum_volts;
float RH;
float temp_volts;
float mili_temp_volts;
float temp_cel;
float temp_cel_avg;
const int numReadings = 15;
int readings[numReadings];
int index = 0;

// Variables for O2 sensor
float Vout = 0;

// Variables for atmospheric pressure
float mmHg = 0;

// Variables for solar radiation
int solarRad = 0;

// Variables for UV intensity
float uvIntensity = 0;
float uvVoltage = 0;
int uvLevel = 0;
int refLevel = 0;

// State management
int displayState = 0; // 0: Temp & Humidity, 1: O2 levels, 2: Atmospheric Pressure, 3: Solar Radiation, 4: UV Intensity

void setup() {
  Serial.begin(9600);

  // LCD initialization
  DDRB |=  0xFF;                
  PORTB |= 0xFF;                
  DDRL = 0xFF;                  
  PORTL = 0xFF;                 
  LCD_Init();

  // Clear display and show intro message
  DrawLCD2(0, 128, 0, 159, Black);   
  FontDisp16(1, 0, 1, "Sensor System", Black, White, 12); 

  // Initialize readings array
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

void loop() {
  if (displayState == 0) {
    // Temperature and Humidity
    sens_hum_val = analogRead(HUMIDITY_PIN);                       
    hum_volts = sens_hum_val * (4.7 / 1023.0);                     
    RH = (((hum_volts * 2.0) / 4.7) - 0.1515) * 157.23;            

    sens_temp_val = analogRead(TEMPERATURE_PIN);                  
    temp_volts = sens_temp_val * (4.7 / 1023);                    
    mili_temp_volts = temp_volts * 1000 * 2;                      
    temp_cel = (mili_temp_volts - 500) / 10;                       

    readings[index] = temp_cel;
    index = (index + 1) % numReadings;

    temp_cel_avg = 0;
    for (int i = 0; i < numReadings; i++) {
      temp_cel_avg += readings[i];
    }
    temp_cel_avg /= numReadings;

    Serial.print("Humidity: ");
    Serial.print(RH);
    Serial.print(" %, Temperature: ");
    Serial.print(temp_cel_avg);
    Serial.println(" °C");

    DrawLCD2(0, 128, 0, 159, Black);   
    FontDisp16(1, 0, 1, "Temp &", Black, White, 13);
    FontDisp16(1, 0, 2, "    Humidity", Black, White, 13);
    FontDisp16(1, 0, 3, ("Humidity: " + String(RH) + " %").c_str(), Black, Red, 12);
    FontDisp16(1, 0, 4, ("Temp: " + String(temp_cel_avg) + " C").c_str(), Black, Red, 12);
  } else if (displayState == 1) {
    // O2 Levels
    Vout = readO2Vout();
    float concentration = readConcentration();

    Serial.print("O2 Vout: ");
    Serial.print(Vout);
    Serial.print(" V, Concentration: ");
    Serial.print(concentration);
    Serial.println(" %");

    DrawLCD2(0, 128, 0, 159, Black);   
    FontDisp16(1, 0, 1, "O2 Sensor Data", Black, White, 12);
    FontDisp16(1, 0, 2, ("Concentration: " + String(concentration) + " %").c_str(), Black, Red, 13);
  } else if (displayState == 2) {
    // Atmospheric Pressure
    mmHg = readPressure(PRESSURE_PIN);

    Serial.print("Atmospheric Pressure(mmHg): ");
    Serial.println(mmHg);

    DrawLCD2(0, 128, 0, 159, Black);   
    FontDisp16(1, 0, 1, "Atmospheric Data", Black, White, 13);
    FontDisp16(1, 0, 2, ("Pressure: " + String(mmHg) + " mmHg").c_str(), Black, Red, 13);
  } else if (displayState == 3) {
    // Solar Radiation
    solarRad = map(analogRead(SOLAR_PIN), 0, 1023, 0, 5000);

    Serial.print("Solar Radiation: ");
    Serial.print(solarRad);
    Serial.println(" Lux");

    DrawLCD2(0, 128, 0, 159, Black);   
    FontDisp16(1, 0, 1, "Solar Radiation", Black, White, 13);
    FontDisp16(1, 0, 2, ("Value: " + String(solarRad) + " Lux").c_str(), Black, Red, 13);
  } else if (displayState == 4) {
    // UV Intensity
    uvLevel = averageAnalogRead(UVOUT);
    refLevel = averageAnalogRead(REF_3V3);
    uvVoltage = 3.3 / refLevel * uvLevel;
    uvIntensity = mapfloat(uvVoltage, 0.99, 2.8, 0.0, 15.0);

    Serial.print("UV Intensity: ");
    Serial.print(uvIntensity);
    Serial.println(" mW/cm^2");

    DrawLCD2(0, 128, 0, 159, Black);   
    FontDisp16(1, 0, 1, "UV Sensor", Black, White, 13);
    FontDisp16(1, 0, 2, ("Intensity: " + String(uvIntensity) + " mW/cm^2").c_str(), Black, Red, 13);
  }

  displayState = (displayState + 1) % 5; // Cycle through states (0 to 4)
  delay(2000); // Delay before switching to the next display
}

// O2 sensor readings
float readO2Vout() {
  long sum = 0;
  for (int i = 0; i < 32; i++) {
    sum += analogRead(O2_PIN);
  }
  sum >>= 5;

  return sum * (VRefer / 1023.0);
}

float readConcentration() {
  float MeasuredVout = readO2Vout();
  return (MeasuredVout * 0.21 / 2.0) * 100;
}

// Atmospheric pressure readings
float readPressure(int pin) {
  int pressureValue = analogRead(pin);
  float pressure = ((pressureValue / 1024.0) + 0.095) / 0.00009;
  return pressure / 180;
}

// UV sensor utilities
int averageAnalogRead(int pinToRead) {
  byte numberOfReadings = 8;
  unsigned int runningValue = 0;

  for (int x = 0; x < numberOfReadings; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return runningValue;
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}