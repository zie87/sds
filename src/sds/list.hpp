#ifndef LODUR_CONTAINER_LIST_HPP
#define LODUR_CONTAINER_LIST_HPP

// this is an im implementation of a double-linked list with little access 
// optimization the search complexity for given position is worst case O(n/2) 
// and best case O(1). THis list save the last searched position and search 
// backwards if the postion is near to to the last element  


#include <stdexcept>
#include <iterator>

namespace lodur
{
namespace container
{
  template<typename T>
  struct list_node
  {
    using value_type = T;

    list_node() : value() {}
    list_node(value_type val) : value(std::move(val)) {}

    list_node(const list_node& )           = delete;
    list_node& operator=(const list_node&) = delete;

    value_type      value;
    list_node*      next     = nullptr;
    list_node*      previous = nullptr;
  };

  template<typename T>
  struct list_iterator
  {
    using value_type        = T;
    using node_type         = list_node<T>;
    using iterator_category = std::bidirectional_iterator_tag;

    list_iterator& operator++() 
    { 
      m_node = m_node->next;
      return *this;
    }
    list_iterator  operator++(int)
    {  
      list_iterator tmp(*this);
      operator++();
      return tmp;
    }

    list_iterator& operator--() 
    { 
      m_node = m_node->previous;
      return *this;
    }
    list_iterator  operator--(int)
    {  
      list_iterator tmp(*this);
      operator--();
      return tmp;
    }

          value_type& operator*()  { return m_node->value; };
          value_type* operator->() { return &m_node->value; };

    const value_type& operator*() const  { return m_node->value; };
    const value_type* operator->() const { return &m_node->value; };

    template<typename V> friend bool operator==(const list_iterator<V>&, const list_iterator<V>&);
    template<typename V> friend bool operator!=(const list_iterator<V>&, const list_iterator<V>&);

    list_iterator(node_type* node = nullptr) : m_node(node) {}    
    list_iterator(const list_iterator& it)   : m_node(it.m_node) {}

    list_iterator& operator=(const list_iterator& rhs) { m_node = rhs.m_node; }

    private:
      node_type* m_node;
  };
  template<typename T>
  bool operator==(const list_iterator<T>& lhs, const list_iterator<T>& rhs) { return lhs.m_node == rhs.m_node; }
  template<typename T>
  bool operator!=(const list_iterator<T>& lhs, const list_iterator<T>& rhs) { return !(lhs == rhs); }

  template<typename T>
  class list
  {
    public:
      using value_type = T;
      using size_type  = std::size_t;
      using node_type  = list_node<T> ; 
      using iterator   = list_iterator<T>;

      list() noexcept
      : m_head(), m_tail(), m_size(0), m_last_pos()
      {
        m_head.previous = m_tail.next = nullptr;
        m_head.next     = &m_tail;
        m_tail.previous = &m_head;
      }

      ~list() noexcept
      {
        clear();
      }

      list(const list& )           = delete;
      list& operator=(const list&) = delete;

      list(size_type size, value_type val)
      : list()
      {
        for(size_type i = 0; i < size; ++i) push_back(val);
      }

      void clear() noexcept
      {
        node_type* tmp = m_tail.previous;
        while( tmp->previous != nullptr )
        {
          tmp = tmp->previous;
          delete tmp->next;
        }
        m_head.next     = &m_tail;
        m_tail.previous = &m_head;
        m_size = 0;
        m_last_pos.node = nullptr;
      }

      iterator begin()  { return iterator(m_head.next); }
      iterator end()  { return iterator(&m_tail); }

      void push_front( value_type val ) 
      {  
        node_type* new_node = new node_type(val);
        m_head.next->previous = new_node;

        new_node->previous = &m_head;
        new_node->next     = m_head.next;
        m_head.next        = new_node;
        
        ++m_size;
      }

      void push_back( value_type val )
      {
        node_type* new_node = new node_type(std::move(val));
        m_tail.previous->next = new_node;

        new_node->previous = m_tail.previous;
        new_node->next     = &m_tail;
        m_tail.previous   = new_node;
      
        ++m_size;
      }

      void pop_front()
      {
        if( empty() ) throw std::out_of_range("Can not delete from empty list!");

        node_type* delete_node = m_head.next;

        m_head.next           = delete_node->next;
        m_head.next->previous = & m_head;

        delete delete_node;
        --m_size;
      }

