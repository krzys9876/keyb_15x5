#include "HID-Project.h"
#include "KeyDebouncer.h"
#include "ArrList.h"
#include "KeyboardMap.h"
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#define MIN_BETWEEN_MILLIS 10
#define RELEASE_MILLIS 10
#define TICK_PIN 23
#define LED_TICKS 10

KeyConfig config = {0,false,false}; // 0 - standard, 1 - F-keys

#define NUMPIXELS 8
#define LED_PIN 11
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

KeyDebouncer keyDebouncer[SIZE];

ArrList<KeyPos> keyb=ArrList<KeyPos>(SIZE);
ArrList<KeyPos> keybPrev=ArrList<KeyPos>(SIZE);

void setup() {
  BootKeyboard.begin();
  BootMouse.begin();
  Serial.begin(115200);
  //Serial1.begin(115200);
  pixels.begin(); 
  Wire.begin();

  for(uint8_t i=0;i<COL_PINS;i++) pinMode(COLpins[i], OUTPUT); 
  for(uint8_t i=0;i<ROWS;i++) pinMode(ROWpins[i], INPUT_PULLUP);   
  pinMode(TICK_PIN,OUTPUT);
}

void loop() {
  static long counter=0;
  static bool loopTick=true;

  readMatrix();
  bool pr=false;
  long kMillis=millis();
  pr|=processPress(kMillis); 
  pr|=processRelease(kMillis);
  clearIfEmpty(kMillis);  

  if(pr) {    
    copyKeyState(); 
    /*Serial1.println("no of pressed keys "+String(keyb.getSize()));
    uint32_t c=pixels.Color(20,20,20);
    uint32_t blank=pixels.Color(0,0,0);
    for(int i=0;i<keyb.getSize();i++) pixels.setPixelColor(i,c);
    for(int i=keyb.getSize();i<SIZE;i++) pixels.setPixelColor(i,blank);*/
  }

  // show pixels every X ticks - it does not make sense to do it every time
  //if((counter % LED_TICKS) == 0) pixels.show();  
  counter++;
  loopTick=!loopTick;
  digitalWrite(TICK_PIN,loopTick);  
}

void readMatrix() {
  keyb.empty();
  for(uint8_t c=0;c<COLS;c++) {
    writeColumns(c);
    for(uint8_t r=0;r<ROWS;r++) {
      uint8_t k=digitalRead(ROWpins[r]);
      if(k==LOW) {
        //BootKeyboard.write('A');
        //delay(100);
        keyb.insertVal({r,c});
      }
    }
  }
}

KeyRaw functionOverride(KeyPos pos) {
  KeyRaw ret = pos.code(config.layer);
  if(config.mouse) {
    switch(ret.key) {
      case KEY_UP_ARROW : ret.function = FN_MOUSE_UP; break;
      case KEY_DOWN_ARROW : ret.function = FN_MOUSE_DOWN; break;
      case KEY_LEFT_ARROW : ret.function = FN_MOUSE_LEFT; break;
      case KEY_RIGHT_ARROW : ret.function = FN_MOUSE_RIGHT; break;
    }
    if(pos.isLeftSpace()) ret.function = FN_MOUSE_BTN_LEFT;
    else if(pos.isRightSpace()) ret.function = FN_MOUSE_BTN_RIGHT;
  }
  if(pos.isFn()) ret.function = FN_SWITCH;
  else if(pos.isMouse()) ret.function = FN_MOUSE;
  else if(pos.isWheel()) ret.function = FN_MOUSE_WHEEL;

  return ret;
}

bool pressFunction(KeyPos pos) {
  bool pressed=false;
  KeyRaw code = functionOverride(pos);
  //Serial.println("pressed Function "+String(pos.row)+" "+String(pos.col)+" "+String(COLmap[pos.col])+" "+String(pos.isLeftSpace())+" "+String(pos.isRightSpace())+" "+String(code.function));
  if(code.isFunction() && !keyDebouncer[pos.index()].isPressConfirmed()) {
    switch(code.function) {
      case FN_SWITCH : config.layer=1; break;
      case FN_MOUSE : config.mouse=true; break;
      case FN_MOUSE_BTN_LEFT : BootMouse.press(MOUSE_LEFT); break;
      case FN_MOUSE_BTN_RIGHT : BootMouse.press(MOUSE_RIGHT); break;
      case FN_MOUSE_WHEEL : config.wheel=true; break;
      case FN_MOUSE_UP : if(config.wheel) BootMouse.move(0,0,1); else BootMouse.move(0,-1,0); break;
      case FN_MOUSE_DOWN : if(config.wheel) BootMouse.move(0,0,-1); else BootMouse.move(0,1,0); break;
      case FN_MOUSE_LEFT : BootMouse.move(-1,0,0); break;
      case FN_MOUSE_RIGHT : BootMouse.move(1,0,0); break;
      default : break;
    }
    keyDebouncer[pos.index()].confirmPress();
    pressed=true;
  }
  return pressed;
}

