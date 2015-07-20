#include <sds/piece_chain_method.hpp>

#include <algorithm>
#include <stdexcept>

#include <iostream>
#include <iomanip>

namespace sequences
{
  static const piece_chain_method::size_type INIT_SIZE = 0x10000;

  piece_node::piece_node(size_type p, size_type o, size_type b) noexcept
  : pos(p), length(o), buffer_id(b)
  {}



  piece_chain_method::piece_chain_method() noexcept
  : basic_type(), m_size(0), m_buffer_list(), m_head(), m_tail(), m_modify_buffer(), m_last_act(), m_last_node()
  {
    m_head.next = &m_tail;
    m_tail.prev = &m_head;
  }

  piece_chain_method::piece_chain_method(size_type size, value_type val)
  : piece_chain_method()
  {
    append(size, val);
  }

  piece_chain_method::~piece_chain_method() noexcept
  {
    clear();
  }


  void piece_chain_method::clear() noexcept
  {
    for(size_type i = 0; i < m_buffer_list.size(); ++i)
    {
      buffer_type* tmp = m_buffer_list[i];
      m_buffer_list[i] = nullptr;
      delete tmp;
    }
    m_buffer_list.clear();

    piece_node* tmp = m_tail.prev;
    while(tmp->prev)
    {
      piece_node* delete_node = tmp;
      tmp = tmp->prev;
      delete delete_node;
    }

    m_head.next = &m_tail;
    m_tail.prev = &m_head;
    m_size = 0;
  }

  piece_chain_method::size_type piece_chain_method::size() const noexcept { return m_size; }
  bool piece_chain_method::empty() const noexcept { return m_head.next == &m_tail; }

  piece_chain_method::value_type piece_chain_method::at(size_type pos)
  {
    if(pos > size() ) throw std::out_of_range("at position higher then size! (piece_chain_method)");

    search_node s_node = search_piece(pos);

    buffer_type* buf = m_buffer_list[ s_node.node->buffer_id ];
    size_type    search_offset = pos - s_node.pos;
    value_type v = *( buf->buffer + ( s_node.node->pos + search_offset ) );
    return v;
  }


  void piece_chain_method::insert( size_type pos, const value_type v){ insert(pos, &v, 1); }
  void piece_chain_method::append(const value_type* s, size_type len){ insert(m_size, s, len); }
  void piece_chain_method::append(size_type len, value_type v){ insert(m_size, len, v); }

  piece_chain_method::string_type piece_chain_method::str() const noexcept
  {
    string_type str;
    piece_node* tmp = m_head.next;

    while(tmp != &m_tail)
    {
      buffer_type* buf = m_buffer_list[tmp->buffer_id];
      str.append( buf->buffer + tmp->pos, tmp->length );
      tmp = tmp->next;
    }
    return str;
  }


  void piece_chain_method::insert( size_type pos, size_type len, value_type s)
  {
    if(pos > size() ) throw std::out_of_range("insert position higher then size! (piece_chain_method)");
    size_type     id = alloc_buffer(len);
    buffer_type* buf = m_buffer_list[id];
    std::fill( buf->buffer, buf->buffer+len, s );
    buf->size = len;

    piece_node* new_piece = new piece_node(0, len, id);

    record_action(action::invalid, 0);
    insert_piece(pos, new_piece, len);
    m_size += len;
  }

  void piece_chain_method::insert(size_type pos, const value_type* s, size_type len )
  {
    if(pos > size() ) throw std::out_of_range("insert position higher then size! (piece_chain_method)");

    // check if modify buffer exist if not create one
    if( m_modify_buffer.id > m_buffer_list.size() ) alloc_modify_buffer( len + INIT_SIZE );

    buffer_type* buf = m_buffer_list[ m_modify_buffer.id ];

    // check if enough space in modify buffer if not reallocate
    if( (buf->capacity - buf->size) < len )
    {
      alloc_modify_buffer( len + INIT_SIZE );
      buf = m_buffer_list[ m_modify_buffer.id ];
      record_action(action::invalid, 0);
    } 

    // copy new content
    std::copy( s, s + len, buf->buffer + m_modify_buffer.pos );
    buf->size += len;
    m_modify_buffer.pos += len;

    piece_node* new_piece = new piece_node(m_modify_buffer.pos - len, len, m_modify_buffer.id);

    insert_piece(pos, new_piece, len);
    record_action(action::insert, pos + len);
    m_size += len;
  }

