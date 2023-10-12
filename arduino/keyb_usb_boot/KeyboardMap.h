#include "HID-Project.h"

#define ROWS 5 // IN
#define COLS 15 // OUT
#define COL_PINS 4 // OUT - using 4 to 16 decoder
#define SIZE ROWS*COLS // size of the matrix
#define LAYERS 2

const uint8_t COLpins[COL_PINS] = {4,5,6,8}; // output (4 to 16 decoder)
const uint8_t ROWpins[ROWS] = {22,21,20,19,18}; // input
const uint8_t COLmap[COLS] = {10,9,8,7,6,5,4,3,2,1,0,11,12,13,14}; // mapping of physical pins to logical columns

enum KeyFunction : uint8_t {
  FN_NORM = 0,
  FN_SWITCH = 1,
  FN_MOUSE_BTN_LEFT = 2,
  FN_MOUSE_BTN_RIGHT = 3,
  FN_MOUSE_WHEEL = 4,
  FN_MOUSE_UP = 5, 
  FN_MOUSE_DOWN = 6, 
  FN_MOUSE_LEFT = 7, 
  FN_MOUSE_RIGHT = 8, 
};

struct KeyRaw {
  KeyboardKeycode mod; 
  KeyboardKeycode key;
  KeyFunction function;

  bool isStandard() { return function==FN_NORM;}
  bool isFunction() { return !isStandard();}
  bool standardNonEmpty() { return isStandard() && (mod!=KEY_RESERVED || key!=KEY_RESERVED); }
  bool isMouseMove() { return function==FN_MOUSE_UP || function==FN_MOUSE_DOWN || FN_MOUSE_UP==FN_MOUSE_LEFT || FN_MOUSE_RIGHT;}
};

struct KeyConfig {
  uint8_t layer; 
  bool wheel;
};