KeyboardKeycode keycodeOverride(KeyPos pos) {  
  KeyboardKeycode actualCode=pos.code(config.layer).key;
  if(config.mouse) {
    switch(actualCode) {
      // Ignore arrows when mouse is active
      case KEY_DOWN_ARROW : actualCode=KEY_RESERVED; break;
      case KEY_UP_ARROW : actualCode=KEY_RESERVED; break;
      case KEY_LEFT_ARROW : actualCode=KEY_RESERVED; break;
      case KEY_RIGHT_ARROW : actualCode=KEY_RESERVED; break;
      default : break;
    }
    if(pos.isLeftSpace() || pos.isRightSpace()) actualCode = KEY_RESERVED;
  } else {
    switch(actualCode) {
      case KEY_DOWN_ARROW : if(config.wheel) actualCode=KEY_PAGE_DOWN; break;
      case KEY_UP_ARROW : if(config.wheel) actualCode=KEY_PAGE_UP; break;
      default : break;
    }
  }
  return actualCode;
}

bool pressNormal(KeyPos pos) {
  bool pressed=false;
  if(pos.code(config.layer).isStandard()) {
    //Serial.println(String(index)+" - "+String(functionIndex));
    //if(index==functionIndex) Serial.println(String(keyDebouncer[index].isPressed()));
    KeyRaw code=pos.code(config.layer);
    if(code.mod!=KEY_RESERVED) BootKeyboard.add(code.mod);
    KeyboardKeycode actualCode=keycodeOverride(pos);
    if(actualCode!=KEY_RESERVED) BootKeyboard.add(actualCode);
    pressed=code.standardNonEmpty();
    //Serial.println("pressed " + String(code.key) + " (" + String(code.mod)+")");        
  }
  return pressed;
}

bool processPressOne(uint8_t keyIndex, long kMillis, bool (*process)(KeyPos pos)) {
  bool pressed=false;
  KeyPos pos=keyb.getVal(keyIndex);
  if(keybPrev.notExists(pos) && keyDebouncer[pos.index()].shouldPress(kMillis)) pressed|=(*process)(pos);
  return pressed;
}

bool processPressList(long kMillis, bool (*process)(KeyPos pos)) {
  bool pressed = false;
  for(uint8_t i=0;i<keyb.getSize();i++) pressed|=processPressOne(i,kMillis,process);
  return pressed;
}

uint8_t calcMouseMove(long kMillis, long mouseMoveStartMills, bool slow) {
  static long lastMillis=0;
  if(lastMillis<mouseMoveStartMills) lastMillis=mouseMoveStartMills;
  long move=0;  
  float speed=mouseSpeedRatio(kMillis-mouseMoveStartMills,slow);
  if(kMillis>lastMillis) move=speed*(kMillis-lastMillis);
  if(speed==0 || move>0) lastMillis=kMillis;
  //Serial.println("lastMillis="+String(lastMillis)+"millis="+String(kMillis)+" start="+String(mouseMoveStartMills)+" speed="+String(speed)+" move="+String(move));
  uint8_t moveRet;
  if(move<127) moveRet=move; else moveRet=0;
  return moveRet;
}

void keepMouseMoving(KeyRaw code, uint8_t move) {
  switch(code.function) {
    case FN_MOUSE_UP : if(config.wheel) BootMouse.move(0,0,move); else BootMouse.move(0,-move,0); break;
    case FN_MOUSE_DOWN : if(config.wheel) BootMouse.move(0,0,-move); else BootMouse.move(0,move,0); break;
    case FN_MOUSE_LEFT : BootMouse.move(-move,0,0); break;
    case FN_MOUSE_RIGHT : BootMouse.move(move,0,0); break;
    default : break;
  }
}

bool isMouseMoving() {
  bool mouseMoving=false;
  for(uint8_t i=0;i<keyb.getSize();i++) {
    KeyRaw code =  functionOverride(keyb.getVal(i));
    mouseMoving = mouseMoving || code.isMouseMove();
  }
  return mouseMoving;
}

void keepPressingFunctionKeys(long kMillis) {
  static long mouseMoveStartMills=kMillis;

  // currently this function handles mouse move only
  bool mouseMoving=isMouseMoving();
  uint8_t move=0;
  if(!mouseMoving) mouseMoveStartMills=kMillis;
  if(mouseMoving) move=calcMouseMove(kMillis,mouseMoveStartMills,config.wheel);
  for(uint8_t i=0;i<keyb.getSize();i++) {
    KeyRaw code=functionOverride(keyb.getVal(i));
    if(mouseMoving && code.isMouseMove()) keepMouseMoving(code,move);
  }
}

