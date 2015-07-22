#ifndef TEST_CHAIN_HPP
#define TEST_CHAIN_HPP

#include <sds/sequence_interface.hpp>
#include <sds/buffer_controller.hpp>

#include <vector>
#include <iostream>

namespace test
{

  struct piece_node
  {
    using size_type         = long unsigned int;

    piece_node(size_type p = 0, size_type o = 0, size_type b = 0) noexcept;

    piece_node(const piece_node&)             = delete;
    piece_node& operator=(const piece_node&)  = delete;

    size_type   pos;
    size_type   length;
    size_type   buffer_id;

    piece_node* next = nullptr;
    piece_node* prev = nullptr;
  };

  class piece_chain_method : public ::sequences::sequence_interface
  {
    public:
      using basic_type  = ::sequences::sequence_interface;
      using value_type  = typename basic_type::value_type;

      using pointer     = value_type*;
      using reference   = value_type&;

      using const_pointer     = const value_type*;
      using const_reference   = const value_type&;

      using size_type       = typename basic_type::size_type;
      using difference_type = typename basic_type::difference_type;


      using string_type = typename basic_type::string_type;
      using buffer_type       = ::sequences::buffer_controller<value_type>;
      using buffer_list_type  = std::vector<buffer_type*>;

      struct       iterator;
      struct const_iterator;

      // using       reverse_iterator = std::reverse_iterator<iterator>;
      // using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      friend class       iterator;
      friend class const_iterator;

      struct iterator
      {
        using value_type  = typename piece_chain_method::value_type;
        using pointer     = typename piece_chain_method::pointer;
        using reference   = typename piece_chain_method::reference;

        using size_type       = typename piece_chain_method::size_type;
        using difference_type = typename piece_chain_method::difference_type;

        using iterator_category = std::bidirectional_iterator_tag;

        iterator(const piece_chain_method* parent) noexcept : m_parent(parent) {};
        iterator(const iterator& it) noexcept
        : m_parent(it.m_parent), m_pos( it.m_pos ), m_node(it.m_node) 
        {}
        ~iterator() noexcept = default;
      
        iterator& operator=(const iterator& it) noexcept {  m_parent = it.m_parent; m_pos = it.m_pos; m_node = it.m_node; return *this; }
        bool operator==(const iterator& it) const  { return (m_parent == it.m_parent ) && (m_pos == it.m_pos) && (m_node == it.m_node); }
        bool operator!=(const iterator& it) const { return !( *this == it ); }

        iterator& operator++() 
        {  
          if(m_pos == 0 || m_pos < (m_node->length -1 ) ) ++m_pos;
          else
          {
            m_pos = 0;
            m_node = m_node->next;
          }
          return *this;
        }
        // iterator operator++(int); //optional
        iterator& operator--()
        {
          if( m_pos == 0 )
          {
            m_node = m_node->prev;
            m_pos  = m_node->length -1;
          } else { --m_pos; }
          return *this;
        }
        // iterator operator--(int); //optional

        reference operator*() const
        {
          piece_chain_method::buffer_type* buf = m_parent->m_buffers[ m_node->buffer_id ];
          return  buf->buffer[ m_node->pos + m_pos ];
        }
        pointer operator->() const
        {
          piece_chain_method::buffer_type* buf = m_parent->m_buffers[ m_node->buffer_id ];
          return  &buf->buffer[ m_node->pos + m_pos ];
        }
        private:
          iterator(const piece_chain_method* parent, piece_node* node, size_type pos) noexcept
          : m_parent(parent), m_pos(pos), m_node( node )
          {
            if(m_pos > 0 && m_pos == m_node->length) m_pos = ( m_node->length - 1);
          }

          const piece_chain_method* m_parent;

          size_type     m_pos      = 0;
          piece_node*   m_node     = nullptr;

          friend class piece_chain_method;
          friend class piece_chain_method::const_iterator;
      };

      struct const_iterator 
      {
        using value_type  = typename piece_chain_method::value_type;
        using pointer     = typename piece_chain_method::const_pointer;
        using reference   = typename piece_chain_method::const_reference;

        using size_type       = typename piece_chain_method::size_type;
        using difference_type = typename piece_chain_method::difference_type;

        using iterator_category = std::bidirectional_iterator_tag;

        const_iterator(const piece_chain_method* parent) noexcept : m_parent(parent) {};
        const_iterator(const iterator& it) noexcept
        : m_parent(it.m_parent), m_pos( it.m_pos ), m_node(it.m_node) 
        {}
        const_iterator(const const_iterator& it) noexcept
        : m_parent(it.m_parent), m_pos( it.m_pos ), m_node(it.m_node) 
        {}
        ~const_iterator() noexcept = default;

