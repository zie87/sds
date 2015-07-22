#ifndef NEATPAD_PIECE_CHAIN_HPP
#define NEATPAD_PIECE_CHAIN_HPP

#include <vector>
#include <string>

#include "piece.hpp"
#include "buffer_control.hpp"

namespace neatpad
{

//  piece_chain class!
class piece_chain
{
  public:
    using size_type   = unsigned long int;
    using value_type  = char;
    using string_type = std::basic_string<value_type>;

    static const size_type MAX_SEQUENCE_LENGTH;

    //  temporary 'reference' to the piece_chain, used for
    //  non-const array access with piece_chain::operator[]
    struct ref
    {
      ref(piece_chain *s, size_type i) noexcept
      : index(i), seq(s) 
      {}

      operator value_type() const { return seq->peek(index); }

      ref(const ref& r)
      : index( r.index ), seq( r.seq )
      {}

      ref& operator=(value_type rhs) 
      { 
        seq->poke(index, rhs); 
        return *this; 
      }

      private:
        size_type     index;
        piece_chain*  seq;
    };

    piece_chain() noexcept;
    ~piece_chain() noexcept;

    piece_chain( const piece_chain& )           = delete;
    piece_chain& operator=(const piece_chain&)  = delete;

    // initialize with a file
    bool    init();
    bool    clear() noexcept;

    // initialize from an in-memory buffer
    bool    init(const value_type* buffer, size_type length);


    //  piece_chain statistics
    size_type   size() const noexcept;
    
    // piece_chain manipulation 
    bool    insert (size_type index, const value_type* buf, size_type length);
    bool    insert (size_type index, const value_type  val, size_type count);
    bool    insert (size_type index, const value_type  val);
    bool    replace(size_type index, const value_type* buf, size_type length, size_type erase_length);
    bool    replace(size_type index, const value_type* buf, size_type length);
    bool    replace(size_type index, const value_type  val, size_type count);
    bool    replace(size_type index, const value_type  val);
    bool    erase  (size_type index, size_type len);
    bool    erase  (size_type index);
    bool    append (const value_type* buf, size_type len);
    bool    append (const value_type val);
    void    breakopt();

    // undo/redo support
    bool    undo();
    bool    redo();
    bool    canundo() const noexcept;
    bool    canredo() const noexcept;

    size_type   event_index()  const noexcept { return undoredo_index; }
    size_type   event_length() const noexcept { return undoredo_length; }

    // print out the piece_chain
    void    debug1();
    void    debug2();

    // access and iteration
    size_type   render(size_type index, value_type* buf, size_type len) const;
    value_type   peek(size_type index) const;
    bool    poke(size_type index, value_type val);

    ref at( size_type index ); 

    value_type   operator[] (size_type index) const;
    ref     operator[] (size_type index);

  private:
    using eventstack = std::vector<piece_range*>;
    using bufferlist = std::vector<buffer_control*>;

    template <class type> void clear_vector(type &source);

    //  Span-table management
    void      deletefrompiece_chain(piece **sptr);
    piece*    piecefromindex(size_type index, size_type *pieceindex) const;
    void      scan(piece *sptr);

    size_type     piece_chain_length = 0;
    piece*  head  = nullptr;
    piece*  tail  = nullptr; 
    piece*  frag1 = nullptr;
    piece*  frag2 = nullptr;

    

    //  Undo and redo stacks
    piece_range*   initundo(size_type index, size_type length, action act);
    void           restore_piece_range(piece_range* range, bool undo_or_redo);
    void           swap_piece_range(piece_range* src, piece_range* dest);
    bool           undoredo(eventstack &source, eventstack &dest);
    void           clearstack(eventstack &source) noexcept;
    piece_range*   stackback(eventstack &source, size_type idx);


    void    group();
    void    ungroup();

    size_type     group_id         = 0 ;
    size_type     group_refcount   = 0 ;
    size_type     undoredo_index   = 0 ;
    size_type     undoredo_length  = 0 ;

    bool          can_quicksave    = false;
    eventstack    undostack;
    eventstack    redostack;

    //  File and memory buffer management
    buffer_control* alloc_buffer(size_type size);
    buffer_control* alloc_modify_buffer(size_type size);
    bool      import_buffer(const value_type* buf, size_type len, size_type *buffer_offset);

    bufferlist    buffer_list;
    int  modify_buffer_id  = 0;
    int  modify_buffer_pos = 0;

    //  Sequence manipulation
    bool      insert_worker (size_type index, const value_type* buf, size_type len, action act);
    bool      erase_worker  (size_type index, size_type len, action act);
    bool      can_optimize  (action act, size_type index);
    void      record_action (action act, size_type index);

    size_type     lastaction_index = -1;
    action        lastaction       = action::invalid;
};

} // neatpad

#endif // NEATPAD_PIECE_CHAIN_HPP