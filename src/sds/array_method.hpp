#ifndef SEQUENCES_ARRAY_METHOD_HPP
#define SEQUENCES_ARRAY_METHOD_HPP

#include "sequence_interface.hpp"

namespace sequences
{

  class array_method : public sequence_interface
  {
    public:
      using basic_type  = sequence_interface;
      using value_type  = typename basic_type::value_type;
      using size_type   = typename basic_type::size_type;
      using string_type = typename basic_type::string_type;

       array_method() noexcept;
      ~array_method() noexcept;

      array_method(array_method&& arr);
      explicit array_method(size_type size);
      explicit array_method(size_type size, value_type val);

      array_method(const array_method& arr);
      array_method& operator=(array_method arr);

      void insert( size_type pos, value_type val ) override;
      void erase( size_type pos) override;
      value_type at(size_type pos) override;
      string_type str() const override;      

      size_type size() const noexcept override;
      bool empty() const noexcept override;
  
    private:
      value_type* m_array;
      
      size_type   m_capacity;
      size_type   m_size;

            size_type   m_increase_counter =   0;
      const size_type   m_increase_factor  = 256;
  };

} // sequences


#endif // SEQUENCES_ARRAY_METHOD_HPP