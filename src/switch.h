#include <Bounce2.h>

class Switch : public Bounce {
  public:
    Switch() { m_enabled = false; m_state = false; }
    
    void enable() { m_enabled = true; }
    void disable() { m_enabled = false; }
    bool enabled() { return m_enabled; }

    uint8_t getPin() { return pin; }

  private:
    bool m_enabled;
    bool m_state;
};