        const_iterator& operator=(const const_iterator& it) noexcept { m_parent = it.m_parent; m_pos = it.m_pos; m_node = it.m_node; return *this; }
        bool operator==(const const_iterator& it) const { return (m_parent == it.m_parent ) && (m_pos == it.m_pos) && (m_node == it.m_node); }
        bool operator!=(const const_iterator& it) const { return !( *this == it ); }

        const_iterator& operator++()
        {  
          if(m_pos == 0 || m_pos < (m_node->length - 1) ) ++m_pos;
          else
          {
            m_pos = 0;
            m_node = m_node->next;
          }
          return *this;
        }
        // const_iterator operator++(int); //optional
        const_iterator& operator--()
        {
          if( m_pos == 0 )
          {
            m_node = m_node->prev;
            m_pos  = m_node->length -1;
          } else { --m_pos; }
          return *this;
        }
        // const_iterator operator--(int); //optionall

        reference operator*() const
        {
          piece_chain_method::buffer_type* buf = m_parent->m_buffers[ m_node->buffer_id ];
          return  buf->buffer[ m_node->pos + m_pos ];
        }
        pointer operator->() const
        {
          piece_chain_method::buffer_type* buf = m_parent->m_buffers[ m_node->buffer_id ];
          return  &buf->buffer[ m_node->pos + m_pos ];
        }
        private:
          const_iterator(const piece_chain_method* parent, piece_node* node, size_type pos) noexcept
          : m_parent(parent), m_pos(pos), m_node( node )
          {
            if(m_pos > 0 && m_pos == m_node->length) m_pos = ( m_node->length - 1);
          }

          const piece_chain_method* m_parent;
          size_type     m_pos      = 1;
          piece_node*   m_node     = nullptr;

          friend class piece_chain_method;
      };




      piece_chain_method() noexcept;
      ~piece_chain_method() noexcept;

      explicit piece_chain_method(size_type size, value_type val);
      
      piece_chain_method(const piece_chain_method&)            = delete;
      piece_chain_method& operator=(const piece_chain_method&) = delete;

            iterator begin()       noexcept { return       iterator( this, m_head.next, 0 ); }
      const_iterator begin() const noexcept { return const_iterator( this, m_head.next, 0 ); }

      //       reverse_iterator rbegin()       noexcept { return       reverse_iterator( iterator( this, &m_head, 0 ) ); }
      // const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator( const_iterator( this, m_head.next->prev, 0 ) ); }

            iterator end()       noexcept { return       iterator( this, &m_tail, 0 ); }
      const_iterator end() const noexcept { return const_iterator( this, m_tail.prev->next, 0 ); }

      //       reverse_iterator rend()       noexcept { return       reverse_iterator( iterator( this, m_tail.prev, m_tail.prev->length -1 ) ); }
      // const_reverse_iterator rend() const noexcept { return const_reverse_iterator( const_iterator( this, m_tail.prev, m_tail.prev->length -1 ) ); }


      value_type at(size_type pos) override;
      size_type size() const noexcept override;
      bool empty() const noexcept override;

      void insert(size_type pos, const value_type* s, size_type len);
      void insert(size_type pos, value_type v) override;
      void insert(size_type pos, size_type len, value_type v);

      void append(const value_type* s, size_type len);
      void append(size_type len, value_type v);

      void erase(size_type pos) override;
      void erase(size_type pos, size_type len);

      void clear() noexcept;

      string_type str() const noexcept override;
      void debug() const noexcept;

    private:
      piece_node m_head;
      piece_node m_tail;

      buffer_list_type m_buffers;

      struct modify_buffer_type
      {
        size_type  id  = -1 ;
        size_type  pos = -1 ;
      };

      modify_buffer_type    m_modify_buf;
  
      inline size_type alloc_buffer( size_type capacity );
      inline void  alloc_modify_buffer( size_type capacity );

      struct search_node
      {
        search_node() = default;
        search_node(size_type p, piece_node* n)
        : pos(p), node(n)
        {}

        search_node(const search_node& n)
        : pos(n.pos), node(n.node)
        {}

        search_node& operator=(const search_node& n)
        {
          pos  = n.pos;
          node = n.node;
          return *this;
        }

        size_type   pos  = -1;
        piece_node* node = nullptr;
      };
      search_node               m_last_search;

      inline search_node search_piece(size_type pos) const noexcept;
      inline search_node search_piece_forwards(size_type pos, const piece_node& head_node, const piece_node& tail_node, size_type first_pos) const noexcept;
      inline search_node search_piece_backwards(size_type pos, const piece_node& tail_node, const piece_node& head_node, size_type last_pos)const noexcept;

      inline void insert_piece(size_type pos, piece_node* new_piece, size_type len);

      size_type   m_size;
  };

} // test

#endif // TEST_CHAIN_HPP