bool processPress(long kMillis) {
  bool fnPressed=processPressList(kMillis,pressFunction);
  bool usbPressed=processPressList(kMillis,pressNormal);
  keepPressingFunctionKeys(kMillis);

  if(usbPressed) BootKeyboard.send(); // this is actual effect!
  return fnPressed || usbPressed;
}

bool releaseFunction(KeyPos pos) {
  bool released=false;
  KeyRaw code = functionOverride(pos);
  if(code.isFunction() && !keyDebouncer[pos.index()].isReleaseConfirmed()) {    
    //Serial.println(code.function);
    switch(code.function) {
      case FN_SWITCH : config.layer=0; break;
      case FN_MOUSE : 
        config.mouse=false; 
        BootMouse.release(MOUSE_LEFT); // release buttons just in case they were pressed when mouse button was released
        BootMouse.release(MOUSE_RIGHT);
        break;
      case FN_MOUSE_BTN_LEFT : BootMouse.release(MOUSE_LEFT); break;
      case FN_MOUSE_BTN_RIGHT : BootMouse.release(MOUSE_RIGHT); break;
      case FN_MOUSE_WHEEL : config.wheel=false; break;
      default : break;
    }
    keyDebouncer[pos.index()].confirmRelease();
    released=true;
  }
  return released;
}

bool releaseNormal(KeyPos pos) {
  bool released=false;
  if(pos.code(config.layer).isStandard()) {
    //Serial.println(String(index)+" - "+String(functionIndex));
    //if(index==functionIndex) Serial.println(String(keyDebouncer[index].isPressed()));
    KeyRaw code=pos.code(config.layer);
    if(code.mod!=KEY_RESERVED) BootKeyboard.remove(code.mod);
    KeyboardKeycode actualCode=keycodeOverride(pos);
    if(actualCode!=KEY_RESERVED) BootKeyboard.remove(actualCode);
    released=code.standardNonEmpty();
    //Serial.println("pressed " + String(code.key) + " (" + String(code.mod)+")");        
  }
  return released;
}

bool processReleaseOne(uint8_t keyIndex, long kMillis, bool (*process)(KeyPos pos)) {
  bool released=false;
  KeyPos pos=keybPrev.getVal(keyIndex);
  if(keyb.notExists(pos) && keyDebouncer[pos.index()].shouldRelease(kMillis)) released|=(*process)(pos);
  return released;
}

bool processReleaseList(long kMillis, bool (*process)(KeyPos pos)) {
  bool released = false;
  for(uint8_t i=0;i<keybPrev.getSize();i++) released|=processReleaseOne(i,kMillis,process);
  return released;
}

bool processRelease(long kMillis) {
  bool fnReleased=processReleaseList(kMillis,releaseFunction);
  bool usbReleased=processReleaseList(kMillis,releaseNormal);

  if(usbReleased) BootKeyboard.send(); // this is actual effect!
  return fnReleased || usbReleased;
}

void clearIfEmpty(long kMillis) {
  static long lastReleaseMillis=0;
  //NOTE: isEmpty is true only if all keys (incl FN, Mouse, Wheel) are released
  if((keyb.isEmpty() || (keyb.getSize()==1 && keyb.getVal(0).isMouse())) && kMillis-lastReleaseMillis>RELEASE_MILLIS) {
    //Serial.println("empty" +String(kMillis));
    BootKeyboard.removeAll();
    BootKeyboard.send(); // this is actual effect!
    lastReleaseMillis=kMillis;
    for(uint8_t i=0;i<SIZE;i++) keyDebouncer[i].init();
  }
}

void writeColumns(uint8_t col) {
  for(uint8_t i=0;i<COL_PINS;i++) digitalWrite(COLpins[i],col & (1 << i)); // 0 for selected column, 1 otherwise
}

void copyKeyState() {
  keybPrev.empty();
  for(uint8_t i=0;i<keyb.getSize();i++) keybPrev.insertVal(keyb.getVal(i));
}

float mouseSpeedRatio(long pressDurationMillis,bool slow) {
  float ratio=0.0;
  if(pressDurationMillis<100) ratio=0.0;
  else if (pressDurationMillis<500) ratio=0.1;
  else if (pressDurationMillis<1000) ratio=0.1+0.9*(pressDurationMillis-500.0)/(1000.0-500.0);
  else ratio=1.00;
  if(slow) ratio=ratio/15.0;
  return ratio; 
}
