#define DELAY 5 // normally these are milliseconds

class KeyDebouncer {
    public:
        KeyDebouncer();
        ~KeyDebouncer();

        void init();
        void confirmPress();
        bool isPressConfirmed();
        void confirmRelease();
        bool isReleaseConfirmed();
        bool shouldPress(long tick);
        bool shouldRelease(long tick);
        bool isPressed();
        bool isReleased();
        long ticksSinceLast(long currentTick);

        enum KeyState : uint8_t {
            RELEASED = 0,
            WAITING_FOR_PRESS = 1,
            PRESSED = 2,
            WAITING_FOR_RELEASE = 3
        };

    private:
        KeyState state;
        long lastTick = 0; // last press or release tick (can't be both, see state logic)
        bool pressConfirmed = false;
        bool releaseConfirmed = false;
        void setPressed();
        void setReleased();
};

KeyDebouncer::KeyDebouncer() { 
    init();
}
KeyDebouncer::~KeyDebouncer() {}

void KeyDebouncer::confirmPress() { pressConfirmed = true; }
bool KeyDebouncer::isPressConfirmed() { return pressConfirmed;}
void KeyDebouncer::confirmRelease() { releaseConfirmed = true; }
bool KeyDebouncer::isReleaseConfirmed() { return releaseConfirmed;}

bool KeyDebouncer::shouldPress(long tick) {
    switch (state) {
        case RELEASED:
            lastTick = tick;
            state = WAITING_FOR_PRESS;
            break;
        case WAITING_FOR_PRESS:
            if(tick - lastTick > DELAY || tick - lastTick < 0) setPressed();
            break;
        case WAITING_FOR_RELEASE: setPressed(); break;
        
        default:
            break;
        }
    return state == PRESSED;
}

bool KeyDebouncer::shouldRelease(long tick) {
    switch (state) {
        case PRESSED:
            lastTick = tick;
            state = WAITING_FOR_RELEASE;
            break;
        case WAITING_FOR_RELEASE:
            if(tick - lastTick > DELAY || tick - lastTick < 0) setReleased();
            break;
        case WAITING_FOR_PRESS: setReleased(); break;
        
        default:
            break;
        }
    return state == RELEASED;
}

void KeyDebouncer::setReleased() {
  state=RELEASED;
  pressConfirmed=false;
  releaseConfirmed=false;
}

void KeyDebouncer::setPressed() {
  state=PRESSED;
  pressConfirmed=false;
}

void KeyDebouncer::init() {
  setReleased();
  lastTick=0;
  }
bool KeyDebouncer::isPressed() {return state == PRESSED;}
bool KeyDebouncer::isReleased() {return state == RELEASED;}

long KeyDebouncer::ticksSinceLast(long currentTick) { return currentTick-lastTick;}