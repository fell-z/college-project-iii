#include "include/LDR.h"

/**
 *  @param pin The analog pin connected to the LDR.
 *  @param detection_factor The factor which determines the threshold.
 */
LDR::LDR(uint8_t pin, float detection_factor = 1.25) : _pin(pin)
{
  _detection_factor = detection_factor;
}

/**
 *  @returns A value between 0 and 1023.
 */
uint16_t LDR::read()
{
  return analogRead(_pin);
}

/**
 *  @note The return value is capped to a value of 1023.
 *  @returns The reference value times the setted detection factor.
 */
uint16_t LDR::detection_threshold()
{
  uint16_t thres = (uint16_t)(_ref * _detection_factor);
  
  if (thres > 1023)
    return 1023;

  return thres;
}

/**
 *  Initializes the pin and the reference.
 *  @note Must always be called in setup().
 */
void LDR::begin()
{
  pinMode(_pin, INPUT);
  _ref = read();
}

/**
 *  @returns The current reference.
 */
uint16_t LDR::get_ref()
{
  return _ref;
}

/**
 *  @returns The current value.
 */
uint16_t LDR::get_value()
{
  return _value;
}

/**
 *  Updates the reference.
 */
void LDR::update_ref()
{
  _ref = read();
}

/**
 *  Updates the value.
 */
void LDR::update_value()
{
  _value = read();
}

/**
 *  Sets the detection factor to the new one provided.
 *  @param factor The new detection factor.
 */
void LDR::set_detection_factor(float factor)
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

/**
 *  @copydoc Sensor::ratio()
 */
float LDR::ratio()
{
  int16_t d = _ref - _value;
  return abs(d) / 1024.0;
}
