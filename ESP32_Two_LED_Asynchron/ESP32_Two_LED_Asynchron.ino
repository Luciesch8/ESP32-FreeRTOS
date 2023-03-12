/*
   This blinks two LEDs independently and not synchronized. Both have other blink frequencies.
   The blink sketches run in two tasks and on two cores.
*/

#define LED1 12
#define LED2 14

TaskHandle_t Task1, Task2;

//int counter = 0;

void blink(byte pin, int duration) {

//    digitalWrite(pin, HIGH);
//    delay(duration);
//    digitalWrite(pin, LOW);
//    delay(duration);

  GPIO.out_w1ts = ((uint32_t)1 << pin);
  delay(duration);
  GPIO.out_w1tc = ((uint32_t)1 << pin);
  delay(duration);
}

void codeForTask1( void * parameter )
{
  for (;;) {
    blink(LED1, 1000);
    //delay(50);
    Serial.println("Task 1: ");
  }
}

void codeForTask2( void * parameter )
{
  for (;;) {
    blink(LED2, 1200);
    //delay(50);
    Serial.println("             Task 2: ");
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  // initialize digital pin LED_BUILTIN as an output.

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  xTaskCreatePinnedToCore(
    codeForTask1, /* Task function. */
    "led1Task",   /* name of task. */
    1000,         /* Stack size of task */
    NULL,         /* parameter of the task */
    1,            /* priority of the task */
    &Task1,       /* Task handle to keep track of created task */
    0);           /* pin task to core 0 */
  //delay(10);  // needed to start-up task1

  xTaskCreatePinnedToCore(
    codeForTask2,
    "led2Task",
    1000,
    NULL,
    1,
    &Task2,
    1);

}

void loop() {
  //delay(1000);
}
