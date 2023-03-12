// ESP32_Queue_TwoCores_sensorTask
// Library : https://github.com/beegee-tokyo/DHTesp 

#include "DHTesp.h"

DHTesp dht;

int dhtPin = 4;     // Digital pin connected to the DHT22 sensor
int nbSensorValues = 2; // Type of values from sensors (temperature, humidity, etc.)

bool initDHT() {
  // Initialize temperature and humidity sensor DHT22
  dht.setup(dhtPin, DHTesp::DHT22);
  Serial.println("DHT initiated");
}

QueueHandle_t queue;
int queueSize = 100;

TaskHandle_t SendTask, ReceiveTask;

static int taskCore = 0; // Core 0 or 1 for sensorSendTask

void sensorSendTask( void * pvParameters ) {
  String taskMessage = "sensorSendTask running on core ";
  taskMessage = taskMessage + xPortGetCoreID();
  float t, h ;

  for (;;) {
    Serial.println(taskMessage);

    TempAndHumidity newValues = dht.getTempAndHumidity();

    // Check if any reads failed and exit early (to try again).
    if (dht.getStatus() != 0) {
      Serial.println("DHT22 error status: " + String(dht.getStatusString()));
    }
    t = newValues.temperature;
    h = newValues.humidity;

    float test[nbSensorValues] = {t, h};
    for ( int i = 0; i < nbSensorValues; i++ )
    {
      xQueueSend(queue, &test[i], portMAX_DELAY);
    }

    delay(1000);
  }
  vTaskDelete(NULL);
}

void sensorReceiveTask( void * pvParameters ) {
  String taskMessage = "sensorReceiveTask running on core ";
  taskMessage = taskMessage + xPortGetCoreID();
  float test[nbSensorValues];

  for (;;) {
    Serial.println(taskMessage);

    for (int i = 0; i < nbSensorValues; i++ ) {
      xQueueReceive(queue, &test[i], portMAX_DELAY);
      Serial.println(test[i]);
    }
  }
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(9600);
  initDHT();

  queue = xQueueCreate( queueSize, sizeof( int ) );

  if (queue == NULL) {
    Serial.println("Error creating the queue");
  }
  Serial.print("Starting to create sensorSendTask on core ");
  Serial.println(taskCore);

  xTaskCreatePinnedToCore(
    sensorSendTask,       // Function to implement the task
    "sensorSendTask",     // Name of the task
    10000,                // Stack size in words
    NULL,                 // Task input parameter
    1,                    // Priority of the task
    &SendTask,            // Task handle
    taskCore);            // Core where the task should run
  Serial.println("SendTask created...");

  xTaskCreatePinnedToCore(
    sensorReceiveTask,    // Function to implement the task
    "sensorReceiveTask",  // Name of the task
    10000,                // Stack size in words
    NULL,                 // Task input parameter
    1,                    // Priority of the task
    &ReceiveTask,         // Task handle
    1);                   // Core where the task should run
  Serial.println("ReceiveTask created...");
}

void loop() {}
