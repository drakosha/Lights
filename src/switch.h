#include <Bounce2.h>

class Switch : public Bounce {
  public:
    Switch() { enabled = false; state = false; }
    
    void enable() { enabled = true; }
    void disable() { enabled = false; }
    uint8_t getPin() { return pin; }

    bool enabled;
    bool state;
};