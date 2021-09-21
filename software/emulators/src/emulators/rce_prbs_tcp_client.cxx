//---< C++ >---//
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

//---< rogue >---//
#include "rogue/utilities/Prbs.h"
#include "rogue/interfaces/stream/TcpClient.h" 

using namespace std;

int main(int argc, char **argv) { 	

  // Create a PRBS receiver
  auto prbs{rogue::utilities::Prbs::create()};

  // Start a TCP bridge client and connect to the remove server at 
  // ports 8000 & 8001. For now, this just connects to localhost.
  auto tcp{rogue::interfaces::stream::TcpClient::create("127.0.0.1", 8000)};


  // Connect to the PRBS generator
  tcp->addSlave(prbs);

  while(true) {
    std::cout << "Rx count: " << prbs->getRxCount() << " "
	      << "Rx bytes: " << prbs->getRxBytes() << " "
	      << "Rx errors: " << prbs->getRxErrors()
	      << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  }
  return EXIT_SUCCESS; 
}