  void  piece_chain_method::insert_piece(size_type pos, piece_node* new_piece, size_type len)
  {
    // get piece with search position
    // size_type   piece_idx = 0;
    // piece_node* piece_ptr = search_piece(pos, piece_idx);


    search_node s_node = search_piece(pos);
    size_type   piece_idx = s_node.pos;
    piece_node* piece_ptr = s_node.node;

    size_type   insert_offset = pos - piece_idx;

    // insert bevor the node
    if(insert_offset == 0 && can_optimize(action::insert, pos))
    {
      piece_ptr->prev->length += len;
    }
    else if(insert_offset == 0)
    {
      piece_node* prev = piece_ptr->prev;
      prev->next = new_piece;
      
      new_piece->prev = prev;
      new_piece->next = piece_ptr;

      piece_ptr->prev = new_piece;
    } else // insert in a node
    {

      piece_node* split_node = new piece_node( piece_ptr->pos + insert_offset, piece_ptr->length - insert_offset, piece_ptr->buffer_id );
      piece_ptr->length = insert_offset;

      piece_node* next = piece_ptr->next;

      split_node->next      = next;
      next->prev            = split_node;

      split_node->prev = new_piece;
      new_piece->next  = split_node;

      new_piece->prev = piece_ptr;
      piece_ptr->next = new_piece;

      // split_node->next = piece_ptr->next;
      // split_node->next->prev = split_node;

      // piece_ptr->next = new_piece;
      // new_piece->next = split_node;
      // split_node->prev = new_piece;
      // new_piece->prev = piece_ptr; 
    }
  }

  void piece_chain_method::erase(size_type pos) { erase(pos,1); }

  void piece_chain_method::erase(size_type pos, size_type len)
  {
    if(pos > size() ) throw std::out_of_range("erase position higher then size! (piece_chain_method)");
  
    // reduze erase length to max if higher then size
    if( (pos + len) > m_size ) len = m_size - pos;

    search_node s_node = search_piece(pos);
    size_type   piece_idx = s_node.pos;
    piece_node* piece_ptr = s_node.node;

    size_type   erase_offset = pos - piece_idx;
    size_type   erase_length = len;
    // split is needed
    if( (piece_ptr->length - erase_offset) > erase_length )
    {
      size_type remove_size = erase_offset + erase_length;
      piece_node* split_node = new piece_node( piece_ptr->pos + remove_size, piece_ptr->length - remove_size, piece_ptr->buffer_id );

      split_node->next  = piece_ptr->next;
      split_node->prev  = piece_ptr;
      piece_ptr->next   = split_node;
      piece_ptr->length = erase_offset;
    } else
    {
      piece_ptr->length = erase_offset;
      erase_length -= erase_offset;
      while( erase_length > 0 )
      {
        piece_ptr = piece_ptr->next;
        if( piece_ptr->length > erase_length )
        {
          piece_ptr->pos    += erase_length;
          piece_ptr->length -= erase_length;
          erase_length = 0;
        } else
        {
          size_type length  = piece_ptr->length;
          piece_ptr->length = 0;
          erase_length -= length;
        }
      }
    }

    record_action(action::erase, pos);
    m_size -= len;
  }

  piece_chain_method::size_type piece_chain_method::alloc_buffer( size_type capacity )
  {
    size_type idx = m_buffer_list.size();
  
    buffer_type* buf = new buffer_type(capacity);
    m_buffer_list.push_back(buf);
  
    return idx;
  }

  void  piece_chain_method::alloc_modify_buffer( size_type capacity )
  {
    size_type id = alloc_buffer(capacity);
    m_modify_buffer.id  = id;
    m_modify_buffer.pos = 0;
  }

  piece_chain_method::search_node piece_chain_method::search_piece(size_type pos) const noexcept
  {
    // hack for 'const' bound access 
    // piece_node* head_ptr = m_tail.next->prev;
    // piece_node* tail_ptr = m_tail.prev->next;

    search_node    s_piece;
    search_node    sb_piece;
    // hack for 'const' bound access 
    // if( pos == m_size ) { return search_node( pos, m_tail.prev->next ); }

    // if( pos < m_size/2 )
    // {
      s_piece = search_piece_forwards(pos, m_head, m_tail, 0);
      // std::cout << "forwards item pos: " << s_piece.pos << " ptr: " << s_piece.node << std::endl;
    // } else
    // {
      // sb_piece = search_piece_backwards(pos, m_tail, m_head, m_size);
      // std::cout << "backwards item pos: " << sb_piece.pos << " ptr: " << sb_piece.node << std::endl;
    // }

      // debug();
    // if( m_last_node.node == nullptr )
    // {
    //   if(pos < m_size/2)
    //   {
    //     search_piece = search_piece_forwards(pos, m_head.next, tail_ptr);
    //   } else
    //   {
    //     search_piece = search_piece_backwards(pos, m_tail.prev, head_ptr, m_size);
    //   }
    // } else
    // {
    //   piece_node* last_ptr = m_last_node.node;
    //   size_type   last_idx = m_last_node.pos;

    //   if( (last_idx <= pos) && (pos <= (last_idx + last_ptr->length))  )
    //   {
    //     //searched piece is equal to last piece
    //     search_piece = m_last_node;
    //   } else
    //   {
    //     if( pos < last_idx )
    //     {
    //       size_type distance_pos = last_idx - pos;

    //       if( pos < distance_pos ) // near to head
    //       {
    //         search_piece = search_piece_forwards(pos, m_head.next, last_ptr);
    //       } else
    //       {
    //         search_piece = search_piece_backwards( pos, last_ptr, head_ptr, last_idx);
    //       }
    //     } else
    //     {
    //       size_type distance_pos = pos - last_idx + last_ptr->length;
    //       size_type distance_end = m_size - pos;

    //       if( distance_pos < distance_end )
    //       {
    //         search_piece = search_piece_forwards( distance_pos, last_ptr->next, tail_ptr );
    //       } else
    //       {
    //         search_piece = search_piece_backwards( pos, m_tail.prev, last_ptr, m_size );
    //       }
    //     }
    //   }
    // }

    // piece_idx = search_piece.pos;
    return s_piece;
  }

