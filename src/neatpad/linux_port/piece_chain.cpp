

// #include <string.h>

#include <iostream>  // for debug
#include <algorithm>

#include "piece_chain.hpp"

namespace neatpad
{

const piece_chain::size_type piece_chain::MAX_SEQUENCE_LENGTH = ((piece_chain::size_type)(-1) / sizeof(piece_chain::value_type));


piece_chain::piece_chain() noexcept 
: undostack(), redostack(), buffer_list()
{
  record_action(action::invalid, 0);
  
  head = tail   = 0;
  piece_chain_length = 0;
  // group_id    = 0;
  // group_refcount  = 0;

  head      = new piece(0, 0, 0);
  tail      = new piece(0, 0, 0);
  head->next    = tail;
  tail->prev    = head;
}

piece_chain::~piece_chain() noexcept
{
  clear();

  delete head;
  delete tail;
}

bool piece_chain::init ()
{
  piece_chain_length = 0;

  if(!alloc_modify_buffer(0x10000)) return false;

  record_action(action::invalid, 0);
  return true;
}

bool piece_chain::init (const value_type *buffer, size_type length)
{
  clear();
  if(!init()) return false;

  buffer_control *bc = alloc_modify_buffer(length);
  std::copy( buffer, buffer + length, bc->buffer );

  bc->length = length;

  piece *sptr = new piece(0, length, bc->id, tail, head);
  head->next = sptr;
  tail->prev = sptr;

  piece_chain_length = length;
  return true;
}

template <class type>
void piece_chain::clear_vector (type &vectorobject)
{
  for(size_type i = 0; i < vectorobject.size(); i++)
  {
    delete vectorobject[i];
  }
}

void piece_chain::debug1 ()
{
  piece *sptr;

  for(sptr = head; sptr; sptr = sptr->next)
  {
    auto buffer = buffer_list[sptr->buffer]->buffer;
    std::cout << string_type( buffer + sptr->offset, buffer + sptr->offset + sptr->length );
  }

  std::cout << std::endl;
}

void piece_chain::debug2 ()
{
  piece *sptr;

  std::cout << "**********************" << std::endl;
  for(sptr = head; sptr; sptr = sptr->next)
  {
    auto buffer = buffer_list[sptr->buffer]->buffer;
    std::cout << "[" << sptr->id << "]"
              << "[" << sptr->offset << "\t" << sptr->length << "]"
              << string_type( buffer + sptr->offset, buffer + sptr->offset + sptr->length )
              << std::endl;
  }

  std::cout << "-------------------------" << std::endl;

  for(sptr = tail; sptr; sptr = sptr->prev)
  {
    auto buffer = buffer_list[sptr->buffer]->buffer;
    std::cout << "[" << sptr->id << "]"
              << "[" << sptr->offset << "\t" << sptr->length << "]"
              << string_type( buffer + sptr->offset, buffer + sptr->offset + sptr->length )
              << std::endl;
  }

  std::cout << "**********************" << std::endl;

  for(sptr = head; sptr; sptr = sptr->next)
  {
    auto buffer = buffer_list[sptr->buffer]->buffer;
    std::cout << string_type( buffer + sptr->offset, buffer + sptr->offset + sptr->length );
  }

  std::cout << std::endl << "piece_chain length = " << piece_chain_length << " chars" << std::endl;
  std::cout << std::endl;
}

//
//  Allocate a buffer and add it to our 'buffer control' list
//
buffer_control* piece_chain::alloc_buffer (size_type maxsize)
{

  auto bc = buffer_control::alloc_buffer(maxsize);
  bc->id    = buffer_list.size();   // assign the id

  buffer_list.push_back(bc);

  return bc;
}

buffer_control* piece_chain::alloc_modify_buffer (size_type maxsize)
{
  buffer_control *bc;
  
  if((bc = alloc_buffer(maxsize)) == 0)
    return 0;

  modify_buffer_id  = bc->id;
  modify_buffer_pos = 0;

  return bc;
}

//
//  Import the specified range of data into the piece_chain so we have our own private copy
//
bool piece_chain::import_buffer (const value_type *buf, size_type len, size_type *buffer_offset)
{
  buffer_control *bc;
  
  // get the current modify-buffer
  bc = buffer_list[modify_buffer_id];

  // if there isn't room then allocate a new modify-buffer
  if(bc->length + len >= bc->capacity)
  {
    bc = alloc_modify_buffer(len + 0x10000);
    
    // make sure that no old pieces use this buffer
    record_action(action::invalid, 0);
  }

  if(bc == 0)
    return false;

  // import the data
  std::copy( buf, buf + len, bc->buffer + bc->length );
  
  *buffer_offset = bc->length;
  bc->length += len;

  return true;
}

//  piece_chain::piecefromindex
//
//  search the piecelist for the piece which encompasses the specified index position
//
//  index   - character-position index
//  *pieceindex  - index of piece within piece_chain
piece* piece_chain::piecefromindex (size_type index, size_type *pieceindex = 0) const
{
  piece * sptr;
  size_type curidx = 0;
  
  // scan the list looking for the piece which holds the specified index
  for(sptr = head->next; sptr->next; sptr = sptr->next)
  {
    if(index >= curidx && index < curidx + sptr->length)
    {
      if(pieceindex) 
        *pieceindex = curidx;

      return sptr;
    }

    curidx += sptr->length;
  }

  // insert at tail
  if(sptr && index == curidx)
  {
    *pieceindex = curidx;
    return sptr;
  }

  return 0;
}


//  UNDO the last action
bool piece_chain::undo () 
{     
  // make sure that no "optimized" actions can occur
  record_action(action::invalid, 0); 
  return undoredo(undostack, redostack); 
}

//  REDO the last UNDO
bool piece_chain::redo ()
{ 
  // make sure that no "optimized" actions can occur
  record_action(action::invalid, 0);
  return undoredo(redostack, undostack); 
}

//  Will calling piece_chain::undo change the piece_chain?
bool piece_chain::canundo () const noexcept { return undostack.size() != 0; }
//  Will calling piece_chain::redo change the piece_chain?
bool piece_chain::canredo () const noexcept { return redostack.size() != 0; }

//  Return logical length of the piece_chain
piece_chain::size_type piece_chain::size () const noexcept { return piece_chain_length; }


void piece_chain::record_action (action act, size_type index)
{
  lastaction_index = index;
  lastaction       = act;
}

bool piece_chain::can_optimize (action act, size_type index)
{
  return (lastaction == act && lastaction_index == index);
}

//
//  piece_chain::insert_worker
//
bool piece_chain::insert_worker (size_type index, const value_type *buf, size_type length, action act)
{
  piece *    sptr;
  size_type   pieceindex;
  size_type    modbuf_offset;
  piece_range  newpieces;
  size_type   insoffset;

  if(index > piece_chain_length)
    return false;

  // find the piece that the insertion starts at
  if((sptr = piecefromindex(index, &pieceindex)) == 0)
    return false;

  // ensure there is room in the modify buffer...
  // allocate a new buffer if necessary and then invalidate piece cache
  // to prevent a piece using two buffers of data
  if(!import_buffer(buf, length, &modbuf_offset))
    return false;

  clearstack(redostack);
  insoffset = index - pieceindex;

  // special-case #1: inserting at the end of a prior insertion, at a piece-boundary
  if(insoffset == 0 && can_optimize(act, index))
  {
    // simply extend the last piece's length
    piece_range *event = undostack.back();
    sptr->prev->length  += length;
    event->length   += length;
  }
  // general-case #1: inserting at a piece boundary?
  else if(insoffset == 0)
  {
    //
    // Create a new undo event; because we are inserting at a piece
    // boundary there are no pieces to replace, so use a "piece boundary"
    //
    piece_range *oldpieces = initundo(index, length, act);
    oldpieces->pieceboundary(sptr->prev, sptr);
    
    // allocate new piece in the modify buffer
    newpieces.append(new piece(
      modbuf_offset, 
      length, 
      modify_buffer_id)
      );
    
    // link the piece into the piece_chain
    swap_piece_range(oldpieces, &newpieces);
  }
  // general-case #2: inserting in the middle of a piece
  else
  {
    //
    //  Create a new undo event and add the piece
    //  that we will be "splitting" in half
    //
    piece_range *oldpieces = initundo(index, length, act);
    oldpieces->append(sptr);

    //  piece for the existing data before the insertion
    newpieces.append(new piece(
              sptr->offset, 
              insoffset, 
              sptr->buffer)
            );

    // make a piece for the inserted data
    newpieces.append(new piece(
              modbuf_offset, 
              length, 
              modify_buffer_id)
            );

    // piece for the existing data after the insertion
    newpieces.append(new piece(
              sptr->offset + insoffset, 
              sptr->length - insoffset, 
              sptr->buffer)
            );

    swap_piece_range(oldpieces, &newpieces);
  }

  piece_chain_length += length;

  return true;
}

//  piece_chain::insert
//
//  Insert a buffer into the piece_chain at the specified position.
//  Consecutive insertions are optimized into a single event
bool piece_chain::insert (size_type index, const value_type *buf, size_type length)
{
  if(insert_worker(index, buf, length, action::insert))
  {
    record_action(action::insert, index + length);
    return true;
  }
  else
  {
    return false;
  }
}

//  piece_chain::insert
//
//  Insert specified character-value into piece_chain
bool piece_chain::insert (size_type index, const value_type val)
{
  return insert(index, &val, 1);
}

//  piece_chain::deletefrompiece_chain
//
//  Remove + delete the specified *piece* from the piece_chain
void piece_chain::deletefrompiece_chain(piece **psptr)
{
  piece *sptr = *psptr;
  sptr->prev->next = sptr->next;
  sptr->next->prev = sptr->prev;
  delete sptr;
  *psptr = nullptr;
}

//  piece_chain::erase_worker
bool piece_chain::erase_worker (size_type index, size_type length, action act)
{
  piece    *sptr;
  piece_range   oldpieces;
  piece_range   newpieces;
  piece_range  *event;
  size_type    pieceindex;
  size_type    remoffset;
  size_type    removelen;
  bool     append_piece_range;  

  // make sure we stay within the range of the piece_chain
  if(length == 0 || length > piece_chain_length || index > piece_chain_length - length)
    return false;

  // find the piece that the deletion starts at
  if((sptr = piecefromindex(index, &pieceindex)) == 0)
    return false;

  // work out the offset relative to the start of the *piece*
  remoffset = index - pieceindex;
  removelen = length;

  //  can we optimize?
  //
  //  special-case 1: 'forward-delete'
  //  erase+replace operations will pass through here
  if(index == pieceindex && can_optimize(act, index))
  {
    event = stackback(undostack, act == action::replace ? 1 : 0);
    event->length += length;
    append_piece_range = true;

    if(frag2 != 0)
    {
      if(length < frag2->length)
      {
        frag2->length -= length;
        frag2->offset += length;
        piece_chain_length -= length;
        return true;
      }
      else
      {
        if(act == action::replace)
          stackback(undostack, 0)->last = frag2->next;

        removelen -= sptr->length;
        sptr = sptr->next;
        deletefrompiece_chain(&frag2);
      }
    }
  }
  
  //  special-case 2: 'backward-delete'
  //  only erase operations can pass through here
  else if(index + length == pieceindex + sptr->length && can_optimize(action::erase, index+length))
  {
    event = undostack.back();
    event->length += length;
    event->index  -= length;
    append_piece_range = false;

    if(frag1 != 0)
    {
      if(length < frag1->length)
      {
        frag1->length -= length;
        frag1->offset += 0;
        piece_chain_length -= length;
        return true;
      }
      else
      {
        removelen -= frag1->length;
        deletefrompiece_chain(&frag1);
      }
    }
  }
  else
  {
    append_piece_range = true;
    frag1 = frag2 = 0;

    if((event = initundo(index, length, act)) == 0)
      return false;
  }

  //  general-case 2+3
  clearstack(redostack);

  // does the deletion *start* mid-way through a piece?
  if(remoffset != 0)
  {
    // split the piece - keep the first "half"
    newpieces.append(new piece(sptr->offset, remoffset, sptr->buffer));
    frag1 = newpieces.first;
    
    // have we split a single piece into two?
    // i.e. the deletion is completely within a single piece
    if(remoffset + removelen < sptr->length)
    {
      // make a second piece for the second half of the split
      newpieces.append(new piece(
              sptr->offset + remoffset + removelen, 
              sptr->length - remoffset - removelen, 
              sptr->buffer)
              );

      frag2 = newpieces.last;
    }

    removelen -= std::min(removelen, (sptr->length - remoffset));

    // archive the piece we are going to replace
    oldpieces.append(sptr);
    sptr = sptr->next;  
  }

  // we are now on a proper piece boundary, so remove
  // any further pieces that the erase-range encompasses
  while(removelen > 0 && sptr != tail)
  {
    // will the entire piece be removed?
    if(removelen < sptr->length)
    {
      // split the piece, keeping the last "half"
      newpieces.append(new piece(
            sptr->offset + removelen, 
            sptr->length - removelen, 
            sptr->buffer)
            );

      frag2 = newpieces.last;
    }

    removelen -= std::min(removelen, sptr->length);

    // archive the piece we are replacing
    oldpieces.append(sptr);
    sptr = sptr->next;
  }

  // for replace operations, update the undo-event for the
  // insertion so that it knows about the newly removed pieces
  if(act == action::replace && !oldpieces.boundary)
    stackback(undostack, 0)->last = oldpieces.last->next;

  swap_piece_range(&oldpieces, &newpieces);
  piece_chain_length -= length;

  if(append_piece_range)
    event->append(&oldpieces);
  else
    event->prepend(&oldpieces);

  return true;
}

//  piece_chain::erase 
//
//  "removes" the specified range of data from the piece_chain. 
bool piece_chain::erase (size_type index, size_type len)
{
  if(erase_worker(index, len, action::erase))
  {
    record_action(action::erase, index);
    return true;
  }
  else
  {
    return false;
  }
}

//  piece_chain::erase
//
//  remove single character from piece_chain
bool piece_chain::erase (size_type index)
{
  return erase(index, 1);
}

//  piece_chain::replace
//
//  A 'replace' (or 'overwrite') is a combination of erase+inserting
//  (first we erase a section of the piece_chain, then insert a new block
//  in it's place). 
//
//  Doing this as a distinct operation (erase+insert at the 
//  same time) is really complicated, so I just make use of the existing 
//  piece_chain::erase and piece_chain::insert and combine them into action. We
//  need to play with the undo stack to combine them in a 'true' sense.
bool piece_chain::replace(size_type index, const value_type *buf, size_type length, size_type erase_length)
{
  size_type remlen = 0;

  // make sure operation is within allowed range
  if(index > piece_chain_length || MAX_SEQUENCE_LENGTH - index < length)
    return false;

  // for a "replace" which will overrun the piece_chain, make sure we 
  // only delete up to the end of the piece_chain
  remlen = std::min(piece_chain_length - index, erase_length);

  // combine the erase+insert actions together
  group();

  // first of all remove the range
  if(remlen > 0 && index < piece_chain_length && !erase_worker(index, remlen, action::replace))
  {
    ungroup();
    return false;
  }
  
  // then insert the data
  if(insert_worker(index, buf, length, action::replace))
  {
    ungroup();
    record_action(action::replace, index + length);
    return true;
  }
  else
  {
    // failed...cleanup what we have done so far
    ungroup();
    record_action(action::invalid, 0);

    piece_range *range = undostack.back();
    undostack.pop_back();
    restore_piece_range(range, true);
    delete range;

    return false;
  }
}

//  piece_chain::replace
//
//  overwrite with the specified buffer
bool piece_chain::replace (size_type index, const value_type *buf, size_type length)
{
  return replace(index, buf, length, length);
}

//  piece_chain::replace
//
//  overwrite with a single character-value
bool piece_chain::replace (size_type index, const value_type val)
{
  return replace(index, &val, 1);
}

//  piece_chain::append
//
//  very simple wrapper around piece_chain::insert, just inserts at
//  the end of the piece_chain
bool piece_chain::append (const value_type *buf, size_type length)
{
  return insert(size(), buf, length);
}

//  piece_chain::append
//
//  append a single character to the piece_chain
bool piece_chain::append (const value_type val)
{
  return append(&val, 1);
}

//  piece_chain::clear
//
//  empty the entire piece_chain, clear undo/redo history etc
bool piece_chain::clear () noexcept
{
  piece *sptr, *tmp;
  
  // delete all pieces in the piece_chain
  for(sptr = head->next; sptr != tail; sptr = tmp)
  {
    tmp = sptr->next;
    delete sptr;
  }

  // re-link the head+tail
  head->next = tail;
  tail->prev = head;

  // delete everything in the undo/redo stacks
  clearstack(undostack);
  clearstack(redostack);

  // delete all memory-buffers
  for(size_type i = 0; i < buffer_list.size(); i++)
  {
    delete[] buffer_list[i]->buffer;
    delete   buffer_list[i];
  }

  buffer_list.clear();
  piece_chain_length = 0;
  return true;
}

//  piece_chain::render
//
//  render the specified range of data (index, len) and store in 'dest'
//  Returns number of chars copied into destination
piece_chain::size_type piece_chain::render(size_type index, value_type *dest, size_type length) const
{
  size_type pieceoffset = 0;
  size_type total = 0;
  piece  *sptr;

  // find piece to start rendering at
  if((sptr = piecefromindex(index, &pieceoffset)) == 0)
    return false;

  // might need to start mid-way through the first piece
  pieceoffset = index - pieceoffset;

  // copy each piece's referenced data in succession
  while(length && sptr != tail)
  {
    size_type copylen   = std::min(sptr->length - pieceoffset, length);
    value_type *source  = buffer_list[sptr->buffer]->buffer;

    // memcpy(dest, source + sptr->offset + pieceoffset, copylen * sizeof(value_type));

    value_type* input = source + sptr->offset + pieceoffset; 
    std::copy( input , input + copylen ,dest);
    
    dest  += copylen;
    length  -= copylen;
    total += copylen;

    sptr = sptr->next;
    pieceoffset = 0;
  }

  return total;
}

//  piece_chain::peek
//
//  return single element at specified position in the piece_chain
piece_chain::value_type piece_chain::peek(size_type index) const
{
  value_type   value;
  return render(index, &value, 1) ? value : 0;
}

//  piece_chain::poke
//
//  modify single element at specified position in the piece_chain
bool piece_chain::poke(size_type index, value_type value) 
{
  return replace(index, &value, 1);
}

//  piece_chain::operator[] const
//
//  readonly array access
piece_chain::value_type piece_chain::operator[] (size_type index) const
{
  return peek(index);
}

//  piece_chain::operator[] 
//
//  read/write array access
piece_chain::ref piece_chain::operator[] (size_type index)
{
  return ref(this, index);
}

//  piece_chain::breakopt
//
//  Prevent subsequent operations from being optimized (coalesced) 
//  with the last.
void piece_chain::breakopt() { lastaction = action::invalid; }


//  piece_chain::undoredo
//
//  private routine used to undo/redo piece_range events to/from 
//  the piece_chain - handles 'grouped' events
bool piece_chain::undoredo (eventstack& source, eventstack& dest)
{
  piece_range *range = 0;
  size_type group_id;

  if(source.empty()) return false;

  group_id = source.back()->group_id;

  do
  {
    // remove the next event from the source stack
    range = source.back();
    source.pop_back();

    // add event onto the destination stack
    dest.push_back(range);

    // do the actual work
    restore_piece_range(range, source == undostack ? true : false);
  }
  while(!source.empty() && (source.back()->group_id == group_id && group_id != 0));

  return true;
}

void piece_chain::restore_piece_range (piece_range *range, bool undo_or_redo)
{
  if(range->boundary)
  {
    piece *first = range->first->next;
    piece *last  = range->last->prev;

    // unlink pieces from main list
    range->first->next = range->last;
    range->last->prev  = range->first;

    // store the piece range we just removed
    range->first = first;
    range->last  = last;
    range->boundary = false;
  }
  else
  {
    piece *first = range->first->prev;
    piece *last  = range->last->next;

    // are we moving pieces into an "empty" region?
    // (i.e. inbetween two adjacent pieces)
    if(first->next == last)
    {
      // move the old pieces back into the empty region
      first->next = range->first;
      last->prev  = range->last;

      // store the piece range we just removed
      range->first  = first;
      range->last   = last;
      range->boundary  = true;
    }
    // we are replacing a range of pieces in the list,
    // so swap the pieces in the list with the one's in our "undo" event
    else
    {
      // find the piece range that is currently in the list
      first = first->next;
      last  = last->prev;

      // unlink the the pieces from the main list
      first->prev->next = range->first;
      last->next->prev  = range->last;

      // store the piece range we just removed
      range->first = first;
      range->last  = last;
      range->boundary = false;
    }
  }

  // update the 'piece_chain length' and 'quicksave' states
  std::swap(range->piece_chain_length,    piece_chain_length);
  std::swap(range->quicksave,             can_quicksave);

  undoredo_index  = range->index;

  if( (range->act == action::erase && undo_or_redo == true) || (range->act != action::erase && undo_or_redo == false) ) { undoredo_length = range->length;}
  else { undoredo_length = 0; }
}


void piece_chain::swap_piece_range(piece_range *src, piece_range *dest)
{
  if(src->boundary)
  {
    if(!dest->boundary)
    {
      src->first->next = dest->first;
      src->last->prev  = dest->last;
      dest->first->prev = src->first;
      dest->last->next  = src->last;
    }
  }
  else
  {
    if(dest->boundary)
    {
      src->first->prev->next = src->last->next;
      src->last->next->prev  = src->first->prev;
    }
    else
    {
      src->first->prev->next = dest->first;
      src->last->next->prev  = dest->last;
      dest->first->prev = src->first->prev;
      dest->last->next = src->last->next;
    } 
  }
}


//  piece_chain::initundo
//  create a new (empty) piece range and save the current piece_chain state
piece_range* piece_chain::initundo (size_type index, size_type length, action act)
{
  piece_range* event = new piece_range (
                piece_chain_length, 
                index,
                length,
                act,
                can_quicksave, 
                group_refcount ? group_id : 0
                );

  undostack.push_back(event);
  
  return event;
}

piece_range* piece_chain::stackback(eventstack& source, size_type idx)
{
  size_type length = source.size();
  
  if(length > 0 && idx < length)
  {
    return source[length - idx - 1];
  }
  else
  {
    return 0;
  }
}

void piece_chain::clearstack (eventstack& dest) noexcept
{
  for(size_type i = 0; i < dest.size(); i++)
  {
    dest[i]->free();
    delete dest[i];
  }

  dest.clear();
}

//  Group repeated actions on the piece_chain (insert/erase etc)
//  into a single 'undoable' action
void piece_chain::group()
{
  if(group_refcount == 0)
  {
    if(++group_id == 0) ++group_id;
    group_refcount++;
  }
}

//
//  Close the grouping
//
void piece_chain::ungroup()
{
  if(group_refcount > 0)
    group_refcount--;
}

} // neatpad