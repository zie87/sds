#ifndef TEST_CHAIN_HPP
#define TEST_CHAIN_HPP

#include "buf_interface.hpp"

#include <sds/buffer_controller.hpp>

#include <vector>

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

    void set_next(piece_node* new_piece) noexcept ;
    void set_prev(piece_node* new_piece) noexcept ;

    piece_node* next = nullptr;
    piece_node* prev = nullptr;
  };

  class piece_chain : public buf_interface
  {
    public:
      using basic_type  = buf_interface;
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

      using       reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      friend class       iterator;
      friend class const_iterator;

      struct iterator
      {
        using value_type  = typename piece_chain::value_type;
        using pointer     = typename piece_chain::pointer;
        using reference   = typename piece_chain::reference;

        using size_type       = typename piece_chain::size_type;
        using difference_type = typename piece_chain::difference_type;

        using iterator_category = std::bidirectional_iterator_tag;

        iterator(const piece_chain* parent) noexcept : m_parent(parent) {};
        iterator(const iterator& it) noexcept
        : m_parent(it.m_parent), m_pos( it.m_pos ), m_node(it.m_node) 
        {}
        ~iterator() noexcept = default;
      
        iterator& operator=(const iterator& it) noexcept { m_parent = it.m_parent; m_pos = it.m_pos; m_node = it.m_node; return *this; }
        bool operator==(const iterator& it) const { return (m_parent == it.m_parent ) && (m_pos == it.m_pos) && (m_node == it.m_node); }
        bool operator!=(const iterator& it) const { return !( *this == it ); }

        iterator& operator++() 
        {  
          if(m_pos == 0 || m_pos < (m_node->length - 1) ) ++m_pos;
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
          piece_chain::buffer_type* buf = m_parent->m_buffers[ m_node->buffer_id ];
          return  buf->buffer[ m_node->pos + m_pos + 1 ];
        }
        pointer operator->() const
        {
          piece_chain::buffer_type* buf = m_parent->m_buffers[ m_node->buffer_id ];
          return  &buf->buffer[ m_node->pos + m_pos + 1 ];
        }
        private:
          iterator(const piece_chain* parent, piece_node* node, size_type pos) noexcept
          : m_parent(parent), m_pos(pos), m_node( node )
          {
            if(m_pos > 0 && m_pos == m_node->length) m_pos = ( m_node->length - 1);
          }

          const piece_chain* m_parent;

          size_type     m_pos      = 0;
          piece_node*   m_node     = nullptr;

          friend class piece_chain;
          friend class piece_chain::const_iterator;
      };

      struct const_iterator 
      {
        using value_type  = typename piece_chain::value_type;
        using pointer     = typename piece_chain::const_pointer;
        using reference   = typename piece_chain::const_reference;

        using size_type       = typename piece_chain::size_type;
        using difference_type = typename piece_chain::difference_type;

        using iterator_category = std::bidirectional_iterator_tag;

        const_iterator(const piece_chain* parent) noexcept : m_parent(parent) {};
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
          piece_chain::buffer_type* buf = m_parent->m_buffers[ m_node->buffer_id ];
          return  buf->buffer[ m_node->pos + m_pos + 1 ];
        }
        pointer operator->() const
        {
          piece_chain::buffer_type* buf = m_parent->m_buffers[ m_node->buffer_id ];
          return  &buf->buffer[ m_node->pos + m_pos + 1 ];
        }
        private:
          const_iterator(const piece_chain* parent, piece_node* node, size_type pos) noexcept
          : m_parent(parent), m_pos(pos), m_node( node )
          {
            if(m_pos > 0 && m_pos == m_node->length) m_pos = ( m_node->length - 1);
          }

          const piece_chain* m_parent;
          size_type     m_pos      = 1;
          piece_node*   m_node     = nullptr;

          friend class piece_chain;
      };




      piece_chain() noexcept;

            iterator begin()       noexcept { return       iterator( this, m_head.next, 0 ); }
      const_iterator begin() const noexcept { return const_iterator( this, m_head.next, 0 ); }

            reverse_iterator rbegin()       noexcept { return       reverse_iterator( iterator( this, &m_head, 0 ) ); }
      const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator( const_iterator( this, m_head.next->prev, 0 ) ); }

            iterator end()       noexcept { return       iterator( this, &m_tail, 0 ); }
      const_iterator end() const noexcept { return const_iterator( this, m_tail.prev->next, 0 ); }

            reverse_iterator rend()       noexcept { return       reverse_iterator( iterator( this, m_tail.prev, m_tail.prev->length  ) ); }
      const_reverse_iterator rend() const noexcept { return const_reverse_iterator( const_iterator( this, m_tail.prev, m_tail.prev->length  ) ); }

      bool empty() const noexcept override;

    private:
      piece_node m_head;
      piece_node m_tail;

      buffer_list_type m_buffers;
  };

} // test

#endif // TEST_CHAIN_HPP

