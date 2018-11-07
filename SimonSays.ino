/*Simon Says game. Now with sound effects. 

Originaly made by Robert Spann
Code trimmed and sound effects added by digimike
more code cleanup and button expansion by Chris Weiss

Buttons are to be set on there designated pins without pull down resistors
and connected to ground rather then +5. 
*/
#include <Tone.h>
#include "Adafruit_TLC5947.h"

// How many boards do you have chained?
#define NUM_TLC5974 1

#define data   5
#define clock   6
#define latch   7
#define oe  -1  // set to -1 to not use the enable pin (its optional)

Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5974, clock, data, latch);


Tone sounder;
uint16_t startTune[] = {NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_F4, NOTE_G4};
uint16_t startDuration[] = {100, 200, 100, 200, 100, 400, 100, 100, 100, 100, 200, 100, 500};
byte startNotes = 5;
uint16_t startNote[] = {NOTE_B4, NOTE_C4, NOTE_E4, NOTE_F4, NOTE_G4};

byte introNotes = 5;
uint16_t introNote[] = {NOTE_G4,NOTE_A4,NOTE_F4,NOTE_F3, NOTE_C4};
uint16_t duration[] = {400, 400, 400, 400, 400, 400};

int gameButton[] = {10, 9, 8, 12, 11}; //The four button input pins
byte gameNotes = 5;
uint16_t gameNote[] = {NOTE_F2, NOTE_A3,NOTE_C3,NOTE_F3,NOTE_G3};

int turnNumber = 0;  // turn counter
int topscore = 0;
byte turnButton[100]; //Intentionally long to store up to 100 inputs (doubtful anyone will get this far)


void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting");
  sounder.begin(4); // speaker is on pin 4
  tlc.begin();

  for(byte x=0; x<gameNotes; x++)
  {
    pinMode(gameButton[x], INPUT_PULLUP);  // button pins are inputs
  }

  randomSeed(analogRead(0)); //Added to generate "more randomness" with the turnButton for the output function
  
  for(byte thisNote = 0; thisNote < 13; thisNote ++) {
    // play the next note:
    sounder.play(startTune[thisNote]);
    // hold the note:
    Serial.println(startuptone2pin(startTune[thisNote]));
    setLED(startuptone2pin(startTune[thisNote]));
    delay(startDuration[thisNote]);
    // stop for the next note:
    sounder.stop();
    setLED(9);
    delay(25);
  }
  delay(1000);
  Serial.println("Ready");
}
 
void loop() 
{   
  for (int y=0; y<=99; y++)
  {
    for(int x=0; x<gameNotes; x++)  // LED pins are outputs
    {
      setLED(x);
    }
    sounder.play(NOTE_C4, 300);
    delay(200);
    setLED(9);
    sounder.play(NOTE_G4, 300);

    delay(1000);
  
    for (int t=turnNumber; t <= turnNumber; t++)
    { //Limited by the turnNumber variable
      Serial.println(""); //Some serial output to follow along
      Serial.print("Turn: ");
      Serial.print(t);
      Serial.println("");
      turnButton[t] = random(0, gameNotes); //Assigning a random number (0-(gameNotes-1)) to the turnButton[y], y being the turn count
      for (int x=0; x <= turnNumber; x++)
      {
        Serial.print(turnButton[x]);
      
        setLED(turnButton[x]);
        sounder.play(gameNote[turnButton[x]], 100);
        delay(400);
        setLED(9);
        delay(100);
      }
    }
    input();
    topscore = turnNumber-1;
  }
}
 
 
 
void input() { //Function for allowing user input and checking input against the generated array
  byte buttonState = 0;  // button state checker
  byte inputArray[100];  
  
//  Serial.println("Getting input:");
//  Serial.println("12345");
  for (int x=0; x <= turnNumber;)
  { //Statement controlled by turn count
    
    for(int y=0; y<gameNotes; y++)
    {
      buttonState = digitalRead(gameButton[y]);
//      Serial.print(buttonState); 
    
      if (buttonState == LOW)
      { //Checking for button push
        
        setLED(y);
        sounder.play(gameNote[y], 100);
        delay(200);
        setLED(9);
        inputArray[x] = y;
        delay(250);
        Serial.print(" ");
        Serial.print(y);
        if (inputArray[x] != turnButton[x]) { //Checks value input by user and checks it against
          fail(turnButton[x]);                              //the value in the same spot on the generated array
        }                                      //The fail function is called if it does not match
        x++;
      }
    }
//    Serial.println(' ');
  }
  delay(500);
  turnNumber++; //Increments the turn count, also the last action before starting the output function over again
}

void fail(int f) { //Function used if the player fails to match the sequence
/*  for(int x=0; x<gameNotes; x++)  // turn all on
  {
    setLED(x);
  }
  */
  setLED(f);

  for (int thisNote = 0; thisNote < introNotes; thisNote ++) {
    // play the next note:
    sounder.play(introNote[thisNote]);
    // hold the note:
    delay(duration[thisNote]);
    // stop for the next note:
    sounder.stop();
    delay(25);
  }
  
  setLED(9);
 
  delay(1000);
  
  //show score
  int z = 0;
  for (int y=0; y<=topscore; y++) {
    z = y % 5;
    setLED(z);
    sounder.play(gameButton[z], 100);
    delay(200);
    setLED(9);
    delay(250);
  }
  topscore = 0;
  turnNumber = -1; //Resets turn value so the game starts over without need for a reset button
}

byte startuptone2pin(uint16_t tonecode) {
  for(byte i=0; i<startNotes; i++) {
    if(startNote[i] == tonecode) {
      return i;
    }
  }
  return 0;
}

byte tone2pin(uint16_t tonecode) {
  for(byte i=0; i<gameNotes; i++) {
    if(gameNote[i] == tonecode) {
      return i;
    }
  }
  return 0;
}

void setLED(int led) {
  switch(led) {
    case 0:
      tlc.setLED(led, 0, 255, 0);
      break;
    case 1:
      tlc.setLED(led, 255, 0, 0);
      break;
    case 2:
      tlc.setLED(led, 128, 0, 128);
      break;
    case 3:
      tlc.setLED(led, 0, 0, 255);
      break;
    case 4:
      tlc.setLED(led, 255, 128, 0);
      break;
    //send any other index to leave all off
    default:
      for(int i=0; i<5; i++) {
        tlc.setLED(i, 0, 0, 0);
      }
  }
  tlc.write();
  
}
