#ifndef LDR_H
#define LDR_H

#include "Arduino.h"
#include "Sensor.h"

class LDR : public Sensor
{
  private:
    const uint8_t _pin;
    uint16_t _ref;
    uint16_t _value;
    float _detection_factor;

    uint16_t read();
    uint16_t detection_threshold();

  public:
    LDR(uint8_t pin, float detection_factor = 1.25);

    void begin();
    uint16_t get_ref();
    uint16_t get_value();
    void update_ref();
    void update_value();
    void set_detection_factor(float factor);
    bool detect_object();

    float ratio();
};

#endif
