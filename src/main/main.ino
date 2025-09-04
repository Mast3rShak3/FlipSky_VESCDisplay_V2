#include <FlickerFreePrint.h> // Flicker-free printing to TFT
#include <ComEVesc.h> // Communication with VESC controller
#include <TFT_eSPI.h> // Hardware-specific library for TFT display
#include <SPI.h>
#include "EEPROMAnything.h" // Easy access to EEPROM storage
#include "JerseyM54_82pt7b.h"
#include "JerseyM54_18pt7b.h"
#include "JerseyM54_14pt7b.h"
#include "Blockletter8pt7b.h"

// Global variables for tracking speed, distance, and motor data
float trip_miles;
float startup_total_miles;
float last_total_miles_stored;
float total_miles;
float tacho;
float rpm;
float speed_mph;
float watts;
float wheel_diameter;
int maxspeed_mph;
int brightness = 255;
char fmt[10];

// Static test data variables - SET TO TRUE TO USE TEST DATA
bool useStaticData = true; // Change to false when VESC is connected
float staticSpeedMPH = 16.0; // MPH
float staticRPM = 2450.0;
float staticVoltage = 42.3;
float staticCurrent = 8.5;
float staticPower = 359.55;
float staticTempVesc = 45.2;
float staticTempMotor = 65.8;
float staticBatteryPercent = 78.0;
float staticMotorCurrent = 12.3;
float staticInputCurrent = 8.1;
float staticError = 0; // 0 = no error

// Font settings for various display elements
#define SPEEDFONT &Jersey_M5482pt7b // Large font for displaying speed
#define DATAFONTSMALL2 &Jersey_M5414pt7b // Small font for other data values
#define DATAFONTSMALL &Jersey_M5418pt7b // Alternative small font
#define DATAFONTSMALLTEXT &Blockletter8pt7b // Font for smaller text labels

// Motor and wheel parameters for speed calculation
#define MOTOR_POLES 30 // Number of motor poles (30 for typical E-scooters)
#define WHEEL_DIAMETER_MM 246 // Diameter of the wheel in millimeters
#define WHEEL_DIAMETER_INCHES 9.685 // Diameter in inches (246mm = 9.685 inches)
#define GEAR_RAITO 1.0 // Motor pulley teeth count (1:1 gearing)
#define PI 3.141592 // Pi constant
#define SCONST_MPH 0.0746 // Conversion factor from RPM to speed (MPH) - calculated for direct MPH

// Pin configs for the hardware

#define RXD2 22 // DISPLAY RX 22 TO VESC TX
#define TXD2 27 // DISPLAY TX 27 TO VESC RX

#define LDR_PIN 34 // Light Detecting Resistor that sits on top right of the display
#define LCD_BACK_LIGHT_PIN 21

// User-configurable settings for warnings and thresholds
int EEPROM_MAGIC_VALUE = 0; // EEPROM magic value to track saved data
#define EEPROM_UPDATE_EACH_MILES 0.062 // Frequency of EEPROM updates (every 0.062 miles)

int COLOR_WARNING_SPEED = TFT_RED; // Color for speed warning display
#define HIGH_SPEED_WARNING_MPH 37 // Speed threshold for warnings (37 MPH)

int COLOR_WARNING_TEMP_VESC = TFT_WHITE; // Color for VESC temperature warnings
#define VESC_TEMP_WARNING1 50 // First temperature warning threshold (50°C)
#define VESC_TEMP_WARNING2 80 // Second temperature warning threshold (80°C)

int COLOR_WARNING_TEMP_MOTOR = TFT_WHITE; // Color for motor temperature warnings
#define MOTOR_TEMP_WARNING1 80 // First motor temperature warning threshold (80°C)
#define MOTOR_TEMP_WARNING2 120 // Second motor temperature warning threshold (120°C)

int BATTERY_WARNING_COLOR = TFT_WHITE; // Color for battery voltage warnings
#define BATTERY_WARNING_HIGH 67.2 // High voltage warning threshold (67.2V)
#define BATTERY_WARNING_LOW 54.4 // Low voltage warning threshold (54.4V)
#define BATTERY_WARNING_0 48.0 // Voltage Cut warning treshold(48V)

int ERROR_WARNING_COLOR = TFT_WHITE; // Color for error warnings

#define DO_LOGO_DRAW // Uncomment if you want enable startup logo and background logo [Currently disbaled version doesn't work so don't disable!]
#define DEBUG_MODE

