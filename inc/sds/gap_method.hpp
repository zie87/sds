#ifndef SEQUENCES_GAP_METHOD_HPP
#define SEQUENCES_GAP_METHOD_HPP

#include "sequence_interface.hpp"

namespace sequences
{

  class gap_method : public sequence_interface
  {
    public:
      using basic_type  = sequence_interface;
      using value_type  = typename basic_type::value_type;
      using size_type   = typename basic_type::size_type;
      using string_type = typename basic_type::string_type;

       gap_method() noexcept;
      ~gap_method() noexcept;
      
      explicit gap_method(size_type size, value_type val);

      gap_method(const gap_method&)            = delete;
      gap_method& operator=(const gap_method&) = delete;

      void replace(size_type pos, size_type len, const value_type* val, size_type val_len);
      void replace(size_type pos, size_type len, size_type val_len, value_type val);

      void insert(size_type pos, const value_type* s, size_type len);
      void insert(size_type pos, size_type len, value_type val) override;
      void insert( size_type pos, value_type val ) override;
      void erase( size_type pos) override;
      void erase( size_type pos, size_type len);
      value_type at(size_type pos) override;
      string_type str() const override;      

      size_type size() const noexcept override;
      bool empty() const noexcept override;
  
    private:
      inline size_type gap_length() const noexcept { return m_gap_offset_end - m_gap_offset_begin; }
      void place_gap(size_type pos, size_type len);

      value_type* m_buffer;
      
      size_type     m_buffer_len;

      size_type     m_gap_min_len;
      size_type     m_gap_max_len;

      size_type     m_gap_offset_begin;
      size_type     m_gap_offset_end;
  };

} // sequences


#endif // SEQUENCES_GAP_METHOD_HPP