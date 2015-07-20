#ifndef SEQUENCES_BUFFER_CONTROL_HPP
#define SEQUENCES_BUFFER_CONTROL_HPP

namespace sequences
{

  template<typename T>
  struct buffer_controller
  {
    using value_type = T;
    using size_type  = unsigned long int;

    buffer_controller() noexcept = default;

    buffer_controller(size_type capa) 
    : buffer( new value_type[capa] ), capacity(capa) 
    {}

    ~buffer_controller() noexcept { if(buffer) delete[] buffer; }

    buffer_controller(const buffer_controller&)            = delete;
    buffer_controller& operator=(const buffer_controller&) = delete;

    value_type* buffer = nullptr;

    size_type   size     = 0;
    size_type   capacity = 0;
  };

} // sequences

#endif // SEQUENCES_BUFFER_CONTROL_HPP