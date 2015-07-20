#ifndef SEQUENCES_PIECE_CHAIN_METHOD_HPP
#define SEQUENCES_PIECE_CHAIN_METHOD_HPP

#include "buffer_controller.hpp"
#include "sequence_interface.hpp"

#include <vector>

namespace sequences
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

    piece_node*   next = nullptr;
    piece_node*   prev = nullptr;
  };

  class piece_chain_method : public sequence_interface
  {
    public:
      using basic_type  = sequence_interface;
      using value_type  = typename basic_type::value_type;
      using size_type   = typename basic_type::size_type;
      using string_type = typename basic_type::string_type;
      using buffer_type       = buffer_controller<value_type>;
      using buffer_list_type  = std::vector<buffer_type*>;


      piece_chain_method() noexcept;
      ~piece_chain_method() noexcept;

      explicit piece_chain_method(size_type size, value_type val);
      
      piece_chain_method(const piece_chain_method&)            = delete;
      piece_chain_method& operator=(const piece_chain_method&) = delete;

      void insert(size_type pos, const value_type* s, size_type len);
      void insert(size_type pos, value_type v) override;
      void insert(size_type pos, size_type len, value_type v);

      void append(const value_type* s, size_type len);
      void append(size_type len, value_type v);

      void erase(size_type pos) override;
      void erase(size_type pos, size_type len);

      void clear() noexcept;

      value_type at(size_type pos) override;
      size_type size() const noexcept override;
      bool empty() const noexcept override;

      string_type str() const noexcept override;
      void debug() const noexcept;
    private:
      size_type         m_size;
      buffer_list_type  m_buffer_list;

      piece_node        m_head;
      piece_node        m_tail;

      struct modify_buffer_type
      {
        size_type  id  = -1 ;
        size_type  pos = -1 ;
      };

      modify_buffer_type    m_modify_buffer;
  
      inline size_type alloc_buffer( size_type capacity );
      inline void  alloc_modify_buffer( size_type capacity );
      inline void insert_piece(size_type pos, piece_node* new_piece, size_type len);

      enum class action 
      {
        invalid = 0,
        insert,
        erase
      };


      // optimizations
      struct last_action
      {
        action      act  = action::invalid;
        size_type   pos  = -1;
        // piece_node* node = nullptr;
      };
      last_action             m_last_act;

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
      search_node               m_last_node;

      inline search_node search_piece(size_type pos) const noexcept;
      inline search_node search_piece_forwards(size_type pos, const piece_node& head_node, const piece_node& tail_node, size_type first_pos) const noexcept;
      inline search_node search_piece_backwards(size_type pos, const piece_node& head_node, const piece_node& tail_node, size_type last_pos) const noexcept;
      // inline search_node search_piece_backwards(size_type pos, piece_node* tail_ptr, piece_node* head_ptr, size_type size) const noexcept;

      inline bool can_optimize(action act, size_type pos);
      inline void record_action (action act, size_type pos);

  };

} // sequences

#endif // SEQUENCES_PIECE_CHAIN_METHOD_HPP
