
#include <stdexcept>
#include <algorithm>

#include <sds/array_method.hpp>

namespace sequences
{

  array_method::array_method() noexcept
  : sequence_interface(), m_array(nullptr), m_capacity(), m_size(0)
  {}

  array_method::array_method(size_type size)
  : sequence_interface(), m_array( new value_type[size] ), m_capacity(size), m_size(0)
  {}

  array_method::array_method(size_type size, value_type val)
  : sequence_interface(), m_array( new value_type[size] ), m_capacity(size), m_size(0)
  {
    std::fill_n( &m_array[0], size, val );
    m_size = size;
  }

  array_method::array_method(array_method&& arr)
  : m_array( std::move(arr.m_array) )
  , m_capacity(std::move(m_capacity))
  , m_size( std::move( arr.m_size ) )
  , m_increase_counter(std::move(arr.m_increase_counter))
  {}

  array_method::array_method(const array_method& arr)
  : array_method( arr.m_size )
  {
    std::copy( arr.m_array, arr.m_array + arr.m_size, m_array);
    m_size = arr.m_size;
    m_increase_counter = arr.m_increase_counter;
  }

  array_method& array_method::operator=(array_method arr)
  {
    std::swap( m_array,            arr.m_array);
    std::swap( m_size,             arr.m_size );
    std::swap( m_capacity,         arr.m_capacity );
    std::swap( m_increase_counter, arr.m_increase_counter );

    return *this;
  }

  array_method::~array_method() noexcept
  {
    if(m_array) { delete[] m_array; }
  }

  array_method::string_type array_method::str() const 
  {
    return string_type(m_array, m_size);
  }

  void array_method::insert(size_type pos, value_type val) 
  {
    if(pos > m_size) throw std::out_of_range("insert position higher then size! (array_method)");
    
    // resize?
    if( m_size == m_capacity )
    {
      unsigned int increase = (++m_increase_counter);
      auto new_capacity     = m_capacity + (increase * m_increase_factor); 
      value_type* new_array = new value_type[ new_capacity ];

      if(pos == 0) 
      { 
        new_array[0] = val;
        std::move( &m_array[0], &m_array[m_size], &new_array[1] );
      } else
      {
        std::move( &m_array[0], &m_array[pos], &new_array[0] );
        new_array[pos] = val;
        std::move( &m_array[pos], &m_array[m_size], &new_array[pos+1] );
      }

      delete[] m_array; m_array = new_array;
      m_capacity = new_capacity;
      ++m_size;
    } else
    {
      std::move_backward( &m_array[pos], &m_array[m_size], &m_array[m_size + 1] );
      m_array[pos] = val;
      ++m_size;
    }
  }

  void array_method::erase(size_type pos) 
  {
    if(pos > m_size) throw std::out_of_range("erase position higher then size! (array_method)");
    if(empty()) throw std::out_of_range("erase when empty! (array_method)");

    std::move( &m_array[pos+1], &m_array[m_size], &m_array[pos] );
    --m_size;
  }

  array_method::value_type array_method::at(size_type pos) 
  {
    if(pos > m_size) throw std::out_of_range("at position higher then size! (array_method)");
    return m_array[pos];
  }

  array_method::size_type array_method::size() const noexcept { return m_size; }
  bool array_method::empty() const noexcept { return m_size == 0; }
} // sequences