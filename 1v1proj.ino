//Lokesh Maddana, 
#include <IRremote.h>
#define IS_Master true
typedef uint32_t uint24_t;
typedef uint32_t rfPayload;
enum GameState{
  UnkownState,
  ReadyState,
  WaitingForCountdownState,
  CountdownState,
  InGameState,
  LostState,
  WonState
};
enum IrAddress{
  UnknownAddress,
  MasterAddress,
  SlaveAddress
}
enum NecCommand {
  UnknownCommand,
  RequestCountdownCommand,
  TryWinCommand,
  DeclareLossCommand
};
const unsigned long baud = 9600;
const uint8_t buttonPin = 2;
const uint8_t redPin = 3;
const uint8_t greenPin = 4;
const uint8_t bluePin = 5;
const uint8_t irRecieverPin = 6;
const uint8_t irEmitterPin = 7;
const uint8_t buzzerPin = 8;
const uint8_t gyroSdaPin = A4;
const uint8_t gyroSclPin = A5;
const unsigned int buzzerFrequency = 300;
const unsigned int long buzzerDuration = 300;
const uint16_t irAddress = IS_MASTER ? MasterAddress : SlaveAddress;
const uint24_t white = 0xFFFFFF;
const uint24_t red = 0xFF0000;
const uint24_t green = 0x00FF00;
const uint24_t blue = 0x0000FF;
const uint24_t black = 0x000000;
const uint24_t purple = 0x800080;
IRrecv irReciever;
IRsend irEmitter;
int lastButtonState;
GameState gameState;
unsigned long countdown;
unsinged long preCountdown;
void setRgbLed(const uint8_t red, const uint8_t green, const uint8_t blue){
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}
void setRgbLed(const uint24_t hex){
  setRgbLed(
    (hex >> 0x10) & 0xFF,
    (hex >> 0x08) & 0xFF,
    hex & 0xFF
  );

}
void irSend(uint8_t command){
  irEmitter.sendNEC(irAddress, command, 0);
}
bool irGet(uint8_t* command, IRRawDataType* raw){
  if(!irReciever.decode()){
    *raw = 0;
    return false;
  }
  *raw = irReciever.decodedIRData.decodeRawData;
  irReceiver.resume();
  if(!irReciever.decodeNEC()){
    return false;
  }
  if(irReciever.decodedIRData.address == irAddress){
    return false;
  }
  *command = irReciever.decodedIRData.command;
  return true;
}





}




void setup() {
  Serial.begin(baud);
  while(!Serial){

  }
  pinMode(buttonPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  irReciever = IRrecv(irRecieverPin);
  irReciever.enableIRIn();
  irEmitter = IRsend(irEmitterPin);
  lastButtonState = LOW;
  gameState = ReadyState;
  countdown = 0;

}

void loop() {
  int buttonState = digitalRead(buttonPin);
  bool newButtonPress = buttonState == HIGH && lastButtonState != HIGH;
  lastButtonState = buttonState;
  uint8_t command;
  IRRawDataType raw;
  bool newIrCommand = irGet(&command, &raw);
  switch(gameState){
    case ReadyState:
    setRgbLed(black);
    if(newButtonPress) {
      gameState = WaitingForCountdownState;
      preCountdown = millis();
      countdown = millis() + 10*1000;
      irSend(RequestCountdownCommand);
    }
    break;
    case WaitingForCountdownState:
    setRgbLed(white);
    if(millis() < preCountdown + 500) {
      break;
    }
    if (raw != 0){
      uint8_t countdownOffset2 = 5;
      irSend(countdownOffset2);
      countdown = millis() + countdownOffset2 * 1000;
      countdown -= 500;
      gameState = CountdownState;
      break;
    }
    if(newIrCommand){
      uint8_t countdownOffset = 5;
      if(command == RequestCountdownCommand){
        irSend(countdownOffset);
      }
      else{
        countdownOffset = command;
      }
      countdown = millis() + countdownOffset * 1000;
      gameState = CountdownState;
      break;
    }
    if(millis() >= countdown){
      gameState = ReadyState;
    }
    break;
    case CountdownState:
    setRgbLed(blue);
    if (millis () >= countdown){
      tone(buzzerPin, buzzerFrequencu, buzzerDuration);
      gameState = InGameState;
    }
    break;
    case InGameState:
    set RgbLed(purple);
    if (raw != 0){
      gameState = LostState;
      countdown = millis() + 5 * 1000;
    }
    if(newIrCommand){
      if(command == TryWinCommand){
        gameState = LostState;
      }
      else if (command == DeclareLossCommand){
        gameState = WonState;
      }
      
      countdown = millis() + 5 * 1000;
    }
    if(newButtonPress){
      irSend(TryWinCommand);
      gameState = WonState;
      countdown = millis() + 5 * 1000;
    }
    break;
    case WonState:
      set RgbLed(green);
      if(millis() >= countdown){
        gameState = ReadyState;
      }
      break;
    case LostState:
      set RgbLed(red);
      if(millis() >= countdown){
        gameState = ReadyState;
      }


  }
}













