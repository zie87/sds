#include <sds/gap_method.hpp>

#include <stdexcept>
#include <algorithm>

namespace sequences
{
  gap_method::gap_method() noexcept
  : sequence_interface()
  , m_buffer(nullptr), m_buffer_len(0)
  , m_gap_min_len(32), m_gap_max_len(256), m_gap_offset_begin(0), m_gap_offset_end(0)
  {}

  gap_method::gap_method(size_type size, value_type val)
  : gap_method()
  {
    place_gap(0, size);
    std::fill_n( m_buffer, size, val );
    m_gap_offset_begin = size;
  }

  gap_method::~gap_method() noexcept 
  {
    if(m_buffer) delete[] m_buffer;
  }

  gap_method::string_type gap_method::str() const
  {
    size_type pos = 0;
    size_type len = size();

    size_type end = pos + len;

    if (end < m_gap_offset_begin) return string_type(m_buffer[pos], len);
    if (pos > m_gap_offset_begin) return string_type(m_buffer[pos + gap_length()], len);

    size_type block1Size = m_gap_offset_begin - pos;
    size_type block2Size = end - m_gap_offset_begin;

    string_type buf(&m_buffer[pos], block1Size);
    buf.append(&m_buffer[m_gap_offset_end], block2Size);

    return buf;    
  }

  gap_method::value_type gap_method::at(size_type pos)
  {
    if(pos > size()) throw std::out_of_range("at position higher then size! (gap_method)");
    if( pos < m_gap_offset_begin)  return m_buffer[pos];
    else return m_buffer[pos + gap_length() ];
  }

  gap_method::size_type gap_method::size() const noexcept { return m_buffer_len - gap_length(); }
  bool gap_method::empty() const noexcept { return size() == 0; }

  void gap_method::replace(size_type pos, size_type len, const value_type* val, size_type val_len)
  {
    if(pos > size() ) throw  std::out_of_range("ŕeplace position higher then size! (gap_method)");
  
    size_type new_len = 0;
    if( len > val_len ) new_len = 0;
    else new_len = val_len - len; 

    place_gap(pos + len, new_len) ;
    std::copy(val,val + val_len,&m_buffer[pos]);
    m_gap_offset_begin += val_len - len;
  }

  void gap_method::replace(size_type pos, size_type len, size_type val_len, value_type val)
  {
    if(pos > size() ) throw  std::out_of_range("ŕeplace position higher then size! (gap_method)");
  
    size_type new_len = 0;
    if( len > val_len ) new_len = 0;
    else new_len = val_len - len; 

    place_gap(pos + len, new_len) ;
    std::fill_n( &m_buffer[pos], val_len, val );
    m_gap_offset_begin += val_len - len;
  }


  void gap_method::insert(size_type pos, size_type len, value_type val) { replace(pos, 0, len, val); }
  void gap_method::insert(size_type pos, const value_type* s, size_type len) { replace(pos, 0, s, len); }
  void gap_method::insert( size_type pos, value_type val )
  { 
    if(pos > size() ) throw std::out_of_range("insert position higher then size! (gap_method)");

    place_gap(pos, 1);
    m_buffer[pos] = val;
    ++m_gap_offset_begin;
  }

  void gap_method::erase(size_type pos) { erase(pos, 1); }
  void gap_method::erase(size_type pos, size_type len)
  {
    if(pos > size() ) throw std::out_of_range("erase position higher then size! (gap_method)");
  
    size_type new_len = 0;
    if( len > 0 ) new_len = 0;
    else new_len -= len; 

    place_gap(pos + len, new_len) ;
    m_gap_offset_begin -= len;
  }

  void gap_method::place_gap(size_type pos, size_type len)
  {
    size_type delta_length = gap_length() - len;
    // if the gap has the right length, move the chars between offset and gap
    if (m_gap_min_len <= delta_length and delta_length <= m_gap_max_len) 
    {
      size_type delta = m_gap_offset_begin - pos;
      // check if the gap is already in place
      if (pos == m_gap_offset_begin) return;
      else if (pos < m_gap_offset_begin) 
      {
        size_type gap_len = m_gap_offset_end - m_gap_offset_begin;
        std::move( &m_buffer[pos], &m_buffer[pos + delta], &m_buffer[pos+gap_len] );
      } else 
      {
        std::move(&m_buffer[m_gap_offset_end], &m_buffer[m_gap_offset_end-delta] ,&m_buffer[m_gap_offset_begin]);
      }
      m_gap_offset_begin -= delta;
      m_gap_offset_end   -= delta;
      return;
    }

    // the gap has not the right length so
    // create new Buffer with new size and copy
    size_type old_length       = gap_length();
    size_type new_length       = m_gap_max_len + len;
    size_type new_gap_offset_end = pos + new_length;
    size_type new_buffer_len  = m_buffer_len + new_length - old_length;

    value_type* new_buffer    = new value_type[new_buffer_len];
    
    if (old_length == 0) 
    {
      std::move(m_buffer, &m_buffer[pos], new_buffer);
      std::move(&m_buffer[pos], &m_buffer[pos +  new_buffer_len - new_gap_offset_end], &new_buffer[new_gap_offset_end]);
    } else if (pos < m_gap_offset_begin) 
    {
      size_type delta = m_gap_offset_begin - pos;
      std::move(m_buffer, &m_buffer[pos], new_buffer);
      std::move(&m_buffer[pos], &m_buffer[pos + delta], &new_buffer[new_gap_offset_end]);
      std::move(&m_buffer[m_gap_offset_end], &m_buffer[m_gap_offset_end + m_buffer_len - m_gap_offset_end ], &new_buffer[new_gap_offset_end + delta]);
    } else {
      size_type delta = pos - m_gap_offset_begin;
      std::move(m_buffer, &m_buffer[m_gap_offset_begin], new_buffer);
      std::move(&m_buffer[m_gap_offset_end], &m_buffer[m_gap_offset_end+delta], &new_buffer[m_gap_offset_begin]);
      std::move(&m_buffer[m_gap_offset_end + delta], &m_buffer[m_gap_offset_end + delta + new_buffer_len - new_gap_offset_end], &new_buffer[new_gap_offset_end]);
    }
    
    delete[] m_buffer;
    m_buffer           = new_buffer;
    m_buffer_len       = new_buffer_len;
    m_gap_offset_begin = pos;
    m_gap_offset_end   = new_gap_offset_end;
  }

} // sequences