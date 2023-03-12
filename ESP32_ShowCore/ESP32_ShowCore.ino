#define LED_BUILIN 2

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  Serial.print("This Task runs on Core: ");
  Serial.println(xPortGetCoreID());

  digitalWrite(LED_BUILIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
