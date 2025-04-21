#ifndef CIRCULAR_AVERAGE_H
#define CIRCULAR_AVERAGE_H

#include "Arduino.h"

/**
 *  A circular buffer of numbers to calculate a running average.
 *  @tparam T the type of elements stored in the buffer, should be a number type.
 */
template <typename T> 
class CircularAverage
{
  private:
    const uint8_t _max_size;

    uint8_t _index = 0;
    uint8_t _size = 0; 
    T _sum = (T)0;
    T *_buf;

    bool has_reached_end();

  public:
    CircularAverage(uint8_t max_size);
    ~CircularAverage();

    void add(T element);

    template <typename R>
    R quick_average();

    template <typename R>
    R average();

    String to_string();
};

#endif
