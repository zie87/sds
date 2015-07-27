#include "rope.hpp"

#include <stdexcept> 
#include <string>
#include <iostream>
#include <list>
#include <algorithm>

namespace examples
{
  rope::rope(const value_type* s, size_type len)
  : rope(string_type(s, len))
  {}

  rope::rope(const string_type& s)
  : m_delete_list()
  {
    value_type* permanent_buffer = alloc_buffer( s.c_str(), s.size()+1 );

    m_root = new rope_node;

    m_root->fragment = nullptr;
    m_root->left = m_root->right = nullptr;
    m_root->weight = s.size();;

    m_root->left = new rope_node;
    m_root->left->fragment = permanent_buffer;
    m_root->left->left = m_root->left->right = nullptr;
    m_root->left->weight = s.size();
  }

  rope::rope(const rope& other)
  : m_delete_list()
  {
    value_type* buffer = alloc_buffer( other.size() );
    other.copy(buffer,other.size());
    
    m_root = new rope_node;
    m_root->left = new rope_node;

    m_root->weight = m_root->left->weight = other.size();
    m_root->left->fragment = buffer;
  }

  rope::rope(size_type size, value_type val)
  : m_delete_list()
  {
    value_type* buffer = alloc_buffer( size );
    std::fill_n(buffer, size, val);
    
    m_root = new rope_node;
    m_root->left = new rope_node;

    m_root->weight = m_root->left->weight = size;
    m_root->left->fragment = buffer;
  }

  rope::rope(rope&& other)
  : m_root(other.m_root), m_delete_list(other.m_delete_list)
  {
    other.m_root = nullptr;
    other.m_delete_list.clear();
  }

  rope::~rope()
  {
    if ( m_root ) delete m_root;
    for ( auto it = m_delete_list.begin(); it != m_delete_list.end(); ++it )
        delete[] *it;
  }


  rope& rope::operator=(rope r)
  {
    std::swap( m_delete_list, r.m_delete_list );
    std::swap( m_root, r.m_root );
    return *this;
  }


  rope::rope_node::rope_node()
  : fragment(nullptr), left(nullptr), right(nullptr)
  {}

  rope::rope_node::~rope_node()
  {
    if ( left ) delete left;
    if ( right ) delete right;
  }

  rope::value_type rope::at(size_type pos) // zero-based index
  {
    if(pos > size() ) throw std::out_of_range("at position higher then size! (rope)");
    size_type current_pos = pos + 1; // We deal with size values, 1-based index
    rope_node* current = m_root->left; // m_root has only a left child

    while ( !current->fragment )
    {
      if ( current_pos > current->weight )
      {
        current_pos -= current->weight;
        current = current->right;
      } else if ( current_pos <= current->weight )
      {
        current = current->left;
      }
    }
    --current_pos; // address array, 0-based index

    return current->fragment[current_pos];
  }

  rope::value_type* rope::alloc_buffer(size_type len )
  {
    value_type* new_s = new value_type[len];
    m_delete_list.push_back(new_s);
    return new_s;
  }

  rope::value_type* rope::alloc_buffer(const value_type* s, size_type len )
  {
    value_type* new_s = alloc_buffer(len);
    std::copy_n( s, len, new_s );
    return new_s;
  }

  void rope::append(const value_type* s, size_type len)
  { 
    value_type* new_s = alloc_buffer(s, len);

    rope_node* new_root = new rope_node;
    rope_node* new_cont = new rope_node;

    new_cont->fragment = new_s;
    new_cont->weight = len;

    new_root->left = m_root;
    m_root->right = new_cont;

    new_root->weight = new_cont->weight + m_root->weight;

    m_root = new_root;
  }

  void rope::append(size_type s, value_type val)
  {
    value_type* new_s = alloc_buffer(s);
    std::fill_n(new_s, s, val);

    rope_node* new_root = new rope_node;
    rope_node* new_cont = new rope_node;

    new_cont->fragment = new_s;
    new_cont->weight = s;

    new_root->left = m_root;
    m_root->right = new_cont;

    new_root->weight = new_cont->weight + m_root->weight;

    m_root = new_root;
  }

