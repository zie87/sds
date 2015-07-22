#include <sds/piece_chain_method.hpp>

#include <algorithm>
#include <stdexcept>

#include <iostream>
#include <iomanip>

namespace sequences
{
  static const piece_chain_method::size_type INIT_SIZE = 0x10000;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  piece_node::piece_node(size_type p, size_type o, size_type b) noexcept
  : pos(p), length(o), buffer_id(b)
  {}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  piece_chain_method::piece_chain_method() noexcept
  : basic_type(), m_head(), m_tail(), m_buffers(), m_modify_buf(), m_last_search()
  , m_size(0)
  {
    m_head.next = &m_tail;
    m_tail.prev = &m_head;
  }

  piece_chain_method::piece_chain_method(size_type size, value_type val)
  : piece_chain_method()
  { append(size, val); }

  piece_chain_method::~piece_chain_method() noexcept { clear(); }


  void piece_chain_method::clear() noexcept
  {
    for(size_type i = 0; i < m_buffers.size(); ++i)
    {
      buffer_type* tmp = m_buffers[i];
      m_buffers[i] = nullptr;
      delete tmp;
    }
    m_buffers.clear();

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

  piece_chain_method::value_type piece_chain_method::at(size_type pos)
  {
    if(pos > size() ) throw std::out_of_range("at position higher then size! (piece_chain_method)");

    search_node s_node = search_piece(pos);
         m_last_search = s_node;

    buffer_type* buf = m_buffers[ s_node.node->buffer_id ];
    size_type    search_offset = pos - s_node.pos;
    value_type v = *( buf->buffer + ( s_node.node->pos + search_offset ) );
    return v;
  }


  piece_chain_method::size_type piece_chain_method::size() const noexcept { return m_size; }
  bool piece_chain_method::empty() const noexcept { return m_head.next == &m_tail; }

  void piece_chain_method::append(const value_type* s, size_type len){ insert(m_size, s, len); }
  void piece_chain_method::append(size_type len, value_type v){ insert(m_size, len, v); }

  void piece_chain_method::insert(size_type pos, value_type v) { insert(pos, &v, 1); }
  void piece_chain_method::insert(size_type pos, size_type len, value_type v)
  {
    if(pos > size() ) throw std::out_of_range("insert position higher then size! (piece_chain_method)");
    size_type     id = alloc_buffer(len);
    buffer_type* buf = m_buffers[id];
    // std::fill( buf->buffer, buf->buffer+len, v );
    std::fill_n(buf->buffer, len, v);
    buf->size = len;

    piece_node* new_piece = new piece_node(0, len, id);

    insert_piece(pos, new_piece, len);
    m_size += len;
  }


  void piece_chain_method::insert(size_type pos, const value_type* s, size_type len )
  {
    if(pos > size() ) throw std::out_of_range("insert position higher then size! (piece_chain_method)");

    // check if modify buffer exist if not create one
    if( m_modify_buf.id > m_buffers.size() ) alloc_modify_buffer( len + INIT_SIZE );

    buffer_type* buf = m_buffers[ m_modify_buf.id ];

    // check if enough space in modify buffer if not reallocate
    if( (buf->capacity - buf->size) < len )
    {
      alloc_modify_buffer( len + INIT_SIZE );
      buf = m_buffers[ m_modify_buf.id ];
    } 

    // copy new content
    std::copy( s, s + len, buf->buffer + m_modify_buf.pos );
    buf->size += len;
    m_modify_buf.pos += len;

    // append to last search?
    if( (m_last_search.pos <= pos) && (pos == (m_last_search.pos + m_last_search.node->length)) )
    {
      piece_node* last_ptr = m_last_search.node;
      if( (last_ptr->buffer_id == m_modify_buf.id) && ( last_ptr->pos + last_ptr->length == buf->size - len ) )
      {
        last_ptr->length += len;
        m_size += len;
        return;
      }
    }

    piece_node* new_piece = new piece_node(m_modify_buf.pos - len, len, m_modify_buf.id);

    insert_piece(pos, new_piece, len);
    m_size += len;
  }


  void piece_chain_method::erase(size_type pos) { erase(pos,1); }
  void piece_chain_method::erase(size_type pos, size_type len)
  {
    if(pos > size() ) throw std::out_of_range("erase position higher then size! (piece_chain_method)");
  
    // reduce erase length if neccessary
    if( (pos + len) > m_size ) len = m_size - pos;

    search_node s_node     = search_piece(pos);
    m_last_search          = s_node;

    size_type erase_offset = pos - s_node.pos;
    size_type erase_length = len;
    piece_node* piece_ptr  = s_node.node;

    if( (piece_ptr->length -erase_offset) > erase_length )
    { // only split is needed

      size_type new_pos     = erase_offset + erase_length;
      piece_node* split_ptr = new piece_node( piece_ptr->pos + new_pos , piece_ptr->length - new_pos, piece_ptr->buffer_id );
      piece_ptr->length     = erase_offset;

      m_last_search.node = piece_ptr->prev;
      m_last_search.pos -= piece_ptr->length;

      piece_node* next_ptr  = piece_ptr->next;

      // connect split with first part
      piece_ptr->next = split_ptr;
      split_ptr->prev = piece_ptr;

      // connect split with next
      next_ptr->prev  = split_ptr;
      split_ptr->next = next_ptr;

    } else
    {
      m_last_search.node = piece_ptr->prev;
      m_last_search.pos -= piece_ptr->length;

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
    m_size -= len;
  }

  piece_chain_method::size_type piece_chain_method::alloc_buffer( size_type capacity )
  {
    size_type idx = m_buffers.size();
  
    buffer_type* buf = new buffer_type(capacity);
    m_buffers.push_back(buf);
  
    return idx;
  }

  void  piece_chain_method::alloc_modify_buffer( size_type capacity )
  {
    size_type id = alloc_buffer(capacity);
    m_modify_buf.id  = id;
    m_modify_buf.pos = 0;
  }

  piece_chain_method::search_node piece_chain_method::search_piece(size_type pos) const noexcept
  {
    // check head and tail;
    if( pos == 0 )      { return search_node( 0, m_head.next); };
    if( pos == m_size ) { return search_node( m_size, m_tail.prev->next); }

    search_node    s_piece;

    if( m_last_search.node != nullptr  ) 
    {
      piece_node* last_ptr = m_last_search.node;
      size_type   last_idx = m_last_search.pos;

      if( (last_idx <= pos) && (pos < (last_idx + last_ptr->length))  )
      {
        //searched piece is equal to last piece
        s_piece = m_last_search;
      } else
      {
        if( pos < last_idx )
        {
          // search in front of last position
          size_type distance_pos = last_idx - pos;

          if( pos < distance_pos ) { s_piece = search_piece_forwards(pos, m_head, *last_ptr, 0); } 
          else { s_piece = search_piece_backwards( pos, *last_ptr, m_head, last_idx + last_ptr->length); }
        
        } else
        {
          // search in behind last position
          size_type distance_pos = pos - last_idx + last_ptr->length;
          size_type distance_end = m_size - pos;

          if( distance_pos < distance_end ) { s_piece = search_piece_forwards( pos, *last_ptr, m_tail, last_idx + last_ptr->length );} 
          else { s_piece = search_piece_backwards( pos, m_tail, *last_ptr, m_size ); }
        }
      }
    }
    else
    {
      if( pos < m_size/2 ){ s_piece = search_piece_forwards(pos, m_head, m_tail, 0); }
      else{ s_piece = search_piece_backwards(pos, m_tail, m_head, m_size); }
    }
    return s_piece;
  }

  piece_chain_method::search_node piece_chain_method::search_piece_forwards(size_type pos, const piece_node& head_node, const piece_node& tail_node, size_type first_pos) const noexcept
  {
    search_node s_piece;
    size_type   current_pos = first_pos;

    for( s_piece.node = head_node.next; s_piece.node != &tail_node; s_piece.node = s_piece.node->next )
    {
      if( (pos >= current_pos) && (pos < current_pos + s_piece.node->length) )
      { 
        s_piece.pos = current_pos;
        return s_piece;
      }
      current_pos += s_piece.node->length;
    }
    return search_node();
  }

  piece_chain_method::search_node piece_chain_method::search_piece_backwards(size_type pos, const piece_node& tail_node, const piece_node& head_node, size_type last_pos) const noexcept
  {
    search_node s_piece;
    size_type   current_pos = last_pos;

    for( s_piece.node = tail_node.prev; s_piece.node != &head_node; s_piece.node = s_piece.node->prev )
    {
      current_pos -= s_piece.node->length;
      if( (pos >= current_pos) && (pos < current_pos + s_piece.node->length) )
      { 
        s_piece.pos = current_pos;
        return s_piece;
      }
    }
    return search_node();
  }

  void  piece_chain_method::insert_piece(size_type pos, piece_node* new_piece, size_type len)
  {

    search_node   s_node = search_piece(pos);
           m_last_search = s_node;    

    size_type   piece_idx = s_node.pos;
    piece_node* piece_ptr = s_node.node;

    size_type   insert_offset = pos - piece_idx;

    // insert bevor the node
    if(insert_offset == 0)
    {
      piece_node* prev_ptr = piece_ptr->prev;

      // connect prev
      prev_ptr->next = new_piece;
      new_piece->prev = prev_ptr;

      // connect next
      new_piece->next = piece_ptr;
      piece_ptr->prev = new_piece;

      m_last_search.node = new_piece;
    } else 
    {
      // split node in 2 pieces
      piece_node* split_ptr = new piece_node( piece_ptr->pos + insert_offset, piece_ptr->length - insert_offset, piece_ptr->buffer_id );
      piece_ptr->length = insert_offset;

      piece_node* next_ptr = piece_ptr->next;

      // connect new piec with frist part
      piece_ptr->next = new_piece;
      new_piece->prev = piece_ptr;

      // connect new_piece with split
      split_ptr->prev = new_piece;
      new_piece->next = split_ptr;

      // connect split with (old) next;
      split_ptr->next = next_ptr;
      next_ptr->prev  = split_ptr;
    }
  }

  piece_chain_method::string_type piece_chain_method::str() const noexcept
  {
    string_type str;
    piece_node* tmp = m_head.next;

    while(tmp != &m_tail)
    {
      buffer_type* buf = m_buffers[tmp->buffer_id];
      str.append( buf->buffer + tmp->pos, tmp->length );
      tmp = tmp->next;
    }
    return str;
  }

  void piece_chain_method::debug () const noexcept
  {
    piece_node* sptr = nullptr;

    std::cout << "HEAD: <" << std::right << std::setw(16) << &m_head << ">" << std::endl;
    std::cout << "TAIL: <" << std::right << std::setw(16) << &m_tail << ">" << std::endl;

    std::cout << "**********************" << std::endl;
    size_type node_cnt = 0;
    for(sptr = m_head.next; sptr != &m_tail; sptr = sptr->next)
    {
      auto buffer = m_buffers[sptr->buffer_id]->buffer;
      std::cout << "{" << std::setw(10) << sptr->buffer_id << "}"
                << "[" << std::left << std::setw(10) << sptr->pos 
                << "<" << std::right << std::setw(16) << sptr << ">"
                << std::right << std::setw(10) << sptr->length << "]\t"
                << string_type( buffer + sptr->pos, buffer + sptr->pos + sptr->length )
                << std::endl;
      ++node_cnt;
    }

    std::cout << "-------------------------" << std::endl;

    for(sptr = m_tail.prev; sptr != &m_head; sptr = sptr->prev)
    {
      auto buffer = m_buffers[sptr->buffer_id]->buffer;
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
      auto buffer = m_buffers[sptr->buffer_id]->buffer;
      std::cout << string_type( buffer + sptr->pos, buffer + sptr->pos + sptr->length );
    }

    std::cout << std::endl << "piece_chain_method length = " << m_size << " chars " << node_cnt << " nodes" <<  std::endl;
    std::cout << std::endl;
  }


} // sequences