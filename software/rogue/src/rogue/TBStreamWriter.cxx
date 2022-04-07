#include "rogue/TBStreamWriter.h"


rogue::TBStreamWriterPtr rogue::TBStreamWriter::create() {
  rogue::TBStreamWriterPtr s = std::make_shared<rogue::TBStreamWriter>();
  return(s);
}

void rogue::TBStreamWriter::writeFile(uint8_t channel, std::shared_ptr<rogue::interfaces::stream::Frame> frame) {
  std::cout<<"nothing to do here"<<std::endl;
  
  rogue::interfaces::stream::Frame::BufferIterator it;
  uint32_t value;
  uint32_t size;
  
  if ( (frame->getPayload() == 0) || (dropErrors_ && (frame->getError() != 0)) ) return;
  
  //rogue::GilRelease noGil;
  std::unique_lock<std::mutex> lock(mtx_);
  
  if ( fd_ >= 0 ) {
    
    // Written size has extra 4 bytes
    size = frame->getPayload() + 4;
    
    // Check file size, including size header
    checkSize(size+4);
    
    // First write size
    intWrite(&size,4);
    
    // Create EVIO header
    value  = frame->getFlags();
    value |= (frame->getError() << 16);
    value |= (channel << 24);
    intWrite(&value,4);
    
    // Write buffers
    for (it=(frame->beginBuffer() + offset_); it != frame->endBuffer(); ++it)
      intWrite((*it)->begin(),(*it)->getPayload());
    
    // Update counters
    frameCount_ ++;
    cond_.notify_all();
  } 
}

