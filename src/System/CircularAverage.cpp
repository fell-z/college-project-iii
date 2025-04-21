#include "include/CircularAverage.h"

/**
 *  Set and creates the buffer of @a max_size size.
 *  @param max_size The maximum size of the buffer.
 */
template <typename T>
CircularAverage<T>::CircularAverage(uint8_t max_size) : _max_size(max_size)
{
  _buf = new T[max_size] { 0 };
}

template <typename T>
CircularAverage<T>::~CircularAverage()
{
  delete [] _buf;
}

template <typename T>
bool CircularAverage<T>::has_reached_end()
{
  return _index == _max_size;
}

/**
 *  Appends the @a element to buffer if not reached max size,
 *  otherwise it starts to replace from the first element and so forth.
 *  Also adds and subtracts to/from the @a sum hidden member.
 *  @param element The element to be added.
 */
template <typename T>
void CircularAverage<T>::add(T element)
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
 *  Quickly calculates the average by using the @a sum hidden member,
 *  thus performing only a division per method call.
 *  @tparam R The return type, should be a number type.
 *  @returns The calculated average.
 *
 *  @warning May present some precision errors when doing an average of floats.
 */
template <typename T>
template <typename R>
R CircularAverage<T>::quick_average()
{
  return (R)_sum / _size;
}

/**
 *  Calculates the average by summing the elements in the buffer
 *  and then dividing by the number of elements.
 *  @tparam R The return type, should be a number type.
 *  @returns The calculated average.
 */
template <typename T>
template <typename R>
R CircularAverage<T>::average()
{
  R sum = 0;

  for (uint8_t i = 0; i < _size; ++i)
    sum += _buf[i];

  return sum / _size;
}

template <typename T>
String CircularAverage<T>::to_string()
{
  String repr = String("{");

  for (uint8_t i = 0; i < _max_size; ++i)
    repr += String(" ") + String(_buf[i]);

  return repr + String(" }");
}
