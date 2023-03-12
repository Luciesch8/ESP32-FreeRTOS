// ESP32_Blinked_Task

uint8_t led_pin = GPIO_NUM_2;   // LED_BUILIN = 2 for NodeMCU ESP32

// define taskBlinkled TaskHandle_t variable
TaskHandle_t taskBlinkled;

void blinkLed(void* param) {
  // Block for 1000 ms or 1 ec.s
  const TickType_t xFrequency = 1000;
  // Initialise the xLastWakeTime variable with the current time.
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    static uint32_t pin_val = 0;
    // toggle the pin value
    pin_val ^= 1;   // exclusive-OR (XOR) operation
    //the code below instead of function digitalWrite(led_pin, pin_val) because more fast;
    if (pin_val) {
      GPIO.out_w1ts = ((uint32_t)1 << led_pin);
    }
    else {
      GPIO.out_w1tc = ((uint32_t)1 << led_pin);
    }
    Serial.printf("Led ---- %s\n" , pin_val ? "On" : "Off");
    // Simply toggle the LED every 1000ms or 1 sec., Wait for the next cycle.
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
  }
  taskBlinkled = NULL;
  vTaskDelete( NULL );
}

void setup() {
  // put your setup code here, to run once:
  // set the serial port at 115200
  Serial.begin(115200);
  // set the Led pin as ouput
  pinMode(led_pin, OUTPUT);
  //create a task that will be executed in the blinkLed() function, with priority 1
  xTaskCreate(
    blinkLed,         /* Task function. */
    "blinkLed",       /* name of task. */
    1024 * 2,         /* Stack size of task */
    NULL,             /* parameter of the task */
    1,                /* priority of the task */
    &taskBlinkled);   /* Task handle to keep track of created task */
}

void loop() {}
