// ESP32_BinarySemaphore_TwoTasks
// library : https://github.com/beegee-tokyo/DHTesp

#include "DHTesp.h"

DHTesp dht;
hw_timer_t * timer;
SemaphoreHandle_t syncSemaphore;
int dhtPin = 4;                 // Digital pin connected to the DHT22 sensor
int sampleTimer = 5 * 1000000 ; // 5 seconds
int nbSensorValues = 2;         // Type of values from sensors (temperature, humidity, etc.)

void IRAM_ATTR onTimer() {
  xSemaphoreGiveFromISR(syncSemaphore, NULL);
}

bool initDHT() {
  dht.setup(dhtPin, DHTesp::DHT22); // Initialize DHT22 sensor
  Serial.println("DHT initiated");
}

void timerTask( void * pvParameters ) {
  timer = timerBegin(0, 80, true);    // 1 tick takes 1/(80MHZ/80)   1 microsecond
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, sampleTimer, true);
  timerAlarmEnable(timer);
  vTaskDelete(NULL);
}

void sensorTask( void * pvParameters ) {
  String taskMessage = "sensorTask running on core ";
  taskMessage = taskMessage + xPortGetCoreID();
  float t, h ;

  for (;;) {
    Serial.println(taskMessage);
    TempAndHumidity newValues = dht.getTempAndHumidity();

    // Check if any reads failed and exit early (to try again).
    if (dht.getStatus() != 0) {
      Serial.println("DHT22 error status: " + String(dht.getStatusString()));
    }
    t = newValues.temperature;                    // temperature
    h = newValues.humidity;                       // humidity
    char *label[2] = {"Temperature", "Humidity"}; // Table for Serial.print()
    float test[nbSensorValues] = {t, h};
    for ( int i = 0; i < nbSensorValues; i++ )
    {
      Serial.printf("%s", label[i]);
      Serial.println(" : " + String(test[i]));
    }
    xSemaphoreTake(syncSemaphore, portMAX_DELAY);
  }
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  
  xTaskCreatePinnedToCore(
    sensorTask,       // Function to implement the task
    "sensorTask",     // Name of the task
    10000,            // Stack size in words
    NULL,             // Task input parameter
    1,                // Priority of the task
    NULL,             // Task handle
    0);               // Core where the task should run
  Serial.println("sensorTask created...");

  xTaskCreate(
    timerTask,       // Function to implement the task
    "timerTask",     // Name of the task
    10000,            // Stack size in words
    NULL,             // Task input parameter
    1,                // Priority of the task
    NULL);            // Task handle

  syncSemaphore = xSemaphoreCreateBinary();
  initDHT();
  void timerTask(void * pvParameters);
}

void loop() {}