const KeyRaw keyMap[ROWS*COLS*LAYERS] {
  {KEY_RESERVED,KEY_ESC},{KEY_RESERVED,KEY_1,FN_NORM},{KEY_RESERVED,KEY_2,FN_NORM},{KEY_RESERVED,KEY_3,FN_NORM},{KEY_RESERVED,KEY_4,FN_NORM},
    {KEY_RESERVED,KEY_5,FN_NORM},{KEY_RESERVED,KEY_6,FN_NORM},{KEY_RESERVED,KEY_7,FN_NORM},{KEY_RESERVED,KEY_8,FN_NORM},{KEY_RESERVED,KEY_9,FN_NORM},
      {KEY_RESERVED,KEY_0,FN_NORM},{KEY_RESERVED,KEY_MINUS,FN_NORM},{KEY_RESERVED,KEY_EQUAL,FN_NORM},{KEY_RESERVED,KEY_DELETE,FN_NORM},{KEY_RESERVED,KEY_BACKSPACE,FN_NORM},
  {KEY_RESERVED,KEY_TAB,FN_NORM},{KEY_RESERVED,KEY_Q,FN_NORM},{KEY_RESERVED,KEY_W,FN_NORM},{KEY_RESERVED,KEY_E,FN_NORM},{KEY_RESERVED,KEY_R,FN_NORM},
    {KEY_RESERVED,KEY_T,FN_NORM},{KEY_RESERVED,KEY_Y,FN_NORM},{KEY_RESERVED,KEY_U,FN_NORM},{KEY_RESERVED,KEY_I,FN_NORM},{KEY_RESERVED,KEY_O,FN_NORM},
      {KEY_RESERVED,KEY_P,FN_NORM},{KEY_RESERVED,KEY_LEFT_BRACE,FN_NORM},{KEY_RESERVED,KEY_RIGHT_BRACE,FN_NORM},{KEY_RESERVED,KEY_BACKSLASH,FN_NORM},{KEY_RESERVED,KEY_HOME,FN_NORM},
  {KEY_RESERVED,KEY_CAPS_LOCK,FN_NORM},{KEY_RESERVED,KEY_A,FN_NORM},{KEY_RESERVED,KEY_S,FN_NORM},{KEY_RESERVED,KEY_D,FN_NORM},{KEY_RESERVED,KEY_F,FN_NORM},
    {KEY_RESERVED,KEY_G,FN_NORM},{KEY_RESERVED,KEY_H,FN_NORM},{KEY_RESERVED,KEY_J,FN_NORM},{KEY_RESERVED,KEY_K,FN_NORM},{KEY_RESERVED,KEY_L,FN_NORM},
      {KEY_RESERVED,KEY_SEMICOLON,FN_NORM},{KEY_RESERVED,KEY_QUOTE,FN_NORM},{KEY_RESERVED,KEY_ENTER,FN_NORM},{KEY_RESERVED,KEY_END,FN_NORM},{KEY_RESERVED,KEY_RESERVED,FN_NORM},
  {KEY_LEFT_SHIFT,KEY_RESERVED,FN_NORM},{KEY_LEFT_SHIFT,KEY_RESERVED,FN_NORM},{KEY_RESERVED,KEY_Z,FN_NORM},{KEY_RESERVED,KEY_X,FN_NORM},{KEY_RESERVED,KEY_C,FN_NORM},
    {KEY_RESERVED,KEY_V,FN_NORM},{KEY_RESERVED,KEY_B,FN_NORM},{KEY_RESERVED,KEY_N,FN_NORM},{KEY_RESERVED,KEY_M,FN_NORM},{KEY_RESERVED,KEY_COMMA,FN_NORM},
      {KEY_RESERVED,KEY_PERIOD,FN_NORM},{KEY_RESERVED,KEY_SLASH,FN_NORM},{KEY_RESERVED,KEY_UP_ARROW,FN_NORM},{KEY_RESERVED,KEY_RESERVED,FN_MOUSE_WHEEL},{KEY_RIGHT_SHIFT,KEY_RESERVED,FN_NORM},
  {KEY_RESERVED,KEY_RESERVED,FN_SWITCH},{KEY_LEFT_CTRL,KEY_RESERVED,FN_NORM},{KEY_LEFT_WINDOWS,KEY_RESERVED,FN_NORM},{KEY_LEFT_ALT,KEY_RESERVED,FN_NORM},{KEY_RESERVED,KEY_SPACE,FN_NORM},
    {KEY_RESERVED,KEY_SPACE,FN_NORM},{KEY_RESERVED,KEY_SPACE,FN_NORM},{KEY_RESERVED,KEY_SPACE,FN_NORM},{KEY_RESERVED,KEY_SPACE,FN_NORM},{KEY_RESERVED,KEY_SPACE,FN_NORM},
      {KEY_RIGHT_ALT,KEY_RESERVED,FN_NORM},{KEY_RESERVED,KEY_LEFT_ARROW,FN_NORM},{KEY_RESERVED,KEY_DOWN_ARROW,FN_NORM},{KEY_RESERVED,KEY_RIGHT_ARROW,FN_NORM},{KEY_RIGHT_CTRL,KEY_RESERVED,FN_NORM},

  {KEY_RESERVED,KEY_ESC,FN_NORM},{KEY_RESERVED,KEY_F1,FN_NORM},{KEY_RESERVED,KEY_F2,FN_NORM},{KEY_RESERVED,KEY_F3,FN_NORM},{KEY_RESERVED,KEY_F4,FN_NORM},
    {KEY_RESERVED,KEY_F5,FN_NORM},{KEY_RESERVED,KEY_F6,FN_NORM},{KEY_RESERVED,KEY_F7,FN_NORM},{KEY_RESERVED,KEY_F8,FN_NORM},{KEY_RESERVED,KEY_F9,FN_NORM},
      {KEY_RESERVED,KEY_F10,FN_NORM},{KEY_RESERVED,KEY_F11,FN_NORM},{KEY_RESERVED,KEY_F12,FN_NORM},{KEY_RESERVED,KEY_DELETE,FN_NORM},{KEY_RESERVED,KEY_BACKSPACE,FN_NORM},
  {KEY_RESERVED,KEY_TAB,FN_NORM},{KEY_RESERVED,KEY_Q,FN_NORM},{KEY_RESERVED,KEY_W,FN_NORM},{KEY_RESERVED,KEY_E,FN_NORM},{KEY_RESERVED,KEY_R,FN_NORM},
    {KEY_RESERVED,KEY_T,FN_NORM},{KEY_RESERVED,KEY_Y,FN_NORM},{KEY_RESERVED,KEY_U,FN_NORM},{KEY_RESERVED,KEY_I,FN_NORM},{KEY_RESERVED,KEY_O,FN_NORM},
      {KEY_RESERVED,KEY_P,FN_NORM},{KEY_RESERVED,KEY_LEFT_BRACE,FN_NORM},{KEY_RESERVED,KEY_RIGHT_BRACE,FN_NORM},{KEY_RESERVED,KEY_BACKSLASH,FN_NORM},{KEY_RESERVED,KEY_HOME,FN_NORM},
  {KEY_RESERVED,KEY_TILDE,FN_NORM},{KEY_RESERVED,KEY_A,FN_NORM},{KEY_RESERVED,KEY_S,FN_NORM},{KEY_RESERVED,KEY_D,FN_NORM},{KEY_RESERVED,KEY_F,FN_NORM},
    {KEY_RESERVED,KEY_G,FN_NORM},{KEY_RESERVED,KEY_H,FN_NORM},{KEY_RESERVED,KEY_J,FN_NORM},{KEY_RESERVED,KEY_K,FN_NORM},{KEY_RESERVED,KEY_L,FN_NORM},
      {KEY_RESERVED,KEY_SEMICOLON,FN_NORM},{KEY_RESERVED,KEY_QUOTE,FN_NORM},{KEY_RESERVED,KEY_ENTER,FN_NORM},{KEY_RESERVED,KEY_INSERT,FN_NORM},{KEY_RESERVED,KEY_RESERVED,FN_NORM},
  {KEY_LEFT_SHIFT,KEY_RESERVED,FN_NORM},{KEY_LEFT_SHIFT,KEY_RESERVED,FN_NORM},{KEY_RESERVED,KEY_Z,FN_NORM},{KEY_RESERVED,KEY_X,FN_NORM},{KEY_RESERVED,KEY_C,FN_NORM},
    {KEY_RESERVED,KEY_V,FN_NORM},{KEY_RESERVED,KEY_B,FN_NORM},{KEY_RESERVED,KEY_N,FN_NORM},{KEY_RESERVED,KEY_M,FN_NORM},{KEY_RESERVED,KEY_COMMA,FN_NORM},
      {KEY_RESERVED,KEY_PERIOD,FN_NORM},{KEY_RESERVED,KEY_SLASH,FN_NORM},{KEY_RESERVED,KEY_RESERVED,FN_MOUSE_UP},{KEY_RESERVED,KEY_RESERVED,FN_MOUSE_WHEEL},{KEY_RIGHT_SHIFT,KEY_RESERVED,FN_NORM},
  {KEY_RESERVED,KEY_RESERVED,FN_SWITCH},{KEY_LEFT_CTRL,KEY_RESERVED,FN_NORM},{KEY_LEFT_WINDOWS,KEY_RESERVED,FN_NORM},{KEY_LEFT_ALT,KEY_RESERVED,FN_NORM},{KEY_RESERVED,KEY_RESERVED,FN_MOUSE_BTN_LEFT},
    {KEY_RESERVED,KEY_RESERVED,FN_MOUSE_BTN_LEFT},{KEY_RESERVED,KEY_SPACE,FN_NORM},{KEY_RESERVED,KEY_SPACE,FN_NORM},{KEY_RESERVED,KEY_RESERVED,FN_MOUSE_BTN_RIGHT},{KEY_RESERVED,KEY_SPACE,FN_MOUSE_BTN_RIGHT},
      {KEY_RIGHT_ALT,KEY_RESERVED,FN_NORM},{KEY_RESERVED,KEY_RESERVED,FN_MOUSE_LEFT},{KEY_RESERVED,KEY_RESERVED,FN_MOUSE_DOWN},{KEY_RESERVED,KEY_RESERVED,FN_MOUSE_RIGHT},{KEY_RIGHT_CTRL,KEY_RESERVED,FN_NORM}
};

struct KeyPos {
  uint8_t row; // ROW - input
  uint8_t col; // COL - output

  bool operator==(const KeyPos other) const { return (row == other.row && col == other.col);}
  uint8_t index() { return row*COLS+COLmap[col];}
  KeyRaw code(uint8_t layer) {return keyMap[index()+layer*SIZE];}
};