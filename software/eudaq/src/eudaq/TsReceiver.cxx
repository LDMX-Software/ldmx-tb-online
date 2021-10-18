#include "eudaq/TsReceiver.h"
#include <iostream>
#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace eudaq {


  std::string TsReceiver::convertToString(char* a, int size)
  {
    int i;
    std::string s = "";
    for (i = 0; i < size; i++) {
      s = s + a[i];
    }
    return s;
  }
  
  
  void TsReceiver::acceptFrame(std::shared_ptr<rogue::interfaces::stream::Frame> frame){

    if (frame->getError()) {
      return;
    }
    auto it{frame->begin()};

    const uint size = frame->getPayload();
    std::cout<<"Payload  "<<size<<std::endl;
    char buffer[size+1];

    //Copy to a buffer
    std::copy(frame->begin(), frame->end(), buffer);
    
    //convert to a string

    std::string result=convertToString(buffer,size);


    std::cout<<result<<std::endl;
    // Print the values in the first 10 locations
    //for (int x=0; x < size; x++) {
    //printf("Entry: %d\n", *it);
    //it++;
    //}
    
  }
}
