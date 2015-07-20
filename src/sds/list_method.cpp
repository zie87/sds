
#include <stdexcept>
#include <algorithm>

#include <sds/list_method.hpp>

namespace sequences
{
  list_method::list_method() noexcept
  : sequence_interface(), m_list()
  {}

  list_method::list_method(size_type size, value_type val)
  : sequence_interface(), m_list(size, val)
  {}

  list_method::~list_method() noexcept { m_list.clear();}

  list_method::size_type list_method::size() const noexcept { return m_list.size(); }
  bool list_method::empty() const noexcept { return m_list.empty(); } 

  list_method::string_type list_method::str() const
  {
    const size_type s = m_list.size();
    value_type array[s];
    m_list.copy(array);

    return string_type( array, s );
  }

  void list_method::insert( size_type pos, value_type val )
  {
    if(pos > size()) throw std::out_of_range("insert position higher then size! (list_method)");
    m_list.insert(pos, val);
  }

  void list_method::erase(size_type pos) 
  {
    if(pos >= size()) throw std::out_of_range("erase position higher then size! (list_method)");
    m_list.erase(pos);
  }

  list_method::value_type list_method::at(size_type pos) 
  {
    if(pos >= size()) throw std::out_of_range("at position higher then size! (list_method)");
    return m_list.at(pos);
  }

} // sequences