#ifdef DO_LOGO_DRAW
#include <PNGdec.h> // PNG decoder library
#include "startup_image.h"// PNG data for startup logo
#include "background_image.h" // PNG data for background image
PNG png; // PNG decoder instance
int16_t xpos = 0; // X position for image drawing
int16_t ypos = 0; // Y position for image drawing
#define MAX_IMAGE_WDITH 320 // Maximum image width for display
#endif

// Other settings
int Screen_refresh_delay = 50; // Delay between screen refreshes (ms)
ComEVesc UART; // VESC UART instance
HardwareSerial VescSerial(1);


TFT_eSPI tft = TFT_eSPI(); 
FlickerFreePrint<TFT_eSPI> Data1(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data2(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data3(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data4(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data5(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data6(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data7(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data8(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data9(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data10(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data11(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data1t(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data2t(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data3t(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data4t(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data5t(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data6t(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data7t(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data8t(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data9t(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data10t(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Data11t(&tft, TFT_WHITE, TFT_BLACK);

void pngDraw(PNGDRAW * pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WDITH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(xpos, ypos + pDraw -> y, pDraw -> iWidth, 1, lineBuffer);
}

void checkMileageValues() {
  total_miles = startup_total_miles + trip_miles;
  bool traveled_enough_distance = (total_miles - last_total_miles_stored >= EEPROM_UPDATE_EACH_MILES);
  if (traveled_enough_distance) {
    last_total_miles_stored = total_miles;
    EEPROM_writeAnything(EEPROM_MAGIC_VALUE, total_miles);
  }
}

void setup(void) {
  VescSerial.begin(115200, SERIAL_8N1, RXD2, TXD2); // VESC RX TX
  // Wait for the Serial port to be ready
  while (!VescSerial) {
    delay(10);
  }
  UART.setSerialPort(&VescSerial);
  #ifdef DEBUG_MODE
  Serial.begin(115200); // Debug MicroUSB ?
  UART.setDebugPort(&Serial);
  #endif
  tft.begin();
  EEPROM.begin(100);
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  
  if (!useStaticData) {
    UART.getVescValues();
  }
  
  EEPROM_readAnything(EEPROM_MAGIC_VALUE, startup_total_miles);
  if (isnan(startup_total_miles)) {
    tft.setCursor(40, 160);
    tft.setTextFont(4);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.print("SETUP EPROM...");
    for (int i = 0; i <100; i++){
	    EEPROM_writeAnything(i,0);
    }
    EEPROM_writeAnything(EEPROM_MAGIC_VALUE,0.0);
    delay(1500);
    ESP.restart();
  }
  last_total_miles_stored = startup_total_miles;
  
  if (useStaticData) {
    tacho = 25472; // Static tacho value (corresponds to 16 miles in tacho units)
    trip_miles = 9.8; // Static trip value in miles
  } else {
    tacho = (UART.data.tachometerAbs / (MOTOR_POLES * 3));
    trip_miles = (tacho / 1000) * 0.621371; // Convert km to miles
  }
  
  if (startup_total_miles != 0) {
    startup_total_miles = startup_total_miles - trip_miles;
  }

  #ifdef DO_LOGO_DRAW
  int16_t rc_bg = png.openFLASH((uint8_t * ) startup_image, sizeof(startup_image), pngDraw);
  if (rc_bg == PNG_SUCCESS) {
    tft.startWrite();
    rc_bg = png.decode(NULL, 0);
    tft.endWrite();
  }
  png.close();
  delay(3000);
  tft.fillScreen(TFT_BLACK);
  int16_t rc_mainbg = png.openFLASH((uint8_t * ) background_image, sizeof(background_image), pngDraw);
  if (rc_mainbg == PNG_SUCCESS) {
    tft.startWrite();
    rc_mainbg = png.decode(NULL, 0);
    tft.endWrite();
  }
  png.close();
  #endif
}

void loop() {
  // Static data simulation or real VESC data
  if (useStaticData) {
    // Simulate dynamic changes for realistic testing
    staticSpeedMPH += (random(-50, 50) / 100.0);
    if (staticSpeedMPH < 0) staticSpeedMPH = 0;
    if (staticSpeedMPH > 31) staticSpeedMPH = 31; // Max ~31 MPH
    
    staticRPM = staticSpeedMPH * 97.8;
    
    staticCurrent += (random(-20, 20) / 100.0);
    if (staticCurrent < 0) staticCurrent = 0;
    if (staticCurrent > 15) staticCurrent = 15;
    
    staticPower = staticVoltage * staticCurrent;
    
    staticBatteryPercent -= 0.001;
    if (staticBatteryPercent < 20) staticBatteryPercent = 95;
    
    // Use static values for calculations
    speed_mph = staticSpeedMPH;
    rpm = staticRPM;
    watts = staticPower;
    
  } else {
    // VESC communication with direct MPH calculation
    UART.getVescValues();
    tacho = (UART.data.tachometerAbs / (MOTOR_POLES * 3));
    rpm = (UART.data.rpm / (MOTOR_POLES / 2));
    trip_miles = (tacho / 1000) * 0.621371; // Convert km to miles
    
    // Direct MPH calculation using wheel diameter in inches
    float wheel_circumference_inches = PI * WHEEL_DIAMETER_INCHES;
    float wheel_circumference_miles = wheel_circumference_inches / (12 * 5280); // Convert inches to miles
    speed_mph = (rpm * wheel_circumference_miles * GEAR_RAITO) * 60; // RPM * circumference * 60 min/hr = MPH
    
    watts = UART.data.inpVoltage*UART.data.avgInputCurrent;
  }
  
  int sensorValue = analogRead(LDR_PIN);
  if(sensorValue <= 200){sensorValue = 200;}
  brightness = map(sensorValue, 200, 1000, 255, 25);
  if(brightness <= 25){brightness=25;} //stupid map function gets destroyed when input value goes over specified 
  analogWrite(LCD_BACK_LIGHT_PIN, brightness);

  //Main Speed Display (MPH) --------------------------------------------------------------------------

  int speedINT = _max(speed_mph, 0);
  if (speedINT > HIGH_SPEED_WARNING_MPH) {
    COLOR_WARNING_SPEED = TFT_RED;
  } else {
    COLOR_WARNING_SPEED = TFT_WHITE;
  }
  Data1.setTextColor(TFT_BLACK, TFT_BLACK);
  
  tft.setFreeFont(SPEEDFONT);
  tft.setCursor(83, 162);
  Data1.setTextColor(COLOR_WARNING_SPEED, TFT_BLACK);
  Data1.print(speedINT);

  //Vesc Temperature Display --------------------------------------------------------------------------
  
  float currentTempVesc = useStaticData ? staticTempVesc : UART.data.tempMosfet;

  if (currentTempVesc > VESC_TEMP_WARNING1) {
    COLOR_WARNING_TEMP_VESC = TFT_YELLOW;
  }
  if (currentTempVesc > VESC_TEMP_WARNING2) {
    COLOR_WARNING_TEMP_VESC = TFT_RED;
  } else {
    COLOR_WARNING_TEMP_VESC = TFT_GREEN;
  }

    tft.setCursor(5, 220);
    tft.setFreeFont(DATAFONTSMALL);
    Data2.setTextColor(COLOR_WARNING_TEMP_VESC, TFT_BLACK);
    dtostrf(currentTempVesc, 3, 0, fmt);
    Data2.print(fmt);

    tft.setCursor(15, 235);
    tft.setFreeFont(DATAFONTSMALLTEXT);
    Data2t.setTextColor(TFT_WHITE, TFT_BLACK);
    Data2t.print("VescT"); 
  
  //Motor Temperature Display --------------------------------------------------------------------------
  
  float currentTempMotor = useStaticData ? staticTempMotor : UART.data.tempMotor;

  if (currentTempMotor > MOTOR_TEMP_WARNING1) {
    COLOR_WARNING_TEMP_MOTOR = TFT_YELLOW;
  }
  if (currentTempMotor > MOTOR_TEMP_WARNING2) {
    COLOR_WARNING_TEMP_MOTOR = TFT_RED;
  } else {
    COLOR_WARNING_TEMP_MOTOR = TFT_GREEN;
  }
  
    tft.setCursor(65, 220);
    tft.setFreeFont(DATAFONTSMALL);
    Data11.setTextColor(COLOR_WARNING_TEMP_MOTOR, TFT_BLACK);
    dtostrf(currentTempMotor, 3, 0, fmt);
    Data11.print(fmt);

    tft.setCursor(65, 235);
    tft.setFreeFont(DATAFONTSMALLTEXT);
    Data11t.setTextColor(TFT_WHITE, TFT_BLACK);
    Data11t.print("MotorT"); 

  //Battery Voltage Display --------------------------------------------------------------------------
  
  float currentVoltage = useStaticData ? staticVoltage : UART.data.inpVoltage;

  if (currentVoltage > BATTERY_WARNING_HIGH) {
    BATTERY_WARNING_COLOR = TFT_RED;
  } else if (currentVoltage < BATTERY_WARNING_0) {
    BATTERY_WARNING_COLOR = TFT_RED;
  } else if (currentVoltage < BATTERY_WARNING_LOW) {
    BATTERY_WARNING_COLOR = TFT_YELLOW;
  } else if (BATTERY_WARNING_LOW < currentVoltage < BATTERY_WARNING_HIGH) {
    BATTERY_WARNING_COLOR = TFT_GREEN;
  }

  tft.setFreeFont(DATAFONTSMALL2);
  Data4.setTextColor(BATTERY_WARNING_COLOR, TFT_BLACK);
  tft.setCursor(10, 25);
  dtostrf(currentVoltage, 3, 1, fmt);
  Data4.print(fmt);

  tft.setCursor(10, 30);
  tft.setTextFont(1);
  Data4t.setTextColor(TFT_WHITE, TFT_BLACK);
  Data4t.print("Battery");

    //Watt/Error Display --------------------------------------------------------------------------
    
  float currentError = useStaticData ? staticError : UART.data.error;

  if (currentError == 0){ // Display Watts when no error(0)
	  tft.setFreeFont(DATAFONTSMALL2);
	  Data10.setTextColor(TFT_WHITE, TFT_BLACK);
	  tft.setCursor(260, 25);
	  dtostrf(watts, 5, 0, fmt);
	  Data10.print(fmt);
	
	  tft.setCursor	(270, 30);
	  tft.setTextFont(1);
	  Data10t.setTextColor(TFT_WHITE, TFT_BLACK);
	  Data10t.print("Watts");
  }else{ //	Error display when present
	  if (currentError > 0) {
	  	ERROR_WARNING_COLOR = TFT_RED;
	  }else{
	    ERROR_WARNING_COLOR = TFT_GREEN;
	  }
	  tft.setFreeFont(DATAFONTSMALL2);
	  Data10.setTextColor(ERROR_WARNING_COLOR, TFT_BLACK);
	  tft.setCursor(270, 25);
	  dtostrf(currentError, 2, 0, fmt);
	  Data10.print(fmt);
				
	  tft.setCursor(270, 30);
	  tft.setTextFont(1);
	  Data10t.setTextColor(TFT_WHITE, TFT_BLACK);
	  Data10t.print("Error");
  }

  //Motor-Phase Current Display --------------------------------------------------------------------------
  
  float currentMotorCurrent = useStaticData ? staticMotorCurrent : UART.data.avgMotorCurrent;

  tft.setCursor(270, 220);
  tft.setFreeFont(DATAFONTSMALL);
  Data6.setTextColor(TFT_GREEN, TFT_BLACK);
  dtostrf(currentMotorCurrent, 3, 0, fmt);
  Data6.print(fmt);

  tft.setCursor(265, 235);
  tft.setFreeFont(DATAFONTSMALLTEXT);
  Data6t.setTextColor(TFT_WHITE, TFT_BLACK);
  Data6t.print("PHASE A");
  
  //Battery Current Display --------------------------------------------------------------------------
  
  float currentInputCurrent = useStaticData ? staticInputCurrent : UART.data.avgInputCurrent;

  tft.setCursor(220, 220);
  tft.setFreeFont(DATAFONTSMALL);
  Data7.setTextColor(TFT_GREEN, TFT_BLACK);
  dtostrf(currentInputCurrent, 3, 0, fmt);
  Data7.print(fmt);

  tft.setCursor(215, 235);
  tft.setFreeFont(DATAFONTSMALLTEXT);
  Data7t.setTextColor(TFT_WHITE, TFT_BLACK);
  Data7t.print("BATT A");

  //Odometer Display (Miles) --------------------------------------------------------------------------

  tft.setCursor(145, 220);
  tft.setFreeFont(DATAFONTSMALL);

  Data9.setTextColor(TFT_WHITE, TFT_BLACK);
  dtostrf(total_miles, 4, 0, fmt);
  Data9.print(fmt);

  tft.setCursor(135, 235);
  tft.setFreeFont(DATAFONTSMALLTEXT);
  Data9t.setTextColor(TFT_WHITE, TFT_BLACK);
  Data9t.print("MILES");
  
  delay(Screen_refresh_delay);
  checkMileageValues();
}