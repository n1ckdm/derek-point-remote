#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); 
const byte address[6] = "01101";

int button_pin = 2;
boolean point_state = true;

void send_point_state()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 400) 
  {
    point_state = !point_state;
    uint16_t point_anlgle =  point_state ? 70 : 108 ;
    Serial.print("Sending point state: ");
    Serial.println(point_anlgle);
    radio.write(&point_anlgle, sizeof(point_anlgle));
  }
  last_interrupt_time = interrupt_time;

}

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting remote...");

  // Attach the interrupt to the button pin
  pinMode(button_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(button_pin), send_point_state, RISING);

  // Create the radio channel
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);

  // No need to listen as we're just transmitting
  radio.stopListening();
}


void loop()
{
  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('\n');
    if (command == "change\n")
    {
      send_point_state();
    }
  }
}