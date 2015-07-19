#include "piece_chain.hpp"

int main()
{
  sequences::piece_chain seq;
  seq.init();
  seq.append( "A large span of text", 20 );
  seq.erase(2, 6);
  seq.insert(10, "English ", 8);

  seq.debug1();
  seq.debug2();

  return 0;
}