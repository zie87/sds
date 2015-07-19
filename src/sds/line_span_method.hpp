#ifndef SEQUENCES_LINE_SPAN_METHOD_HPP
#define SEQUENCES_LINE_SPAN_METHOD_HPP

#include "sequence_interface.hpp"
#include "array_methode.hpp"

namespace sequences
{

  class line_span_method : public sequence_interface
  {
    public:
      using basic_type  = sequence_interface;
      using value_type  = typename basic_type::value_type;
      using size_type   = typename basic_type::size_type;
      using string_type = typename basic_type::string_type;

       line_span_method() noexcept;
      ~line_span_method() noexcept;
      
      explicit line_span_method(size_type size);
      explicit line_span_method(size_type size, value_type val);

      line_span_method(const line_span_method&)            = delete;
      line_span_method& operator=(const line_span_method&) = delete;

      void insert( size_type pos, value_type val ) override;
      void erase( size_type pos) override;
      value_type at(size_type pos) override;
      string_type str() const override;      

      size_type size() const noexcept override;
      bool empty() const noexcept override;
  
    private:
      value_type* m_buffer;

            size_type m_size;
      const size_type m_line_width = 128; 

            size_type  m_increase_counter = 0;
      const size_type  m_increase_factor  = 1024;

  };

} // sequences

#endif // SEQUENCES_LINE_SPAN_METHOD_HPP