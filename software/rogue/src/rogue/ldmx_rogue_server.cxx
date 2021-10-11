//---< C++ >---//
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

//---< boost >---//
#include <boost/program_options.hpp>

//---< emulators >---//
#include "emulators/Generator.h"
#include "emulators/HCalGenerator.h"
#include "emulators/TrigScintGenerator.h"

//---< ldmx-rogue >---//
#include "rogue/TcpCommandReceiver.h" 

//---< rogue >---//
#include "rogue/interfaces/stream/TcpServer.h" 

namespace po = boost::program_options;

int main(int argc, char **argv) {

  // The port number
  int port{8000}; 

  // The rate (Hz) at which to run
  float rate{5.}; 

  po::variables_map var_map;
  // NOTE: The same options needs to be declared here and in positionals
  po::options_description options("Available options");
  options.add_options()("help, h", "Print usage")
		       ("port, p", po::value<int>(&port),
			"The port number.") 
		       ("hcal", po::bool_switch()->default_value(false), 
			"Create an HCal data emulator.") 
		       ("trig", po::bool_switch()->default_value(false), 
			"Create an Trig Scint data emulator.") 
		       ("emulate", po::bool_switch()->default_value(false), 
			"Run in emulator mode.") 
  		       ("rate, r", po::value<float>(&rate),
			"Rate to run at in Hz."); 

  try {
    // Parser the command line options
    po::store(po::parse_command_line(argc, argv, options), var_map); 
    po::notify(var_map);
  } catch(po::unknown_option unknown) {
    std::cout << "-----< emulator >  Option unknown: " 
	      << unknown.get_option_name() << std::endl;
    std::cerr << options << std::endl;
    return EXIT_FAILURE; 
  }

  // Flag indicating if an HCal generator/receiver should be used
  bool hcal{var_map["hcal"].as<bool>()}; 

  // Flag indicating if an Trigger Scintillator generator/receiver should be used
  bool trig{var_map["trig"].as<bool>()}; 

  // Flag indicating whether the server should emulate data
  bool emulate{var_map["emulate"].as<bool>()};

  // Print out all available commands if help is passed or if neither 
  // a server or client is specified.
  if (var_map.count("help")) { 
    std::cout << options << std::endl;
    return EXIT_SUCCESS;
  } 

  // Start a TCP bridge server and listen on all interfaces,
  //  ports 8000 & 8001
  auto tcp{rogue::interfaces::stream::TcpServer::create("*", port)};

  // Create the TCP command receiver
  auto tcp_command{rogue::TcpCommandReceiver::create()}; 

  // Connect the TCP command receiver to the tcp rogue client
  tcp->addSlave(tcp_command); 

  std::shared_ptr<emulators::Generator> generator(nullptr);
  
  // Create the specified generator 
  if (hcal) generator = emulators::HCalGenerator::create();
  else if (trig) generator = emulators::TrigScintGenerator::create(); 

  if (emulate && generator == nullptr) { 
    // throw an exception
  } else { 
    generator->addSlave(tcp); 
  }

  // Send frames at a constant rate
  bool running{false}; 
  while(true) {
    if (!running && tcp_command->startRun()) {
	std::cout << "[ LDMX ] Starting run." << std::endl;
	running = true;
    } else if (running && tcp_command->stopRun()) { 
	std::cout << "[ LDMX ] Stopping run." << std::endl;
	running = false;
    }

    if (running) { 
	if (emulate) {
          generator->genFrame(1000);
	}
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(int((1/rate)*1000)));
  }
  return EXIT_SUCCESS; 
}
