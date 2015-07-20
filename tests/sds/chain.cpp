#include "chain.hpp"

namespace test
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  piece_node::piece_node(size_type p, size_type o, size_type b) noexcept
  : pos(p), length(o), buffer_id(b)
  {}


  void piece_node::set_next( piece_node* new_node) noexcept
  {
    piece_node* next_ptr = next;

    next           = new_node;
    new_node->prev = this;
    new_node->next = next_ptr;

    if( next_ptr ){ next_ptr->prev = new_node; }
  }

  void piece_node::set_prev( piece_node* new_node ) noexcept
  {
    piece_node* prev_ptr = prev;
  
    prev           = new_node;
    new_node->next = this;
    new_node->prev = prev_ptr;

    if(prev_ptr){ prev_ptr->next = new_node; }
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  piece_chain::piece_chain() noexcept
  : basic_type(), m_head(), m_tail(), m_buffers()
  {
    m_head.set_next(&m_tail);
  }





  bool piece_chain::empty() const noexcept { return m_head.next == &m_tail; }



} // test