  piece_chain_method::search_node piece_chain_method::search_piece_forwards(size_type pos, const piece_node& head_node, const piece_node& tail_node, size_type first_pos) const noexcept
  {
    // std::cout << "*** search_piece_forwards ***" << std::endl;
    // std::cout << "pos: " << pos << " first_pos: " << first_pos << std::endl;
    // std::cout << "head_node: " << &head_node << " tail_node: " << &tail_node << std::endl;

    // piece_node* tmp = head_node.next;
    // while( tmp != &tail_node )
    // {
    //   std::cout << "FRONT " << tmp << std::endl;
    //   tmp = tmp->next;
    // } 
    // tmp = tail_node.prev;
    // while( tmp != &head_node )
    // {
    //   std::cout << "BACK: " << tmp << std::endl;
    //   tmp = tmp->prev;
    // } 

    search_node s_piece;
    size_type   current_pos = first_pos;

    for( s_piece.node = head_node.next; s_piece.node != &tail_node; s_piece.node = s_piece.node->next )
    {
      // std::cout << " node: " << s_piece.node << " current_pos:" << current_pos << " node length: " << s_piece.node->length << std::endl;
      if( (pos >= current_pos) && (pos < current_pos + s_piece.node->length) )
      { 
        s_piece.pos = current_pos;
        return s_piece;
      }
      current_pos += s_piece.node->length;
    }
    // insert at tail
    if(pos == current_pos)
    {
      // std::cout << "return tail" << std::endl;
      s_piece.pos  = current_pos;
      return s_piece;
    }

    return search_node();

  }

  // piece_chain_method::search_node piece_chain_method::search_piece_forwards(size_type pos, piece_node* head_ptr, piece_node* tail_ptr) const noexcept
  // {
  //   search_node search_piece;
  //   size_type   current_pos = 0;

  //   // scan the list looking for the piece which holds the specified position
  //   for(search_piece.node = head_ptr; search_piece.node != tail_ptr; search_piece.node = search_piece.node->next)
  //   {
  //     if( (pos >= current_pos) && (pos < current_pos + search_piece.node->length) )
  //     { 
  //       search_piece.pos = current_pos;
  //       return search_piece;
  //     }
  //     current_pos += search_piece.node->length;
  //   }
  //   // insert at tail
  //   if(pos == current_pos)
  //   {
  //     search_piece.pos = current_pos;
  //     return search_piece;
  //   }
  //   return search_node();
  // }

  // piece_chain_method::search_node piece_chain_method::search_piece_backwards(size_type pos, const piece_node& tail_node, const piece_node& head_node, size_type last_pos) const noexcept
  // {
    // piece_node* sptr = nullptr;
    // for(sptr = tail_node.prev; sptr != &head_node; sptr = sptr->prev)
    // {
    //   auto buffer = m_buffer_list[sptr->buffer_id]->buffer;
    //   std::cout << "{" << std::setw(10) << sptr->buffer_id << "}"
    //             << "[" << std::left << std::setw(10) << sptr->pos 
    //             << "<" << std::right << std::setw(16) << sptr << ">"
    //             << std::right << std::setw(10) << sptr->length << "]\t"
    //             << string_type( buffer + sptr->pos, buffer + sptr->pos + sptr->length )
    //             << std::endl;
    // }

    // std::cout << "*** search_piece_backwards ***" << std::endl;    
    // std::cout << "pos: " << pos << " last_pos: " << last_pos << std::endl;
    // std::cout << "head_node: " << &head_node << " tail_node: " << &tail_node << std::endl;

