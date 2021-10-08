
//---< C++ >---//
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

//---< boost >---//
#include <boost/program_options.hpp>

//---< rogue >---//
#include "rogue/interfaces/stream/TcpServer.h" 
#include "rogue/interfaces/stream/TcpClient.h" 

//---< emulators >---//
#include "emulators/Generator.h"
#include "emulators/HCalGenerator.h"
#include "emulators/HCalReceiver.h"
#include "emulators/TrigScintGenerator.h"
#include "emulators/TrigScintReceiver.h"

namespace po = boost::program_options;

int main(int argc, char **argv) {
 
  // The server address
  std::string addr{"*"};

  // The port number
  int port{8000}; 

  // The rate (Hz) at which to run
  float rate{5.}; 

  po::variables_map var_map;
  // NOTE: The same options needs to be declared here and in positionals
  po::options_description options("Available options");
  options.add_options()("help, h", "Print usage")
		       ("addr, a", po::value<std::string>(&addr),
			"The interface address for the server.")
		       ("port, p", po::value<int>(&port),
			"The port number.") 
		       ("server, s", po::bool_switch()->default_value(false),
			"Start a server.") 
		       ("client, c", po::bool_switch()->default_value(false), 
			"Start a client.") 
		       ("hcal", po::bool_switch()->default_value(false), 
			"Create an HCal data emulator.") 
		       ("trig", po::bool_switch()->default_value(false), 
			"Create an Trig Scint data emulator.") 
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

  // Flag indicating that this is running server side
  bool server{var_map["server"].as<bool>()}; 
  
  // Flag indicating that this is running client side
  bool client{var_map["client"].as<bool>()}; 
  
  // Flag indicating if an HCal generator/receiver should be used
  bool hcal{var_map["hcal"].as<bool>()}; 

  // Flag indicating if an Trigger Scintillator generator/receiver should be used
  bool trig{var_map["trig"].as<bool>()}; 

  // Print out all available commands if help is passed or if neither 
  // a server or client is specified.
  if (var_map.count("help") || (!server && !client)) {
    std::cout << options << std::endl;
    return EXIT_SUCCESS;
  } else if (server && client) {
    std::cerr << "-----< emulator > Both a server and client can't be " 
	     << "initialized together." << std::endl;
    return EXIT_FAILURE;
  }

  //  If starting a server, setup TCP bridge server and listen on all 
  //  interfaces, on the specified ports. The specified generator 
  //  is also instantiated and connected to the TCP bridge.
  //  TODO: At some point, generators should be loaded dynamically
  if (server) {

    std::shared_ptr<emulators::Generator> generator(nullptr);
    
    // Start the TCP bridge server
    auto tcp{rogue::interfaces::stream::TcpServer::create(addr, port)};

    // Create the specified generator
    if (hcal) generator = emulators::HCalGenerator::create();
    else if (trig) generator = emulators::TrigScintGenerator::create(); 

    // Connect the generator to the TCP bridge.
    generator->addSlave(tcp);

    while(true) {
      std::cout << "-----< emulator > Sending Frame" << std::endl;
      generator->genFrame(1000);
      std::cout << "-----< emulator > Summary -----\n"
		<< "\tTx count: " << generator->getTxCount() << "\n"
		<< "\tTx bytes: " << generator->getTxBytes() << "\n "
		<< "\tTx errors: " << generator->getTxErrors()
		<< std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(int((1/rate)*1000)));
      
    }
  } else if (client) {
    
    // Pointer to the receiver being used.  By default, it's null.
    // TODO: Should this be a vector instead? 
    std::shared_ptr<emulators::Receiver> receiver(nullptr); 

    // Start a TCP bridge client and connect to the remove server at 
    // ports 8000 & 8001. For now, this just connects to localhost.
    auto tcp{rogue::interfaces::stream::TcpClient::create(addr, port)};
  
    // Create the specified receiver
    if (hcal) receiver = emulators::HCalReceiver::create();
    else if (trig) receiver = emulators::TrigScintReceiver::create();

    // Connect the TCP bridge to the receiver
    tcp->addSlave(receiver);

    while(true) {
      std::cout << "Rx count: " << receiver->getRxCount() << " "
         	<< "Rx bytes: " << receiver->getRxBytes() << " "
         	<< "Rx errors: " << receiver->getRxErrors()
                << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(int((1/rate)*1000)));
    } 
  }
  return EXIT_SUCCESS; 
}
