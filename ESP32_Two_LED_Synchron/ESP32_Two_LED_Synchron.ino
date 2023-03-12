/*
   This sketch uses semaphores to synchronize two LEDs. The blink sketches run in two tasks and on two cores.
   As shown in the video you can either blink them sequencially or in parallel
*/
#define LED1 12
#define LED2 14

TaskHandle_t Task1, Task2;
SemaphoreHandle_t baton;

int counter = 0;

void blink(byte pin, int duration) {

  //    digitalWrite(pin, HIGH);
  GPIO.out_w1ts = ((uint32_t)1 << pin);
  delay(duration);
  //    digitalWrite(pin, LOW);
  GPIO.out_w1tc = ((uint32_t)1 << pin);
  delay(duration);
}

void codeForTask1( void * parameter )
{
  for (;;) {
    xSemaphoreTake( baton, portMAX_DELAY );
    blink(LED1, 1000);
    xSemaphoreGive( baton );
    delay(50);
    Serial.print("Counter in Task 1: ");
    Serial.println(counter);
    counter++;
  }
}

void codeForTask2( void * parameter )
{
  for (;;) {
    xSemaphoreTake( baton, portMAX_DELAY );
    //xSemaphoreGive( baton );
    blink(LED2, 1000);
    xSemaphoreGive( baton );
    delay(50);
    Serial.print("                            Counter in Task 2: ");
    Serial.println(counter);
    counter++;
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  // initialize digital pin LED as an output.
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  baton = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(
    codeForTask1,
    "led1Task",
    1000,
    NULL,
    1,
    &Task1,
    0);
  //delay(500);  // needed to start-up task1

  xTaskCreatePinnedToCore(
    codeForTask2,
    "led2Task",
    1000,
    NULL,
    1,
    &Task2,
    1);

}

void loop() {}
