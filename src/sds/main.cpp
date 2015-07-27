

/*

  - Sequence length of 8000 characters
  - Block size of 1024 characters
  - Fixed buffer methods keep buffers at least half full (from 512 to 1024 characters)
  - The location of 98% of the edits is normally distributed around the location of the previous
    edit with a standard deviation of 25
  - The location of 2% of the edits is uniformly distributed over the entire sequence
  - After each edit, 25 characters on each side of the edit location are accessed
  - Every 250 edits the entire file is scanned sequentially with ItemAts

*/

#include <sds/array_method.hpp>
#include <sds/gap_method.hpp>
#include <sds/list_method.hpp>
#include <sds/piece_chain_method.hpp>
// #include "line_span_method.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <chrono>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <boost/program_options.hpp>
namespace bo_opts = boost::program_options;

static std::random_device rdn_device;
static std::mt19937       rdn_generator(rdn_device());

unsigned int filtered_normally_distributed( unsigned int point, unsigned int deviation, unsigned int min = 0, unsigned int max = -1 )
{
  unsigned int number = min;

  do
  {
    std::normal_distribution<float> ndis(point,deviation);
    number =  std::round( ndis(rdn_generator) );
  } while( number < min || number > max  );


  return static_cast<unsigned int>(number);
}

struct configuration
{
  using size_type = std::size_t;

  size_type number_of_edits      = 60000;

  size_type sequenz_size         = 8000;
  size_type block_size           = 1024;

  float     uniformly_factor     = 0.2;
  size_type deviation            = 25;

  size_type border_access        = 25;
  size_type access_frequency     = 250;
};

void generate_random_positions( const configuration conf, std::vector<std::size_t>& vec )
{
  std::size_t last_pos = 0;

  std::size_t distribution_switch = conf.number_of_edits / conf.uniformly_factor;
  std::size_t size                = conf.sequenz_size;

  for(std::size_t i = 0; i < conf.number_of_edits; ++i)
  {
    if( i%distribution_switch == 0 )
    {
      std::uniform_int_distribution<> udis(0, ( size + i ));
      last_pos = udis(rdn_generator);
    } else
    {
      last_pos = filtered_normally_distributed( last_pos, conf.deviation, 0, size+1 );
    }
    vec.push_back(last_pos);
  }
}

struct insert_data
{
  std::size_t     size         = 0;
  std::intmax_t   insert_time  = 0;
  std::intmax_t   access_time  = 0;
};

struct access_data
{
  std::size_t     size         = 0;
  std::intmax_t   access_time  = 0;
};

using insert_vec = std::vector<insert_data>;
using access_vec = std::vector<access_data>;

void test_run( const configuration& conf, const std::vector<std::size_t>& dist_vec, sequences::sequence_interface* seq, insert_vec& i_vec, access_vec& a_vec )
{
  using clock_t      = std::chrono::steady_clock;
  using duration_t   = std::chrono::nanoseconds;
  using time_point_t = std::chrono::time_point<clock_t>;

  clock_t      clock;
  time_point_t start;
  time_point_t stop;

  for(unsigned int i = 0; i < dist_vec.size(); ++i)
  {
    insert_data data;
    start = clock.now();
    seq->insert( dist_vec[i], 'b' );
    stop  = clock.now();
    data.insert_time = std::chrono::duration_cast<duration_t>(stop - start).count();

    unsigned int access_size = seq->size() - 1;
    start = clock.now();
    for( unsigned int j = 0; j < conf.border_access; ++j ) { seq->at(j); seq->at( access_size - j ); }
    stop = clock.now();
    data.access_time = std::chrono::duration_cast<duration_t>(stop - start).count();
    data.size = seq->size();

    i_vec.push_back(data);

    if( i % conf.access_frequency == 0 )
    {
      const unsigned int size = seq->size();
      
      start = clock.now();
      for( unsigned int j = 0; j < size; ++j ) { seq->at(j); }
      stop = clock.now();
      access_data adat;
      adat.access_time = std::chrono::duration_cast<duration_t>(stop - start).count();
      adat.size = size;

      a_vec.push_back(adat);
    }
  }

  const unsigned int size = seq->size();
  start = clock.now();
  for( unsigned int j = 0; j < size; ++j ) { seq->at(j); }
  stop = clock.now();
  access_data adat;
  adat.access_time = std::chrono::duration_cast<duration_t>(stop - start).count();
  adat.size = size;

  a_vec.push_back(adat);

}

