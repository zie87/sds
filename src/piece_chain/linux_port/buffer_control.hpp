#ifndef SEQUENCES_BUFFER_CONTROL_HPP
#define SEQUENCES_BUFFER_CONTROL_HPP

namespace sequences
{

  struct buffer_control
  {
    using value_type = char;
    using size_type  = unsigned long int;

    buffer_control() = default;

    buffer_control(const buffer_control&)            = delete;
    buffer_control& operator=(const buffer_control&) = delete;

    value_type*   buffer   = nullptr;
    size_type     length   = 0;
    size_type     capacity = 0;
    int           id       = 0;

    static buffer_control* alloc_buffer(size_type capacity);
  };


} // sequences

#endif // SEQUENCES_BUFFER_CONTROL_HPP