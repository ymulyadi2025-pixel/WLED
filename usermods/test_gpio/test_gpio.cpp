#include "wled.h"

class TestGpioUsermod : public Usermod {
  private:
    static const uint8_t PIN_BUTTON      = 6;
    static const uint8_t PIN_RELAY_TEST  = 7;
    static const uint8_t PIN_STATUS_LED  = 8;
    static const uint8_t PIN_BATTERY_ADC = 0;

    unsigned long lastCheck = 0;
    const unsigned long checkInterval = 1000;
    bool toggleState = false;

  public:
    void setup() override {
      pinMode(PIN_BUTTON, INPUT_PULLUP);
      pinMode(PIN_RELAY_TEST, OUTPUT);
      pinMode(PIN_STATUS_LED, OUTPUT);
      digitalWrite(PIN_RELAY_TEST, LOW);
      digitalWrite(PIN_STATUS_LED, LOW);
      Serial.println(F("[TestGpio] Usermod siap"));
    }

    void loop() override {
      if (millis() - lastCheck < checkInterval) return;
      lastCheck = millis();

      int buttonState = digitalRead(PIN_BUTTON);
      int batteryRaw  = analogRead(PIN_BATTERY_ADC);

      toggleState = !toggleState;
      digitalWrite(PIN_RELAY_TEST, toggleState);
      digitalWrite(PIN_STATUS_LED, toggleState);

      Serial.printf("[TestGpio] BTN=%d ADC=%d\n", buttonState, batteryRaw);
    }
};

static TestGpioUsermod test_gpio_usermod;
REGISTER_USERMOD(test_gpio_usermod);
