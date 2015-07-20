#ifndef SEQUENCES_LIST_METHOD_HPP
#define SEQUENCES_LIST_METHOD_HPP


#include "list.hpp"
#include "sequence_interface.hpp"

namespace sequences
{

  class list_method : public sequence_interface
  {
    public:
      using basic_type  = sequence_interface;
      using value_type  = typename basic_type::value_type;
      using size_type   = typename basic_type::size_type;
      using string_type = typename basic_type::string_type;

      using list_type   = ::lodur::container::list<value_type>;

       list_method() noexcept;
      ~list_method() noexcept;

      
      explicit list_method(size_type size, value_type val);

      list_method(const list_method&)            = delete;
      list_method& operator=(const list_method&) = delete;

      void insert( size_type pos, value_type val ) override;
      void erase( size_type pos) override;
      value_type at(size_type pos) override;
      string_type str() const override;      

      size_type size() const noexcept override;
      bool empty() const noexcept override;
  
    private:       
      list_type m_list;
  };

} // sequences


#endif // SEQUENCES_LIST_METHOD_HPP