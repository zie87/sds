#ifndef SEQUENCES_SEQUENCE_INTERFACE_HPP
#define SEQUENCES_SEQUENCE_INTERFACE_HPP

#include <string>

namespace sequences
{

  struct sequence_interface
  {
    using value_type  = char;
    using size_type   = std::size_t;
    using string_type = std::basic_string<value_type>;

    virtual ~sequence_interface() {};

    virtual void insert( size_type pos, value_type val ) = 0;
    virtual void erase( size_type pos)                   = 0;
    virtual value_type at(size_type pos)                 = 0;
    virtual string_type str() const                      = 0;

    virtual size_type size() const noexcept              = 0;
    
    virtual bool empty() const noexcept { return size() == 0; }
  };

} // sequences


#endif // SEQUENCES_SEQUENCE_INTERFACE_HPP