      void pop_back()
      {
        if( empty() ) throw std::out_of_range("Can not delete from empty list!");

        node_type* delete_node = m_tail.previous;
        m_tail.previous       = delete_node->previous;
        m_tail.previous->next = &m_tail;

        delete delete_node;
        --m_size;
      }

      void insert(size_type pos, value_type val)
      {
        set_position(pos);

        node_type* new_node = new node_type(val);

        node_type* tmp     = m_last_pos.node->previous;
        tmp->next          = new_node;
        new_node->previous = tmp; 

        new_node->next              = m_last_pos.node;
        m_last_pos.node->previous   = new_node;

        m_last_pos.position += 1; 
        ++m_size;
      }

      void erase(size_type pos)
      {
        set_position(pos);

        node_type* tmp = m_last_pos.node->previous;

        tmp->next = m_last_pos.node->next;
        tmp->next->previous = tmp;

        tmp = m_last_pos.node;
        m_last_pos.node = m_last_pos.node->next;
        delete tmp; --m_size;
      };

      value_type front()
      {
        return m_head.next->value;
      }

      value_type back()
      {
        return m_tail.previous->value;
      }

      value_type& at(size_type pos)
      {
        if( pos > m_size ) throw std::out_of_range("Position out of list!");
        return (*this)[pos];
      }

      value_type& operator[]( size_type pos )
      {
        set_position(pos);
        return m_last_pos.node->value;
      }

      const value_type& at(size_type pos) const
      {
        if( pos > m_size ) throw std::out_of_range("Position out of list!");
        return (*this)[pos];
      }
      const value_type& operator[]( size_type pos ) const { return get_position(pos)->value; }

      void copy( value_type* array ) const
      {
        node_type* tmp = m_head.next;
        for( unsigned int i = 0; i < m_size; ++i )
        {
          array[i] = tmp->value;
          tmp = tmp->next;
        }
      }

      size_type size()  const noexcept { return m_size; }
      bool      empty() const noexcept { return m_head.next == nullptr; }

    private:
      struct position
      {
        node_type*     node      = nullptr;
        size_type position       = 0;
      };

      node_type* get_position(size_type pos) const
      {
        if( pos > m_size/2 ) { return get_pos_from_back(pos); }
        else { return get_pos_from_front(pos); }
      } 

      node_type* get_pos_from_front(size_type pos) const
      {
        node_type* tmp = m_head.next;
        for( size_type i = 1; i <= pos; ++i )
        {
          tmp = tmp->next;
        }
        return tmp;
      }

      node_type* get_pos_from_back(size_type pos) const
      {
        node_type* tmp = m_tail.previous;
        for( size_type i = m_size - 1; i > pos; --i )
        {
          tmp = tmp->previous;
        }
        return tmp;
      }

      inline void set_position(size_type pos)
      {
        if( pos > m_size ) throw std::out_of_range("Position out of list!");

        if( m_last_pos.node == nullptr )
        {        
          if( pos > m_size/2 ) { set_pos_from_back(pos, &m_tail, m_size); }
          else { set_pos_from_front(pos, &m_head, 0); }
        } else
        {
          // calc min last_pas begin end
          if( pos == m_last_pos.position) { return; }
          if( pos > m_last_pos.position )
          {
            size_type distance_pos = pos - m_last_pos.position;
            size_type distance_end = m_size - pos;

            if( distance_pos < distance_end ) { set_pos_from_back( pos, m_last_pos.node, m_last_pos.position ); }
            else{ set_pos_from_back(pos, &m_tail, m_size); }

          } else
          {
            size_type distance_pos = m_last_pos.position - pos;

            if( pos < distance_pos ) { set_pos_from_front(pos, m_last_pos.node, m_last_pos.position); }
            else { set_pos_from_front(pos, &m_head, 0); }
          }
        }
      }

      inline void set_pos_from_back(size_type pos, node_type* start, size_type start_pos)
      {
        node_type* tmp = start;
        for( size_type i = start_pos; i > pos; --i )
        {
          tmp = tmp->previous;
        }
        m_last_pos.node     = tmp;
        m_last_pos.position = pos;
      }


      inline void set_pos_from_front(size_type pos, node_type* start, size_type start_pos)
      {
        node_type* tmp = start;
        for( size_type i = start_pos; i <= pos; ++i )
        {
          tmp = tmp->next;
        }
        m_last_pos.node     = tmp;
        m_last_pos.position = pos;
      }

      node_type  m_head;
      node_type  m_tail;

      size_type m_size;
      position  m_last_pos;

  };
} // container
} // lodur


#endif // LODUR_CONTAINER_LIST_HPP