#include "eudaq/CaptanTrigScintTestBeamProducer.h"

//---< ldmx-eudaq >---//
#include "eudaq/TrigScintDataSender.h"

namespace {
  auto dummy0 = eudaq::Factory<eudaq::Producer>::Register<
    eudaq::CaptanTrigScintTestBeamProducer, const std::string &, const std::string &>(
										      eudaq::CaptanTrigScintTestBeamProducer::factory_id_);
}

namespace eudaq {

  CaptanTrigScintTestBeamProducer::CaptanTrigScintTestBeamProducer(const std::string &name, const std::string &runcontrol)
    : RogueTcpClientProducer(name, runcontrol) {
    sender_ = TrigScintDataSender::create(this);
  }

  void CaptanTrigScintTestBeamProducer::DoInitialise() {
    /**
     * Add initialization here ...
     */
    auto ini{GetInitConfiguration()};
    
    // Get the UDP server address and server from the configuration
    // Default: localhost:9999
    auto addr{ini->Get("TCP_ADDR", "127.0.0.1")};
    auto port{ini->Get("TCP_PORT", 9999)};
    EUDAQ_INFO("TCP client listening on " + addr + ":" + std::to_string(port));
    
    // Open the UDP client

    udp_client_ = TsUdpClient::create(addr,port,false);


    // Connect the UDP client to the file writer

    // Set how much data to buffer before writing
    writer_->setBufferSize(10000);
    udp_client_->addSlave(writer_->getChannel(0)); 

    // Connect the data sender to the TCP client
    udp_client_->addSlave(sender_);
    
  }

  void CaptanTrigScintTestBeamProducer::DoConfigure() {

    auto conf{GetConfiguration()};
    
    // Get the path to the output file
    output_path_ = conf->Get("OUTPUT_PATH", ".");
    
    // Get the file prefix
    file_prefix_ = conf->Get("ROGUE_FILE_PATTERN", "test"); 
    
    // Build the file name
    auto output_file{output_path_ + "/" + file_prefix_ + "_" + std::to_string(GetRunNumber()) + ".dat"}; 
    
    // First, make sure an existing file isn't open.
    if (writer_->isOpen()) writer_->close(); 
    
    // Open a file to write the stream
    writer_->open(output_file);
    
    EUDAQ_INFO("Writing rogue stream to " + output_file);

    
  }

  void CaptanTrigScintTestBeamProducer::DoStartRun() {

    udp_client_->dataEnableToggle(false);
    udp_client_->dataEnableToggle(true);

  }
  

  void CaptanTrigScintTestBeamProducer::DoStopRun() {
    
    udp_client_->dataEnableToggle(false);
  }


  
} // namespace eudaq


