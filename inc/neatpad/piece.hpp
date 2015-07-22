#ifndef NEATPAD_PIECE_HPP
#define NEATPAD_PIECE_HPP

namespace neatpad
{

    //  piece_chain::action
  //
  //  enumeration of the type of 'edit actions' our piece_chain supports.
  //  only important when we try to 'optimize' repeated operations on the
  //  piece_chain by coallescing them into a single piece.
  //
  enum class action
  { 
    invalid = 0, 
    insert, 
    erase, 
    replace 
  };

  struct piece
  {
    using value_type = char;
    using size_type  = unsigned long int;

    piece(size_type off, size_type len, int buf, piece *nx = 0, piece *pr = 0) 
    : next(nx), prev(pr),offset(off), length(len), buffer(buf)
    {
      static int count=-2;
      id = count++;
    }

    piece(const piece&)            = delete;
    piece& operator=(const piece&) = delete;

    piece   *next;
    piece   *prev;
    
    size_type  offset;
    size_type  length;

    int         buffer;
    int         id = 0;
  };  

  //  piece_chain::piece_range
  //
  //  private class to the piece_chain. Used to represent a contiguous range of pieces.
  //  used by the undo/redo stacks to store state. A piece-range effectively represents
  //  the range of pieces affected by an event (operation) on the piece_chain
  class piece_range
  {
    friend class piece_chain;
    friend class undo_redo_stack;

    public:
      using value_type = char;
      using size_type  = unsigned long int;

      piece_range( size_type seqlen = 0, size_type idx = 0, size_type len = 0,  action  a = action::invalid, bool qs = false, size_type id = 0) noexcept
      : boundary(true), piece_chain_length(seqlen), index(idx), length(len), act(a), quicksave(qs), group_id(id)
      {}
        
      // destructor does nothing - because sometimes we don't want
      // to free the contents when the piece_range is deleted. e.g. when
      // the piece_range is just a temporary helper object. The contents
      // must be deleted manually with piece_range::free
      ~piece_range() noexcept {}

      piece_range(const piece_range&)            = delete;
      piece_range& operator=(const piece_range&) = delete; 

      // separate 'destruction' used when appropriate
      void free()
      {
        piece *sptr, *next, *term;
        
        if(boundary == false)
        {
          // delete the range of pieces
          for(sptr = first, term = last->next; sptr && sptr != term; sptr = next)
          {
            next = sptr->next;
            delete sptr;
          }
        }
      }

      // add a piece into the range
      void append(piece *sptr)
      {
        if(sptr != 0)
        {
          // first time a piece has been added?
          if(first == 0)
          {
            first = sptr;
          }
          // otherwise chain the pieces together.
          else
          {
            last->next = sptr;
            sptr->prev = last;
          }
          
          last     = sptr;
          boundary = false;
        }
      }

      // join two piece-ranges together
      void append(piece_range* range)
      {
        if(range->boundary == false)
        { 
          if(boundary)
          {
            first       = range->first;
            last        = range->last;
            boundary    = false;
          }
          else
          {
            range->first->prev = last;
            last->next  = range->first;
            last    = range->last;
          }
        }
      }

      // join two piece-ranges together. used only for 'back-delete'
      void prepend(piece_range* range)
      {
        if(range->boundary == false)
        {
          if(boundary)
          {
            first       = range->first;
            last        = range->last;
            boundary    = false;
          }
          else
          {
            range->last->next = first;
            first->prev = range->last;
            first   = range->first;
          }
        }
      }
      
      // An 'empty' range is represented by storing pointers to the
      // pieces ***either side*** of the piece-boundary position. Input is
      // always the piece following the boundary.
      void pieceboundary(piece *before, piece *after)
      {
        first    = before;
        last     = after;
        boundary = true;
      }

      
    private:
      
      // the piece range
      piece  *first = nullptr;
      piece  *last  = nullptr;
      bool   boundary;

      // piece_chain state
      size_type  piece_chain_length;
      size_type  index;
      size_type  length;
      action   act;
      bool   quicksave;
      size_type  group_id;
  };


} // neatpad

#endif // NEATPAD_PIECE_HPP