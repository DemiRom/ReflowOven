#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>             // Arduino SPI library
#include <Arduino.h>
#include <max6675.h>
#include <PIDController.h>

/* Temporary hard code, will want to be able to adjust in settings later */
#define __Kp 30 // Proportional constant
#define __Ki 0.7 // Integral Constant
#define __Kd 200 // Derivative Constant

#define TFT_CS    10  // define display chip select pin
#define TFT_DC     9  // define display data/command pin
#define TFT_RST    8  // define display reset pin
#define TC_DO      14 // define thermocouple MISO
#define TC_CS      4  // define thermocouple chip select
#define TC_CLK     15 // define thermocouple CLK 
//TODO Define button inputs here
//TODO Define oven control digital outputs here
#define OVN_ON 	   -1

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
MAX6675 tc = MAX6675(TC_CLK, TC_CS, TC_DO);
PIDController pid; 

void drawTemp();
void drawSetpoint();
void drawElementState();
void drawText(char *text, uint8_t size, uint16_t color);

int setPoint = 60;
bool ovenOn = false; 

void setup(void) {
	Serial.begin(9600);
	Serial.print(F("SMD REFLOW"));

	tft.init(240, 320, SPI_MODE2);    // Init ST7789 display 240x320 pixel
	tft.setRotation(1);

	Serial.println(F("Initialized"));

	uint16_t time = millis();
	tft.fillScreen(ST77XX_BLACK);
	time = millis() - time;

	Serial.println(time, DEC);
	delay(500);

	tft.fillScreen(ST77XX_BLACK);
	drawText("DIY SMD REFLOW STARTING!", 3, ST77XX_WHITE);
	delay(1000);

	tft.fillScreen(ST77XX_BLACK);
	drawText("PID INIT", 2, ST77XX_WHITE);

	pid.begin();          // initialize the PID instance
	pid.setpoint(setPoint);    // The "goal" the PID controller tries to "reach"
	pid.tune(__Kp, __Ki,__Kd);    // Tune the PID, arguments: kP, kI, kD
	pid.limit(0, 255);    // Limit the PID output between 0 and 255, this is important to get rid of integral windup!
	
	delay(4000);
	tft.fillScreen(ST77XX_BLACK);
}

void loop() {
	drawTemp();
	drawSetpoint();
	ovenOn = pid.compute(tc.readCelsius()) > 128;
	//digitalWrite(OVN_ON, ovenOn); TODO
	delay(60);
}

void drawTemp() { 
	tft.setCursor(0,0);
	tft.setTextColor(ST77XX_BLUE);
	tft.setTextSize(4);
	tft.print(tc.readCelsius());
	tft.print(" C");
}

void drawSetpoint() { 
	tft.setCursor(0, 48);
	tft.setTextColor(ST77XX_GREEN); 
	tft.setTextSize(4); 
	tft.print(setPoint);
}

void drawElementState() { 
	tft.fillCircle(20, 96, 10, ovenOn ? ST77XX_RED : ST77XX_BLACK);
}

void drawText(char* text, uint8_t size, uint16_t colour) { 
	tft.setCursor(0,0);
	tft.setTextColor(colour);
	tft.setTextSize(size);
	tft.print(text);
}