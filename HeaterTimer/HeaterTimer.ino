
// Import required libraries
#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <aREST.h>
#include <avr/wdt.h>

#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV2);
aREST rest = aREST();
                                       
#define WLAN_SSID       "{NETWORK_SSID}"
#define WLAN_PASS       "{WIFI_PASSWORD}"
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define LISTEN_PORT           80

// Server instance
Adafruit_CC3000_Server restServer(LISTEN_PORT);

const int powerPin = 7;
const int ledPin = 6;
const int sensorPin = 0;
float temp;
int currPowerPinState = LOW;

int temperature;
int working;
int highTemp;

void setup(void)
{  
  pinMode(powerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  // Init variables and expose them to REST API
  temperature = 24;
  highTemp = 65;
  working = 1;
  rest.variable("temperature",&temperature);
  rest.variable("working",&working);
  rest.variable("state", &currPowerPinState);
  rest.variable("highTemp", &highTemp);
  // Function to be exposed
  rest.function("power",powerControl);
  rest.function("workingControl", workingControl);
  rest.function("tempControl", tempControl);
  
  rest.set_id("008");
  rest.set_name("OfficeHeat");
  
  if (!cc3000.begin())
  {
    while(1);
  }
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    while(1);
  }
  while (!cc3000.checkDHCP())
  {
    delay(100);
  }
     
  restServer.begin();

  wdt_enable(WDTO_8S);
}

void loop() {
  int sensorVal = analogRead(sensorPin);

  float voltage = (sensorVal/1024.0) * 5.0;  
  temp = (((voltage - .5) * 100) * 1.8) + 32;
  temperature = round(temp);
  
  if (working)
  {
    // if the temp is greater than the high temp turn off the power
    if (round(temp) > highTemp)
    {
      currPowerPinState = LOW;
      digitalWrite(powerPin, LOW);
      digitalWrite(ledPin, LOW);
    }
    else if ((currPowerPinState == LOW) && (round(temp) <= (highTemp - 2)))
    {
      currPowerPinState = HIGH;
      digitalWrite(powerPin, HIGH);
      digitalWrite(ledPin, HIGH);
    } 
  }
  else if (!working && (currPowerPinState == HIGH))
  {
     currPowerPinState = LOW;
     digitalWrite(powerPin, LOW);
     digitalWrite(ledPin, LOW);     
  }
  // Handle REST calls
  Adafruit_CC3000_ClientRef client = restServer.available();
  rest.handle(client);
  wdt_reset();

  // Check connection & reset if connection is lost
  if(!cc3000.checkConnected()){while(1){}}
  wdt_reset();
 
}
int powerControl(String command)
{
  int state = command.toInt();
  working = state;
  currPowerPinState = state;
  digitalWrite(ledPin, state);
  digitalWrite(powerPin, state);

  return state;  
}

int workingControl(String command)
{
  int state = command.toInt();

  working = state;

  return state;  
}

int tempControl(String command)
{
  int state = command.toInt();

  highTemp = state;

  return state;  
}