/*

template <class T, class A = std::allocator<T> >
class X {
public:
    typedef A allocator_type;
    typedef typename A::value_type value_type; 
    typedef typename A::reference reference;
    typedef typename A::const_reference const_reference;
    typedef typename A::difference_type difference_type;
    typedef typename A::size_type size_type;

    class iterator { 
    public:
        typedef typename A::difference_type difference_type;
        typedef typename A::value_type value_type;
        typedef typename A::reference reference;
        typedef typename A::pointer pointer;
        typedef std::random_access_iterator_tag iterator_category; //or another tag

        iterator();
        iterator(const iterator&);
        ~iterator();

        iterator& operator=(const iterator&);
        bool operator==(const iterator&) const;
        bool operator!=(const iterator&) const;
        bool operator<(const iterator&) const; //optional
        bool operator>(const iterator&) const; //optional
        bool operator<=(const iterator&) const; //optional
        bool operator>=(const iterator&) const; //optional

        iterator& operator++();
        iterator operator++(int); //optional
        iterator& operator--(); //optional
        iterator operator--(int); //optional
        iterator& operator+=(size_type); //optional
        iterator operator+(size_type) const; //optional
        friend iterator operator+(size_type, const iterator&); //optional
        iterator& operator-=(size_type); //optional            
        iterator operator-(size_type) const; //optional
        difference_type operator-(iterator) const; //optional

        reference operator*() const;
        pointer operator->() const;
        reference operator[](size_type) const; //optional
    };
    class const_iterator {
    public:
        typedef typename A::difference_type difference_type;
        typedef typename A::value_type value_type;
        typedef typename A::reference const_reference;
        typedef typename A::pointer const_pointer;
        typedef std::random_access_iterator_tag iterator_category; //or another tag

        const_iterator ();
        const_iterator (const const_iterator&);
        const_iterator (const iterator&);
        ~const_iterator();

        const_iterator& operator=(const const_iterator&);
        bool operator==(const const_iterator&) const;
        bool operator!=(const const_iterator&) const;
        bool operator<(const const_iterator&) const; //optional
        bool operator>(const const_iterator&) const; //optional
        bool operator<=(const const_iterator&) const; //optional
        bool operator>=(const const_iterator&) const; //optional

        const_iterator& operator++();
        const_iterator operator++(int); //optional
        const_iterator& operator--(); //optional
        const_iterator operator--(int); //optional
        const_iterator& operator+=(size_type); //optional
        const_iterator operator+(size_type) const; //optional
        friend const_iterator operator+(size_type, const const_iterator&); //optional
        const_iterator& operator-=(size_type); //optional            
        const_iterator operator-(size_type) const; //optional
        difference_type operator-(const_iterator) const; //optional

        const_reference operator*() const;
        const_pointer operator->() const;
        const_reference operator[](size_type) const; //optional
    };

    typedef std::reverse_iterator<iterator> reverse_iterator; //optional
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator; //optional

    X();
    X(const X&);
    ~X();

    X& operator=(const X&);
    bool operator==(const X&) const;
    bool operator!=(const X&) const;
    bool operator<(const X&) const; //optional
    bool operator>(const X&) const; //optional
    bool operator<=(const X&) const; //optional
    bool operator>=(const X&) const; //optional

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;
    reverse_iterator rbegin(); //optional
    const_reverse_iterator rbegin() const; //optional
    const_reverse_iterator crbegin() const; //optional
    reverse_iterator rend(); //optional
    const_reverse_iterator rend() const; //optional
    const_reverse_iterator crend() const; //optional

    reference front(); //optional
    const_reference front() const; //optional
    reference back(); //optional
    const_reference back() const; //optional
    template<class ...Args>
    void emplace_front(Args...); //optional
    template<class ...Args>
    void emplace_back(Args...); //optional
    void push_front(const T&); //optional
    void push_front(T&&); //optional
    void push_back(const T&); //optional
    void push_back(T&&); //optional
    void pop_front(); //optional
    void pop_back(); //optional
    reference operator[](size_type); //optional
    const_reference operator[](size_type) const; //optional
    reference at(size_type); //optional
    const_reference at(size_type) const; //optional

    template<class ...Args>
    iterator emplace(const_iterator, Args...); //optional
    iterator insert(const_iterator, const T&); //optional
    iterator insert(const_iterator, T&&); //optional
    iterator insert(const_iterator, size_type, T&); //optional
    template<class iter>
    iterator insert(const_iterator, iter, iter); //optional
    iterator insert(const_iterator, std::initializer_list<T>); //optional
    iterator erase(const_iterator); //optional
    iterator erase(const_iterator, const_iterator); //optional
    void clear(); //optional
    template<class iter>
    void assign(iter, iter); //optional
    void assign(std::initializer_list<T>); //optional
    void assign(size_type, const T&); //optional

    void swap(const X&);
    size_type size();
    size_type max_size();
    bool empty();

    A get_allocator(); //optional
};
template <class T, class A = std::allocator<T> >
void swap(X<T,A>&, X<T,A>&); //optional

*/