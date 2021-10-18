#include "eudaq/TcpCommandGenerator.h"

//---< StdLib >---//
#include <stdio.h>
#include <iostream>

//---< rogue >---//
#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace eudaq {

void TcpCommandGenerator::genFrame(uint8_t command) {

  auto frame{reqFrame(1, true)};

  // Update frame payload size
  frame->setPayload(1);

  // Get an iterator to the beginning of the frame
  auto it{frame->begin()};

  // Push the string
  toFrame(it, 1, &command);

  // Pass the frame to the TCP server
  sendFrame(frame);
}


  void TcpCommandGenerator::send_buffer(char buffer[], const uint& size) {

    rogue::interfaces::stream::FramePtr frame;
    rogue::interfaces::stream::FrameIterator it;
    
    frame = reqFrame(size,true);
    frame->setPayload(size);
    it=frame->begin();
    rogue::interfaces::stream::toFrame(it,size,&buffer);
    sendFrame(frame);
        
  }
  
  void TcpCommandGenerator::send_float(const float& value) {

    std::string val = std::to_string(value);
    const uint size = val.length();
    char buffer[size+1];
    std::strcpy(buffer,val.c_str());

    //send_buffer(buffer,size);
    
    rogue::interfaces::stream::FramePtr frame;
    rogue::interfaces::stream::FrameIterator it;

    frame = reqFrame(size,true);
    frame->setPayload(size);
    it=frame->begin();
    rogue::interfaces::stream::toFrame(it,size,&buffer);
    sendFrame(frame);
    
  }

  void TcpCommandGenerator::send_var(const std::string& varname, const float& value) {

    //add setVariable command
    
    std::string command = "setVariable:";

    //add varname
    command+=varname+":";

    //float to string
    std::string val = std::to_string(value);
    command+=val;    

    send_cmd(command);

  }
  
  void TcpCommandGenerator::send_cmd(const std::string& command) {
    
    const uint size = command.length();
    char buffer[size+1];
    std::strcpy(buffer, command.c_str());
    rogue::interfaces::stream::FramePtr frame;
    rogue::interfaces::stream::FrameIterator it;
    //Request buffer 
    frame = reqFrame(size,true);

    std::cout<<"size "<<size<<std::endl;
    std::cout<<"m_cmd.data() " << command.data()<<std::endl;
    std::cout<<"buffer " << buffer<<std::endl;
    
    // Update frame payload size
    frame->setPayload(size);
    
    // Get an iterator to the start of the Frame
    it = frame->begin();
    
    // Push the string
    rogue::interfaces::stream::toFrame(it,size,&buffer);
    sendFrame(frame);
  }
  

  
} // namespace eudaq
