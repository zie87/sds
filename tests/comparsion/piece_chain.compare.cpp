#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>

#include <neatpad/piece_chain.hpp>
#include <sds/piece_chain_method.hpp>
#include "../sds/chain.hpp"

template<class PC>
void insert_orderer_char_test( PC& chain, unsigned long cnt )
{
  for( unsigned long i = 0; i < cnt; ++i )
  {
    chain.insert(i, 'I');
  }
}

template<class PC>
void insert_multiple_char_test( PC& chain, unsigned long cnt )
{
  for( unsigned long i = 0; i < cnt; ++i )
  {
    chain.insert(i, cnt*10, 'M');
  }
}

template<class PC>
void insert_sequence_test( PC& chain, unsigned long cnt, const char* seq, unsigned long size )
{
  for( unsigned long i = 0; i < cnt; ++i )
  {
    chain.insert(i, seq, size);
  }
}

// hasn't this functions
template<>
void insert_multiple_char_test<neatpad::piece_chain>( neatpad::piece_chain& chain, unsigned long cnt )
{
  unsigned long size_cnt = cnt*10;
  for( unsigned long i = 0; i < cnt; ++i )
  {
    char* buf = new char[size_cnt];
    std::fill_n(buf, size_cnt, 'M');
    chain.insert(i, buf, size_cnt);
    delete[] buf;
  }
}

template<class PC>
void access_test( PC& chain )
{
  for( unsigned long i = 0; i < chain.size(); ++i )
  {
    chain.at(i);
  }  
}

template<class PC>
void append_test( PC& chain, unsigned long cnt )
{
  for( unsigned long i = 0; i < cnt; ++i )
  {
    chain.append("APPENDTEST", 10);
  }
}

int main()
{

  sequences::piece_chain_method   sds_pc;
  neatpad::piece_chain            nep_pc; nep_pc.init();
  test::piece_chain_method        tes_pc;

  using clock_t      = std::chrono::steady_clock;
  using duration_t   = std::chrono::nanoseconds;
  using time_point_t = std::chrono::time_point<clock_t>;

  clock_t      clock;
  time_point_t start;
  time_point_t stop;

  const unsigned int counter = 1024;

  // append
  //////////////////////////////////////////////////////////////////////////////
  start = clock.now();
  append_test(sds_pc, counter*10);
  stop = clock.now();
  auto time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "sds chain append time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  start = clock.now();
  append_test(nep_pc, counter*10);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "nep chain append time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  start = clock.now();
  append_test(tes_pc, counter*10);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "tes chain append time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  std::cout << std::endl << std::endl;
  // access
  //////////////////////////////////////////////////////////////////////////////
  start = clock.now();
  access_test(sds_pc);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "sds chain access time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  start = clock.now();
  access_test(nep_pc);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "nep chain access time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  start = clock.now();
  access_test(tes_pc);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "tes chain access time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  std::cout << std::endl << std::endl;
  // insert
  //////////////////////////////////////////////////////////////////////////////
  start = clock.now();
  insert_orderer_char_test(sds_pc, counter*10);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "sds chain insert ordered char time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  start = clock.now();
  insert_orderer_char_test(nep_pc, counter*10);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "nep chain insert ordered char time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  start = clock.now();
  insert_orderer_char_test(tes_pc, counter*10);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "tes chain insert ordered char time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  std::cout << std::endl << std::endl;
  // access
  //////////////////////////////////////////////////////////////////////////////
  start = clock.now();
  access_test(sds_pc);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "sds chain access time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  start = clock.now();
  access_test(nep_pc);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "nep chain access time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  start = clock.now();
  access_test(tes_pc);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "tes chain access time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  std::cout << std::endl << std::endl;
  // insert
  //////////////////////////////////////////////////////////////////////////////
  start = clock.now();
  insert_multiple_char_test(sds_pc, counter);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "sds chain insert multiple char time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  start = clock.now();
  insert_multiple_char_test(nep_pc, counter);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "nep chain insert multiple char time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  start = clock.now();
  insert_multiple_char_test(tes_pc, counter);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "tes chain insert multiple char time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  std::cout << std::endl << std::endl;
  // access
  //////////////////////////////////////////////////////////////////////////////
  start = clock.now();
  access_test(sds_pc);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "sds chain access time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  start = clock.now();
  access_test(nep_pc);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "nep chain access time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;

  start = clock.now();
  access_test(tes_pc);
  stop = clock.now();
  time_cnt = std::chrono::duration_cast<duration_t>(stop - start).count();
  std::cout << "tes chain access time " << std::right << std::setw(20)  << time_cnt << " ns" << std::endl;
}