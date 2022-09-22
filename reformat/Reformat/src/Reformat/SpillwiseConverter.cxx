
#include "Reformat/SpillwiseConverter.h"

#include <ctime>
#include <queue>
#include <algorithm>

#include "Framework/EventFile.h"
#include "Framework/Event.h"
#include "Framework/EventHeader.h"
#include "Framework/RunHeader.h"
#include "TTimeStamp.h"

#include "Reformat/Utility/TimestampCloud.h"

namespace reformat {

SpillwiseConverter::~SpillwiseConverter() {
  framework::logging::close();
}

void SpillwiseConverter::configure(const framework::config::Parameters& cfg) {
  reformat_log(fatal) << "Configuring SpillwiseConverter" << "\n";
  int tl{cfg.getParameter<int>("term_level",4)},
      fl{cfg.getParameter<int>("file_level",4)};
  framework::logging::open(
      framework::logging::convertLevel(tl),
      framework::logging::convertLevel(fl),
      cfg.getParameter<std::string>("log_file",""));
  reformat_log(info) << "Beginning configuration.";

  pass_ = cfg.getParameter<std::string>("pass_name");  // pass is a keyword in Python
  output_filename_ = cfg.getParameter<std::string>("output_filename");
  run_ = cfg.getParameter<int>("run");
  start_event_ = cfg.getParameter<int>("start_event");
  //max_diff_ = cfg.getParameter<int>("max_diff");
  keep_all_ = cfg.getParameter<bool>("keep_all");
  event_limit_ = cfg.getParameter<int>("event_limit");
  detector_name_ = cfg.getParameter<std::string>("detector_name");
  print_frequency_ = cfg.getParameter<int>("print_frequency");
  require_WR_ = cfg.getParameter<bool>("require_WR");

  reformat_log(info) << "Converter parameters: {\n\t"
    << "pass : " << pass_ << ",\n\t"
    << "output_filename : " << output_filename_ << ",\n\t"
    << "run : " << run_ << ",\n\t"
    << "start_event : " << start_event_ << ",\n\t"
    //<< "max_diff : " << max_diff_ << ",\n\t"
    << "event_limit : " << event_limit_ << ",\n\t"
    << "detector_name : " << detector_name_ << ",\n\t"
    << "keep_all : " << keep_all_ << ",\n\t"
    << "require_WR : " << require_WR_ << "\n}";

  // Framework already has a library loading mechanism, no need to repeat ourselves
  auto libs{
      cfg.getParameter<std::vector<std::string>>("libraries", {})};
  std::for_each(libs.begin(), libs.end(), [](auto &lib) {
    RawDataFileFactory::get().loadLibrary(lib);
  });

  auto input_files{
      cfg.getParameter<std::vector<framework::config::Parameters>>(
          "input_files", {})};
  if (input_files.empty()) {
    EXCEPTION_RAISE(
        "NoInputFiles",
        "No input files have been defined. What should I be doing?\n"
        "Use c.input_files to tell me what files to convert.");
  }
  for (auto f : input_files) {
    auto class_name{f.getParameter<std::string>("class_name")};
    input_files_.emplace_back(RawDataFileFactory::get().create(class_name, f));
  }
  //TODO Errors when this is uncommented
  /*
  std::set<std::string> names;
  for (const auto& f : input_files_) {
    reformat_log(info) << f->name() << "\n";
    if (names.find(f->name()) == names.end()) {
      names.insert(f->name());
    } else {
      EXCEPTION_RAISE("NameRepeat",
          "Two input files have the same name.");
    }
  }
  */
  reformat_log(info) << "Configuration complete.";
}

void SpillwiseConverter::convert() {
  
  reformat_log(fatal) << "Beginning conversion";
  framework::Event output_event(pass_);
  framework::EventFile output_file(
      {}, output_filename_, 
      nullptr, true, true, false);
  output_file.setupEvent(&output_event);

  ldmx::RunHeader run_header(run_);
  run_header.setRunStart(std::time(nullptr));
  run_header.setDetectorName(detector_name_);
  output_file.writeRunHeader(run_header);

  reformat_log(fatal) << "Initialized Event, EventFile, and RunHeader";

  // initialize empty queues for all input files
  std::map<std::string,std::queue<EventPacket>> event_queue;
  for (const auto& f : input_files_) {
    event_queue[f->name()];
    reformat_log(fatal) << "Inialized event queue for " << f->name();
  }

  // Read WR file (and check whether one exists, otherwise we are probably not interested)
  int i_event{0};
  while (not input_files_.empty()) {
    auto f_it = input_files_.begin();
    while (f_it != input_files_.end()) {
        //reformat_log(fatal) << (*f_it)->name() << "\n";

        auto ep = (*f_it)->next();
        if (ep) {
            event_queue[(*f_it)->name()].push(ep.value());
            reformat_log(debug) << (*f_it)->name() << " popped " << ep.value().data().size() 
              << " bytes from timestamp " << ep.value().timestamp();
            ++f_it;
        } else {
            // file says no more events
            reformat_log(fatal) << (*f_it)->name() << " is done and removed from files on event " << i_event;
            reformat_log(fatal) << "Size: " << event_queue[(*f_it)->name()].size() << "\n"; 
            f_it = input_files_.erase(f_it);
        } 

    }
  } 

  reformat_log(fatal) << "Finished Reading Files\n";
  reformat_log(fatal) << "Splitting into spills\n";

  
  std::vector<std::vector<EventPacket>> FT50Spills;
  std::vector<std::vector<EventPacket>> FT51Spills;
  std::vector<std::vector<EventPacket>> WRSpills;
  std::vector<std::vector<EventPacket>> TSSpills;

  //Splitting events into groups of spills works slightly differently for each subsystem
  for( auto iter = event_queue.begin(); iter != event_queue.end(); ++iter){
    
    
    reformat_log(fatal) << iter->first << "\n";

    if(iter->first=="FT50Raw"){
      reformat_log(debug) << "Splitting FT50Raw" << "\n";
      std::vector<EventPacket> spill;
      while( !iter->second.empty()){
        auto e = iter->second.front();
        iter->second.pop();
        if( e.timestamp() >= 0xffffffff00000000){
            FT50Spills.push_back(spill);
            spill.clear();
            //reformat_log(fatal) << e.timestamp() << "\n";
            reformat_log(fatal) << "Added new spill, total: " << FT50Spills.size() << "\n";
        }
        else{
          spill.push_back(e);
        }
      }
    }

    if(iter->first=="FT51Raw"){
      reformat_log(debug) << "Splitting FT51Raw" << "\n";
      std::vector<EventPacket> spill;
      while( !iter->second.empty()){
        auto e = iter->second.front();
        iter->second.pop();
        if( e.timestamp() >= 0xffffffff00000000){
            FT51Spills.push_back(spill);
            spill.clear();
            //reformat_log(fatal) << e.timestamp() << "\n";
            reformat_log(fatal) << "Added new spill, total: " << FT51Spills.size() << "\n";
        }
        else{
          spill.push_back(e);
        }
      }
    }
     
    
    if(iter->first=="WhiteRabbitRaw"){
      reformat_log(debug) << "Splitting FT51Raw" << "\n";
      std::vector<EventPacket> spill;
      while( !iter->second.empty()){
        auto e = iter->second.front();
        iter->second.pop();
        //TODO Reconsider this type casting
        std::vector<uint8_t> datachar = e.data();
        std::vector<uint32_t>* datap = (std::vector<uint32_t>*)&datachar;
        //reformat_log(fatal) << "Event channel: " << (*datap).at(2) << "\n";
        if( (*datap).at(2) == 1){
            WRSpills.push_back(spill);
            reformat_log(fatal) << "Added new spill, total: " << WRSpills.size()+1 << " with events: " << spill.size() <<"\n";
            spill.clear();          
            //reformat_log(fatal) << data.at(2) << "\n";
        }
        //TODO Only consider triggers ( == 2 ) right now, not cherenkov "events" (== 3 or == 4)
        if( (*datap).at(2) == 2){
          spill.push_back(e);
        }
      }
    }
    
    if(iter->first=="TrigScintRaw"){
      int spillCounter = 0;
      std::vector<EventPacket> spill;
      while(!iter->second.empty()){

        auto e = iter->second.front();
        //reformat_log(fatal) << e.timestamp() << "\n";
        //reformat_log(fatal) << spill.size() << "\n";
        iter->second.pop();
        
        if((e.timestamp() >> 32) != spillCounter){

          spillCounter = (e.timestamp() >> 32);
          TSSpills.push_back(spill);
          reformat_log(fatal) << "New spill " << spillCounter << " size: " << spill.size() << "\n";
          spill.clear();
        }

        spill.push_back(e);
      }
    }
  }
  
  reformat_log(info) << "Finished splitting into spills\n";

  /*
  double maxEff = 0;
  int bestDelay = 0;
  for(int delay = 2096000; delay <= 2099000; delay += 10){
    reformat_log(fatal) << delay;
    std::string filenamewr = "WRRaw";
    utility::TimestampCloud spillwr(WRSpills.at(18), filenamewr);
    std::string filenameft50 = "FT50Raw";
    utility::TimestampCloud spillft50(FT50Spills.at(1), filenameft50);
    spillft50.translate(delay);
   
    std::vector<int> pairsft50;
    pairsft50 = spillwr.nnpair(spillft50);

    double eff = 1.0-std::count(pairsft50.begin(), pairsft50.end(), -1)/(double)pairsft50.size();
    if(eff > maxEff){
      maxEff = eff;
      bestDelay = delay;
    }

  }
  reformat_log(fatal) << maxEff << "\n" << bestDelay; 
  */

  // deltaT between scintillator telescope triggers in WR and downstream fiber trackers = -2096660 ns
  
  //TODO Sketch of TS to WR Alignment 
  reformat_log(info) << "Pairing TS and WR\n";
 
    
  for(int TSSpillNumber = 0; TSSpillNumber < TSSpills.size(); TSSpillNumber++){
    reformat_log(fatal) << "TSSpillNumber " << TSSpillNumber << "\n";
    bool found = false;
    bool foundFT50 = false;
    for(int i = 1; i < WRSpills.size(); i++){
      if(!found){
        //reformat_log(fatal) << "Spill " << i << "\n";
       
        for(int k = 0; k < 15; k++){
          std::vector<int> pairs;
          std::string filenamewr = "WRRaw";
          utility::TimestampCloud spillwr(WRSpills.at(i), filenamewr);
          std::string filenamets = "TSGrouped";
          utility::TimestampCloud spillts(TSSpills.at(TSSpillNumber), filenamets);

          //reformat_log(fatal) << "WR Length: " << spillwr.points.size() << "\n";
          //reformat_log(fatal) << "TS Length: " << spillts.points.size() << "\n";

          //for(int j = 0; j < spillwr.points.size(); j++){
          //  reformat_log(fatal) << spillwr.points.at(j) << "\n";
          //}

          if(spillwr.points.size() > 50){
            double dt = 0;

            spillts.translate(-spillts.points.at(0));
            //One clock tick is ~8 ns
            spillts.scale(8.000046470962321);
            dt += spillwr.points.at(0);
            spillwr.translate(-spillwr.points.at(0));
            dt += spillwr.points.at(k);
            spillwr.translate(-spillwr.points.at(k));

            pairs = spillts.nnpair(spillwr);
            //reformat_log(fatal) << "    " << pairs.size() << " : " << std::count(pairs.begin(), pairs.end(), -1) << "\n";
            //reformat_log(fatal) << 1.0-std::count(pairs.begin(), pairs.end(), -1)/(double)pairs.size() << "\n";
            if(1.0-std::count(pairs.begin(), pairs.end(), -1)/(double)pairs.size() > 0.1) {
              found = true; 
              reformat_log(fatal) << "Spill " << i << "\n";
              reformat_log(fatal) << 1.0-std::count(pairs.begin(), pairs.end(), -1)/(double)pairs.size() << "\n";
              for(int j = 0; j < FT50Spills.size(); j++){
                std::vector<int> pairsft50;
                std::string filename50 = "FT50Raw";
                utility::TimestampCloud spill50(FT50Spills.at(j), filename50);

                spill50.translate(-dt);
                //spill50.translate(2097700);
                spill50.translate(2096660);
                pairsft50 = spillts.nnpair(spill50);

                //reformat_log(fatal) << "FT50 efficiency " << 1.0-std::count(pairsft50.begin(), pairsft50.end(), -1)/(double)pairsft50.size() << "\n";
                if(1.0-std::count(pairsft50.begin(), pairsft50.end(), -1)/(double)pairsft50.size() > 0.1) {
                  foundFT50 = true; 
                  reformat_log(fatal) << "TS Spill Number: " << TSSpillNumber<<", TS to FT50 efficiency: " << 1.0-std::count(pairsft50.begin(), pairsft50.end(), -1)/(double)pairsft50.size() << "\n";


                  
                  //Build events!
                  for(int event_i = 0; event_i < spillts.points.size(); event_i++){
                    ldmx::EventHeader& eh = output_event.getEventHeader();
                    eh.setRun(run_);
                    eh.setEventNumber(i_event++);
                    eh.setTimestamp(TTimeStamp()); // related to earliest_ts ???
                    eh.setRealData(true); // probably smart...
                                          
                    output_event.add("TSAligned", TSSpills.at(TSSpillNumber).at(event_i).data());

                    output_file.nextEvent(true);
                  }
                  reformat_log(fatal) << "Added " << spillts.points.size() << " events\n";
                }
              }
            };
          }
        }
      }
    }
    if(!foundFT50){
      reformat_log(fatal) << "TS Spill Number: " << TSSpillNumber<<", TS to FT50 efficiency: " << 0.0 << "\n";
    }
    if(!found){
      reformat_log(fatal) << "Could not align TS spill" << "\n";

      //Add empty events
    }
  }
  
  
  run_header.setRunEnd(std::time(nullptr));
  output_file.close();
  reformat_log(info) << "Conversion complete.";
  
}

}  // namespace reformat
