#ifndef ROGUE_TBSTREAMWRITER_H
#define ROGUE_TBSTREAMWRITER_H

#include "rogue/utilities/fileio/StreamWriter.h"
#include <rogue/interfaces/stream/Frame.h>
#include <rogue/interfaces/stream/Buffer.h>
#include <iostream>

namespace rogue{
  class TBStreamWriter : public rogue::utilities::fileio::StreamWriter {

  public:
  TBStreamWriter() : rogue::utilities::fileio::StreamWriter(){};

    virtual ~TBStreamWriter(){this->close();};
    
    static std::shared_ptr<rogue::TBStreamWriter> create();

    void setOffset(unsigned int offset){
      offset_ = offset;
      std::cout<<"Setting offset to "<<offset_<<std::endl;
    }
    
  protected:
    
    //Skip some portion of the initial buffer
    unsigned int offset_{0};
    
    virtual void writeFile (uint8_t channel, std::shared_ptr<rogue::interfaces::stream::Frame> frame);
    
    
        
    
  };

  typedef std::shared_ptr<rogue::TBStreamWriter> TBStreamWriterPtr;

} //rogue
#endif