    // piece_node* tmp = head_node.next;
    // while( tmp != &tail_node )
    // {
    //   std::cout << "FRONT " << tmp << std::endl;
    //   tmp = tmp->next;
    // } 
    // tmp = tail_node.prev;
    // while( tmp != &head_node )
    // {
    //   std::cout << "BACK: " << tmp << std::endl;
    //   tmp = tmp->prev;
    // } 

    // search_node s_piece;
    // size_type   current_pos = last_pos;

    // if( pos == current_pos )
    // {
    //   std::cout << "return tail" << std::endl;
    //   return search_node(current_pos, tail_node.prev->next );
    // }

    // for( s_piece.node = tail_node.prev; s_piece.node != &head_node; s_piece.node = s_piece.node->prev )
    // {
    //   std::cout << " node: " << s_piece.node << " current_pos:" << current_pos << " node length: " << s_piece.node->length << std::endl;
    //   current_pos -= s_piece.node->length;
    //   if( (pos >= current_pos) && (pos < current_pos + s_piece.node->length) )
    //   { 
    //     s_piece.pos = current_pos;
    //     return s_piece;
    //   }
    // }
    // if(pos == current_pos)
    // {
    //   s_piece.pos  = current_pos;
    //   // s_piece.node = s_piece.node->next;
    //   return s_piece;
    // }

  //   return search_node();
  // }

  // piece_chain_method::search_node piece_chain_method::search_piece_backwards(size_type pos, piece_node* tail_ptr, piece_node* head_ptr, size_type size) const noexcept
  // {
  //   search_node search_piece;
  //   size_type   cur_idx = size;

  //   for(search_piece.node = tail_ptr; search_piece.node != head_ptr; search_piece.node = search_piece.node->prev)
  //   {
  //     cur_idx -= search_piece.node->length;
  //     if( (pos >= cur_idx) && (pos < cur_idx + search_piece.node->length) )
  //     { 
  //       search_piece.pos = cur_idx;
  //       return search_piece;
  //     }
  //   }
  //   search_piece.node = tail_ptr;

  //   // insert at tail
  //   if(pos == cur_idx)
  //   {
  //     search_piece.pos = cur_idx;
  //     return search_piece;
  //   }
  //   return search_node();
  // }


  bool piece_chain_method::can_optimize(action act, size_type pos)
  {
    return (m_last_act.act == act) && ( m_last_act.pos == pos );
  }

  void piece_chain_method::record_action (action act, size_type pos)
  {
    m_last_act.act  = act;
    m_last_act.pos  = pos;
    // m_last.node = node;
  }

  void piece_chain_method::debug () const noexcept
  {
    piece_node* sptr = nullptr;

    std::cout << "HEAD: <" << std::right << std::setw(16) << &m_head << ">" << std::endl;
    std::cout << "TAIL: <" << std::right << std::setw(16) << &m_tail << ">" << std::endl;

    std::cout << "**********************" << std::endl;
    for(sptr = m_head.next; sptr != &m_tail; sptr = sptr->next)
    {
      auto buffer = m_buffer_list[sptr->buffer_id]->buffer;
      std::cout << "{" << std::setw(10) << sptr->buffer_id << "}"
                << "[" << std::left << std::setw(10) << sptr->pos 
                << "<" << std::right << std::setw(16) << sptr << ">"
                << std::right << std::setw(10) << sptr->length << "]\t"
                << string_type( buffer + sptr->pos, buffer + sptr->pos + sptr->length )
                << std::endl;
    }

    std::cout << "-------------------------" << std::endl;

    for(sptr = m_tail.prev; sptr != &m_head; sptr = sptr->prev)
    {
      auto buffer = m_buffer_list[sptr->buffer_id]->buffer;
      std::cout << "{" << std::setw(10) << sptr->buffer_id << "}"
                << "[" << std::left << std::setw(10) << sptr->pos 
                << "<" << std::right << std::setw(16) << sptr << ">"
                << std::right << std::setw(10) << sptr->length << "]\t"
                << string_type( buffer + sptr->pos, buffer + sptr->pos + sptr->length )
                << std::endl;
    }

    std::cout << "**********************" << std::endl;

    for(sptr = m_head.next; sptr != &m_tail; sptr = sptr->next)
    {
      auto buffer = m_buffer_list[sptr->buffer_id]->buffer;
      std::cout << string_type( buffer + sptr->pos, buffer + sptr->pos + sptr->length );
    }

    std::cout << std::endl << "piece_chain length = " << m_size << " chars" << std::endl;
    std::cout << std::endl;
  }

} // sequences