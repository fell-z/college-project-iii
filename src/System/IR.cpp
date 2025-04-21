#include "include/IR.h"

/**
 *  Constructor for IR.
 *  @param data_pin The pin connected to the @a out of the sensor.
 */
IR::IR(uint8_t data_pin) : _data_pin(data_pin) {}

/**
 *  Initializes the pin to the appropriate mode.
 */
void IR::begin()
{
  pinMode(_data_pin, INPUT);
}

/**
 *  @returns HIGH or LOW.
 */
uint8_t IR::read()
{
  return digitalRead(_data_pin);
}

bool IR::detect_object()
{
  return read() == HIGH;
}

/**
 *  @copydoc Sensor::ratio()
 */
float IR::ratio()
{
  return (float)read();
}
