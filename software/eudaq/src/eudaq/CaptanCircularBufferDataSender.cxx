#include "eudaq/CaptanCircularBufferDataSender.h"

#include <rogue/interfaces/stream/Buffer.h>

//---< C++ StdLib >---//
#include <bitset>
#include <cstring>
#include <iostream>

//---< rogue >---//
#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace eudaq {

size_t CaptanCircularBufferDataSender::writeIntoCircularBuffer(const char* data_loc, size_t data_size)
{
  
  if(mCircularBuffer.capacity()-mCircularBuffer.size() < data_size){
    std::cout << "ERROR: NOT ENOUGH SPACE IN BUFFER!" << std::endl;
    return 0;
  }
  
  std::unique_lock<std::mutex> lock(*(mBufferLock));
  mCircularBuffer.insert(mCircularBuffer.end(),data_loc,data_loc+data_size);
  if(!mCircularBuffer.is_linearized()) mCircularBuffer.linearize();
  return mCircularBuffer.size();
}

size_t CaptanCircularBufferDataSender::deleteFromCircularBuffer(size_t data_size)
{
  std::unique_lock<std::mutex> lock(*(mBufferLock));
  mCircularBuffer.erase_begin(data_size);
  if(!mCircularBuffer.is_linearized()) mCircularBuffer.linearize();
  return mCircularBuffer.size();
}

size_t CaptanCircularBufferDataSender::readFromCircularBuffer()
{  
  size_t size_to_read_bytes = mCircularBuffer.size();
  size_t n_words_to_read = size_to_read_bytes/sizeof(uint64_t);

  char* dataptr = mCircularBuffer.array_one().first;

  char* event_begin=nullptr;

  uint64_t this_word=0;
  uint64_t n_words_in_event=0;
  
  for(size_t i_w=0; i_w<n_words_to_read; ++i_w)
  {
    this_word = (uint64_t)(*dataptr+i_w);
    if(this_word==0xFFFFFFFF && event_begin==nullptr){
      event_begin=dataptr;
      continue;
    }
    else if(this_word==0xFFFFFFFF && n_words_in_event==0){
      n_words_in_event=i_w;
      break;
    }
  }

  if(n_words_in_event==0)
    return n_words_to_read*sizeof(uint64_t);

  //now do the sending piece of the code ... 
  
  auto event{eudaq::Event::MakeUnique("TrigScintTestBeamRaw")};

  // Copy the data block from the circular buffer
  event->AddBlock(0x2, event_begin, n_words_in_event*sizeof(uint64_t));

  // Send the event to the data collectors
  producer_->SendEvent(std::move(event)); 

  //now delete...
  deleteFromCircularBuffer(n_words_in_event*sizeof(uint64_t));  
  return 0;
}


void CaptanCircularBufferDataSender::sendEvent(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {


  size_t offset_bytes=2;

  //loop over buffers in frame, writing them into circular buffer...
  for (auto it=(frame->beginBuffer()); it != frame->endBuffer(); ++it){
      size_t written = writeIntoCircularBuffer((char*)(*it)->begin()+offset_bytes,(*it)->getPayload()-offset_bytes);
      if(written==0){
        std::cout << "ERROR: FAILED TO WRITE FRAME TO CIRCULAR BUFFER!" << std::endl;
      }
  }

  //now read from circular buffer, quitting if circular buffer is basically empty
  //or if we read to end and we are missing end of event
  size_t read_pos=0;
  while(read_pos==0 && mCircularBuffer.size()>sizeof(uint64_t))
    read_pos=readFromCircularBuffer();

  
}
} // namespace eudaq
