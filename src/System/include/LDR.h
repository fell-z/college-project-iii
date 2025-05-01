#ifndef LDR_H
#define LDR_H

#include "Arduino.h"
#include "Sensor.h"

class LDR : public Sensor
{
  private:
    uint8_t _pin;
    uint16_t _ref;
    uint16_t _value;
    unsigned long _ref_timeout;
    unsigned long _last_timeout = millis();
    float _detection_factor;

    /**
     *  @returns A value between 0 and 1023.
     */
    uint16_t read()
    {
      return analogRead(_pin);
    }

    /**
     *  @note The return value is capped to a value of 1023.
     *  @returns The reference value times the setted detection factor.
     */
    uint16_t detection_threshold()
    {
      uint16_t thres = (uint16_t)(_ref * _detection_factor);
      
      if (thres > 1023)
        return 1023;

      return thres;
    }

  public:
    /**
     *  @param pin The analog pin connected to the LDR.
     *  @param detection_factor The factor which determines the threshold.
     */
    LDR(uint8_t pin, unsigned long ref_timeout, float detection_factor = 1.25)
    {
      _pin = pin;
      _ref_timeout = ref_timeout;
      _detection_factor = detection_factor;
    }

    /**
     *  Initializes the pin and the reference.
     *  @note Must always be called in setup().
     */
    void begin()
    {
      pinMode(_pin, INPUT);
      _ref = read();
    }

    /**
     *  @returns The current reference.
     */
    uint16_t ref()
    {
      return _ref;
    }

    /**
     *  @returns The current value.
     */
    uint16_t value()
    {
      return _value;
    }

    void update()
    {
      if (millis() - _last_timeout >= _ref_timeout)
      {
        _last_timeout = millis();
        _ref = read();
      }

      _value = read();
    }

    /**
     *  Sets the detection factor to the new one provided.
     *  @param factor The new detection factor.
     */
    void set_detection_factor(float factor)
    {
      _detection_factor = factor;
    }

    /**
     *  @returns true If a object is close enough to the sensor, otherwise false.
     */
    bool LDR::detect_object()
    {
      return _value >= detection_threshold();
    }
};

#endif
