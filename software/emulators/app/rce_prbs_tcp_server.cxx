//---< C++ >---//
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

//---< rogue >---//
#include "rogue/utilities/Prbs.h"
#include "rogue/interfaces/stream/TcpServer.h" 

using namespace std; 

int main(int argc, char **argv) {

  // Create a PRBS generator
  auto prbs{rogue::utilities::Prbs::create()};

  // Start a TCP bridge server and listen on all interfaces,
  //  ports 8000 & 8001
  auto tcp{rogue::interfaces::stream::TcpServer::create("*", 8000)};

  // Connect to the PRBS generator
  prbs->addSlave(tcp); 

  // Send frames at a constant rate
  while(true) {
    std::cout << "Sending frame" << std::endl;
    prbs->genFrame(1000); 
    std::cout << "Tx count: " << prbs->getTxCount() << " "
	      << "Tx bytes: " << prbs->getTxBytes() << " "
	      << "Tx errors: " << prbs->getTxErrors()
	      << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  return EXIT_SUCCESS; 
}
