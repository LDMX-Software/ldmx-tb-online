//---< C++ >---//
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

//---< ldmx-rogue >---//
#include "rogue/TcpCommandReceiver.h" 

//---< rogue >---//
#include "rogue/interfaces/stream/TcpServer.h" 

using namespace std; 

int main(int argc, char **argv) {

  // Start a TCP bridge server and listen on all interfaces,
  //  ports 8000 & 8001
  auto tcp{rogue::interfaces::stream::TcpServer::create("*", 8000)};

  // Create the TCP command receiver
  auto tcp_command{rogue::TcpCommandReceiver::create()}; 

  // Connect the TCP command receiver to the tcp rogue client
  tcp->addSlave(tcp_command); 

  // Connect to the PRBS generator
  //prbs->addSlave(tcp); 

  // Send frames at a constant rate
  bool running{false}; 
  while(true) {
    if (!running & tcp_command->startRun()) { 
	std::cout << "[ LDMX ] Starting run." << std::endl;
	running = true;
    } else if (running & tcp_command->stopRun()) { 
	std::cout << "[ LDMX ] Stopping run." << std::endl;
	running = false;
    }

	  //prbs->genFrame(1000); 
    //std::cout << "Tx count: " << prbs->getTxCount() << " "
    //	      << "Tx bytes: " << prbs->getTxBytes() << " "
    //	      << "Tx errors: " << prbs->getTxErrors()
    //	      << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  return EXIT_SUCCESS; 
}
