#include <libunittest/all.hpp>

namespace ut  = unittest;
namespace uta = ut::assertions;

// #include <sds/piece_chain_method.hpp>
#include "chain.hpp"

struct redesign_tests : ut::testcase<> 
{

  static void run()
  {
    UNITTEST_CLASS(redesign_tests);
    UNITTEST_RUN(simple_test);
    UNITTEST_RUN(fill_test);
    UNITTEST_RUN(iterator_test);
    UNITTEST_RUN(insert_test);
  }

  redesign_tests() {} // executed before each test and before set_up()
  ~redesign_tests() {} // executed after each test and after tear_down()

  void set_up() {} // executed before each test and after constructor
  void tear_down() {} // executed after each test and before destructor

  void simple_test()
  {
    test::piece_chain_method pc;
    uta::assert_equal( 0, pc.size() );
    uta::assert_true( pc.empty() );
    uta::assert_true( pc.begin() == pc.end() );
    // uta::assert_true( pc.rbegin() == pc.rend() );


    pc.append("A large span of text", 20);
    uta::assert_equal( 20, pc.size() );
    uta::assert_false( pc.empty() );
    uta::assert_equal("A large span of text", pc.str());

    pc.erase(2,6);
    uta::assert_equal( 14, pc.size() );
    uta::assert_equal("A span of text", pc.str());

    // pc.debug();

    pc.insert(10, "English ", 8);
    uta::assert_equal( 22, pc.size() );
    uta::assert_equal("A span of English text", pc.str());


    pc.erase(6,8);
    uta::assert_equal( 14, pc.size() );
    uta::assert_equal("A spanish text", pc.str());
  
    pc.insert(0, "This is not ", 12);
    uta::assert_equal( 26, pc.size() );
    uta::assert_equal("This is not A spanish text", pc.str());

    std::string test_str("This is not A spanish text");
    for(unsigned int i = 0; i < test_str.size(); ++i )
    {
      uta::assert_equal( test_str.at(i), pc.at(i) );
    }
  }

  void fill_test()
  {
    test::piece_chain_method pc;
    pc.append("A large span of text", 20);
    uta::assert_equal( 20, pc.size() );

    // test fill
    pc.append(10, 'Z');
    uta::assert_equal( 30, pc.size() );

    pc.insert(5, 5, 'Q');
    uta::assert_equal( 35, pc.size() );

    // pc.debug();

    test::piece_chain_method pc2(20, 'A');
    uta::assert_equal( 20, pc2.size() );
    for(unsigned int i = 0; i < 20; ++i )
    {
      uta::assert_equal( 'A', pc2.at(i) );
    }
  }

  void iterator_test()
  {
    test::piece_chain_method pc(20, 'A');

    unsigned int cnt = 0;
    for( auto it = pc.begin(); it != pc.end(); ++it )
    {
      uta::assert_equal(*it, 'A');
      ++cnt;
    }
    uta::assert_equal( 20, cnt );

    cnt = 0;
    for( test::piece_chain_method::const_iterator it = pc.begin(); it != pc.end(); ++it)
    {
      uta::assert_equal(*it, 'A');
      ++cnt;
    }
    uta::assert_equal( 20, cnt );
  }

  void insert_test()
  {
    test::piece_chain_method pc(1024, 'A');
    pc.append(10, 'C');
    pc.append("BBBBBBBBBB", 10);
    uta::assert_equal(1044, pc.size());

    for(unsigned int i = 0; i < 56; ++i)
    {
      pc.insert(200+i, 'D');
    }
    uta::assert_equal(1100, pc.size()); // without optimization 60 nodes, with 6
    // pc.debug();
  };
};

REGISTER(redesign_tests) // this registers the test class