#include "WiFiConnect.h" 

// Configuration options
#define LED_1 12
#define LED_2 13
#define LED_3 25
#define LED_4 26
#define MIN_HOUR 5
#define MAX_HOUR 8
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

// Global Vars
WiFiConnect wc;

/**
 * Called for AP Config Mode
 **/
void configModeCallback(WiFiConnect *mWiFiConnect) {
  Serial.println("Entering Access Point");
}

/**
 * Called for printing time
 **/
void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

/**
 * Change all the connected Pin states
 **/
void changeLEDState(int newState){
  digitalWrite(LED_1, newState);
  digitalWrite(LED_2, newState);
  digitalWrite(LED_3, newState);
  digitalWrite(LED_4, newState);
}

/**
 * Initial Startup Setup
 **/
void setup() {
  
  // Setup the Console
  Serial.begin(9600);
  while (!Serial) {
    delay(100);
  }

  // Setup Outputs
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  changeLEDState(HIGH);

  // Setup the WiFi
  wc.setDebug(true);
  wc.setAPCallback(configModeCallback);
    if (!wc.autoConnect()) { // try to connect to wifi
      /* We could also use button etc. to trigger the portal on demand within main loop */
      wc.startConfigurationPortal(AP_LOOP);//if not connected show the configuration portal
    }

  // Get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  // Disconnect from WiFi
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  Serial.println("Setup Complete");
}

/**
 * Main Loop
 **/
void loop() {

  // Show the time for debugging
  printLocalTime();  

  // If we are within the time slot, ensure the output LEDs are on.
  // As we only run this every 10 seconds, we will just call digitalWrite from changeLEDState.
  struct tm timeinfo;
  if(getLocalTime(&timeinfo)){

    int cHour = timeinfo.tm_hour;
    if(cHour >= MIN_HOUR && cHour <= MAX_HOUR){
      changeLEDState(HIGH);
    }else{
      changeLEDState(LOW);
    }

  }else{
    Serial.println("Failed to obtain time for comparison!");
    changeLEDState(HIGH);
    delay(500);
    changeLEDState(LOW);
  }

  // Wait for 10 seconds before reevaluation
  delay(10 * 1000);
}