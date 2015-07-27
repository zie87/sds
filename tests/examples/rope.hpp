#ifndef EXAMPLES_ROPE_METHOD_HPP
#define EXAMPLES_ROPE_METHOD_HPP

#include <string>
#include <list>

namespace examples
{

  class rope
  {
    public:
      using value_type        = char;
      using size_type         = std::size_t;
      using difference_type   = std::ptrdiff_t;
      using string_type       = std::basic_string<value_type>;

      rope(const string_type&);
      rope(const value_type*, size_type len);
      rope(const rope&);
      rope(rope&&);

      explicit rope(size_type size, value_type val);

      ~rope();

      rope& operator=(rope r);

      value_type at(size_type);
      void insert(size_type, const value_type*, size_type len);
      void insert(size_type, const string_type&);

      void append(const value_type*, size_type);
      void append(size_type s, value_type val);

      size_type size() const noexcept;
      bool empty() const noexcept { return size() == 0; }

      void debug();

      void copy(value_type*, size_type) const;
      void consolidate(); // make one contiguous node.
    private:

      struct rope_node
      {
        ~rope_node();
        rope_node();
        void debug(std::ostream&, size_type);

        rope_node(const rope_node&) = delete;
        rope_node& operator=(const rope_node) = delete;

        const value_type* fragment;
        rope_node *left, *right;
        size_type weight = 0;
      };

      inline void update_weights(rope_node*) const;
      inline size_type internal_copy(rope_node*, value_type*, size_type) const;
      inline size_type weight_sum(rope_node*) const;


      inline value_type* alloc_buffer(size_type len );
      inline value_type* alloc_buffer(const value_type* s, size_type len );

      rope_node*                   m_root = nullptr;
      std::list<const value_type*> m_delete_list;
};

} // examples


#endif // EXAMPLES_ROPE_METHOD_HPP