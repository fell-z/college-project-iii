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
    const int _max_size;

    int _index = 0;
    int _size = 0; 
    T _sum = (T)0;
    T* _buf;

    bool has_reached_end()
    {
      return _index == _max_size;
    }

  public:
    /**
     *  Set and creates the buffer of @a max_size size.
     *  @param max_size The maximum size of the buffer.
     */
    CircularAverage(int max_size) : _max_size(max_size)
    {
      _buf = new T[max_size] { 0 };
    }

    ~CircularAverage()
    {
      delete [] _buf;
    }

    /**
     *  Appends the @a element to buffer if not reached max size,
     *  otherwise it starts to replace from the first element and so forth.
     *  Also adds and subtracts to/from the @a sum hidden member.
     *  @param element The element to be added.
     */
    void add(T element)
    {
      if (has_reached_end())
        _index = 0;
      if (_size < _max_size)
        _size++;

      // replace the old value with new value
      _sum -= _buf[_index];
      _sum += element;

      _buf[_index++] = element;
    }

    /**
     *  Erases the contents of the buffer by setting all values to 0.
     */
    void clear()
    {
      _index = 0;
      _size = 0;
      _sum = (T)0;
      memset(_buf, 0, sizeof(T) * _max_size);
    }

    /**
     *  Quickly calculates the average by using the @a sum hidden member,
     *  thus performing only a division per method call.
     *  @returns The calculated average.
     *
     *  @warning May present some precision errors when doing an average of floats.
     */
    float quick_average()
    {
      if (_size == 0)
        return 0.0;

      return (float)_sum / _size;
    }

    /**
     *  Calculates the average by summing the elements in the buffer
     *  and then dividing by the number of elements.
     *  @returns The calculated average.
     */
    float average()
    {
      float sum = 0;

      for (int i = 0; i < _size; ++i)
        sum += _buf[i];

      return sum / _size;
    }

    String to_string()
    {
      String repr = String("{");

      for (int i = 0; i < _max_size; ++i)
        repr += String(" ") + String(_buf[i]);

      return repr + String(" }");
    }
};

#endif
