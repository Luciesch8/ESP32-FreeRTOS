#include "DHTesp.h"

DHTesp dht;
hw_timer_t * timer;
SemaphoreHandle_t syncSemaphore;
int dhtPin = 4;     // Digital pin connected to the DHT22 sensor
int sampleTimer = 5 * 1000000 ; // 5 s

void IRAM_ATTR onTimer() {
  xSemaphoreGiveFromISR(syncSemaphore, NULL);
}

void setup() {
  Serial.begin(115200);
  syncSemaphore = xSemaphoreCreateBinary();
  dht.setup(dhtPin, DHTesp::DHT22);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, sampleTimer, true);
  timerAlarmEnable(timer);
}

void loop() {
  xSemaphoreTake(syncSemaphore, portMAX_DELAY);
  float temperature = dht.getTemperature();
  Serial.print("Temperature: ");
  Serial.println(temperature);
}
