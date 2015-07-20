#include "buffer_control.hpp"

namespace neatpad
{

  buffer_control* buffer_control::alloc_buffer(size_type capacity)
  {
    buffer_control* bc = new buffer_control() ;
    
    bc->buffer  = new value_type[capacity];
    bc->length   = 0;
    bc->capacity = capacity;
    return bc;
  }

} // neatpad