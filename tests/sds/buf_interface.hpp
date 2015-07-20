#ifndef TEST_BUF_INTERFACE_HPP
#define TEST_BUF_INTERFACE_HPP

#include <string>

namespace test
{

  struct buf_interface
  {
    using value_type  = char;
    using size_type         = std::size_t;
    using difference_type   = std::ptrdiff_t;
    using string_type = std::basic_string<value_type>;

    virtual ~buf_interface() {};

    virtual void insert( size_type pos, value_type val ) = 0;
    virtual void erase( size_type pos)                   = 0;
    virtual value_type at(size_type pos)                 = 0;
    virtual string_type str() const                      = 0;

    virtual size_type size() const noexcept              = 0;
    virtual bool empty() const noexcept                  = 0;
  };

} // test


#endif // TEST_BUF_INTERFACE_HPP