void print_data(const std::string& name, const insert_vec& i_vec, const access_vec& a_vec)
{
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss_i;
  // ss_i << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%X") << '-' << name << "_insert.txt";
  ss_i << in_time_t << "-" << name << "_insert.csv";

  std::ofstream log_file( ss_i.str().c_str() );
  for( auto&& it : i_vec )
  {    
    log_file << std::right << std::setw(15) << it.size
             << "; " << std::right << std::setw(15) << it.insert_time 
             << "; " << std::right << std::setw(15) << it.access_time 
             << std::endl; 
  }

  log_file.close();

  std::stringstream ss_a;
  // ss_a << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%X") << '-' << name << "_access.txt";
  ss_a << in_time_t << "-" << name << "_access.csv";

  log_file.open(ss_a.str().c_str() );
  for( auto&& it : a_vec )
  {    
    log_file <<  std::right << std::setw(15) << it.size 
             << "; " << std::right << std::setw(15) << it.access_time 
             << std::endl; 
  }
  log_file.close();
}

int main(int argc, char* argv[])
{ 
  try
  {  
    bo_opts::options_description desc("Allowed options");
    desc.add_options()
      ("help,h", "produce help message")
      ("file,f", bo_opts::value<std::string>(), "load distribution data")
      ("skip-array"        , "skip tests for array_sequence")
      ("skip-list"         , "skip tests for list_sequence")
      ("skip-gap"          , "skip tests for gap_sequence")
      ("skip-piece-chain"  , "skip tests for piece_chain_sequence")
    ;

    bo_opts::variables_map vm;        
    bo_opts::store(bo_opts::parse_command_line(argc, argv, desc), vm);
    bo_opts::notify(vm);    

    if (vm.count("help")) 
    {
      std::cout << desc << "\n";
      return 0;
    }

    bool        create = true;

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    // ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%X") << "-distribution_positions.dat";
    ss << in_time_t << "-" << "distribution_positions.dat";
    std::string path(ss.str());

    if( vm.count("file") )
    { 
      create = false;
      path = vm["file"].as<std::string>();
    }

    bool skip_array       = false; if(vm.count("skip-array"))       skip_array = true;
    bool skip_list        = false; if(vm.count("skip-list"))        skip_list = true;
    bool skip_gap         = false; if(vm.count("skip-gap"))         skip_gap = true;
    bool skip_piece_chain = false; if(vm.count("skip-piece-chain"))   skip_piece_chain = true;

    configuration configs;
    std::vector<std::size_t> distribution_vec;
    distribution_vec.clear();

    if(create)
    {
      std::cout << "init distribution positions!" << std::endl;
      generate_random_positions(configs, distribution_vec);

      std::cout << "saving distribution positions!" << std::endl;

      std::ofstream os(path.c_str(), std::ios::binary);
      boost::archive::binary_oarchive oar(os);
      oar << distribution_vec;
    } else
    {
      std::cout << "loading distribution positions!" << std::endl;
      std::ifstream is(path.c_str(), std::ios::binary);
      boost::archive::binary_iarchive iar(is);
      iar >> distribution_vec;
    }


    insert_vec i_vec;
    access_vec a_vec;

    if(!skip_array)
    {
      std::cout << "testing array sequence!" << std::endl;
      sequences::array_method*  array_seq = new sequences::array_method(configs.sequenz_size, 'a');
      i_vec.clear(); a_vec.clear();
      test_run( configs, distribution_vec, array_seq, i_vec, a_vec );
      print_data("array_seq", i_vec, a_vec);
      delete array_seq;
    }

    if(!skip_gap)

    { 
      std::cout << "testing gap sequence!" << std::endl;
      sequences::gap_method*    gap_seq   = new sequences::gap_method(  configs.sequenz_size, 'a');
      i_vec.clear(); a_vec.clear();
      test_run( configs, distribution_vec, gap_seq, i_vec, a_vec );
      print_data("gap_seq", i_vec, a_vec);
      delete gap_seq;
    }

    if(!skip_list)
    {     
      std::cout << "testing list sequence!" << std::endl;
      sequences::list_method*   list_seq  = new sequences::list_method( configs.sequenz_size, 'a');
      i_vec.clear(); a_vec.clear();
      test_run( configs, distribution_vec, list_seq, i_vec, a_vec );
      print_data("list_seq", i_vec, a_vec);
      delete list_seq;
    }

    if(!skip_piece_chain)
    {     
      std::cout << "testing piece_chain sequence!" << std::endl;
      sequences::piece_chain_method*   piece_chain_seq  = new sequences::piece_chain_method( configs.sequenz_size, 'a');
      i_vec.clear(); a_vec.clear();
      test_run( configs, distribution_vec, piece_chain_seq, i_vec, a_vec );
      print_data("piece_chain_seq", i_vec, a_vec);
      // piece_chain_seq->debug();
      delete piece_chain_seq;
    }

  } catch(std::exception& e) 
  {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  } catch(...) 
  {
    std::cerr << "Exception of unknown type!\n";
    return 1;
  }

  return 0;
}