  void rope::insert(size_type pos, const string_type& s) { insert(pos,s.c_str(),s.size()); }
  void rope::insert(size_type pos, const value_type* s, size_type len)
  {
    if(pos > size() ) throw std::out_of_range("insert position higher then size! (rope)");
    if ( pos == m_root->weight ) // append
    {
      append( s, len );
    } else
    {
      value_type* new_s = alloc_buffer(s, len);
      size_type current_pos = pos + 1;
      rope_node* current = m_root;

      while (  ! current->fragment )
      {
        if ( current_pos > current->weight )
        {
          current_pos -= current->weight;
          current = current->right;
        } else
        {
          current->weight += len; // already update tree
          current = current->left;
        }
      }

      // Those nodes are attached to current.
      rope_node* new_content = new rope_node;
      new_content->fragment  = new_s;
      new_content->weight    = len;

      rope_node* new_left    = new rope_node;
      rope_node* new_right   = new rope_node;

      new_left->fragment = nullptr;
      new_left->weight = current_pos;
      new_left->right = new_content;
      new_left->left = new rope_node;
      new_left->left->fragment = current->fragment;
      new_left->left->weight = current_pos;

      new_right->fragment = current->fragment + current_pos;
      new_right->weight = current->weight - current_pos;

      current->fragment = nullptr;
      current->left = new_left;
      current->right = new_right;
      update_weights(current);
    }
  }

  rope::size_type rope::size() const noexcept { return m_root->weight; }

  void rope::consolidate()
  {
    value_type* new_contiguous_string = new value_type[size()];
    size_type original_size = size();
    copy(new_contiguous_string,size());

    // Free memory.
    delete m_root;

    std::for_each(m_delete_list.begin(),m_delete_list.end(),[](const value_type* p) { delete[] p; });
    m_delete_list.clear();
    
    // New tree
    rope_node* m_root = new rope_node, *left = new rope_node;

    m_root->left = left;
    left->weight = m_root->weight = original_size;
    left->fragment = new_contiguous_string;

    m_delete_list.push_back(new_contiguous_string);
  }

  void rope::copy(value_type* buffer, size_type size) const
  {
    internal_copy(m_root,buffer,size);
    buffer[size-1] = '\0';
  }

  void rope::debug()
  {
    std::cout << m_root->weight << " characters in rope.\n";
    m_root->left->debug(std::cout,0);

  }

  void rope::rope_node::debug(std::ostream& out, size_type level)
  {
    if ( !this ) return;

    for ( size_type i = 0; i < level; i++ ) out << "│   ";

    if ( left ) out << "├── " << string_type(fragment ? fragment : "<node>" ,fragment ? weight : 6) << " (" << weight << ")\n";
    else  out << "└── " << string_type(fragment ? fragment : "<node>",fragment ? weight : 6) << " (" << weight << ")\n";
    
    right->debug(out,level+1);
    left->debug(out,level);
  }

  void rope::update_weights(rope_node* n) const
  {
    if ( n->left )
    {
      update_weights(n->left);
      n->weight = weight_sum(n->left);
    }
    if ( n->right )
    {
      update_weights(n->right);
    }

  }

  rope::size_type rope::weight_sum(rope_node* n) const
  {
    if ( n->fragment ) return n->weight;
    else
    {
      size_type sum = 0;
      if ( n->left )  sum += weight_sum(n->left);
      if ( n->right ) sum += weight_sum(n->right);
      return sum;
    }

  }

  rope::size_type rope::internal_copy(rope_node* n, value_type* buffer, size_type len) const
  {
    if ( len == 0 || ! n ) return 0;

    if ( ! n->fragment )
    {
      size_type copied = internal_copy(n->left,buffer,len);
      size_type copied2 = internal_copy(n->right,buffer+copied,len-copied);
        
      return copied + copied2;
    } else
    {
      size_type to_copy = n->weight < len ? n->weight : len;
      std::copy_n(n->fragment, to_copy, buffer);

      return to_copy;
    }
  }
} // sequence