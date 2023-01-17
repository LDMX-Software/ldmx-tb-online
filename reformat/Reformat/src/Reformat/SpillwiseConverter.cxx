
#include "Reformat/SpillwiseConverter.h"

#include <ctime>
#include <queue>
#include <algorithm>
#include <iomanip>

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

std::vector<int> pairTStoFT( utility::TimestampCloud& spillts, std::vector<std::vector<EventPacket>> FTSpills, double dt, int& spillnumber, utility::TimestampCloud& finalcloud, bool isCherenkov = false){
    std::vector<int> finalftpairs;   

    for(int j = 0; j < FTSpills.size(); j++){
      std::vector<int> pairsft;
      std::string filename = "FTRaw";
      utility::TimestampCloud spill(FTSpills.at(j), filename);

      spill.translate(-dt);
      //spill.translate(2096900); //time difference between WR and downstream FT, currently hard coded here, but there should be some automatic alignment routine made
      if(isCherenkov){
        //spill.translate(4948224); //time difference between WR trigger and cherenkovs,  currently hard coded here, but there should be some automatic alignment routine made
        spill.translate(0); //time difference between WR trigger and cherenkovs,  currently hard coded here, but there should be some automatic alignment routine made
      }
      else{
        spill.translate(2097060); //time difference between WR and downstream FT, currently hard coded here, but there should be some automatic alignment routine made
      }
      pairsft = spillts.nnpair(spill);


      if(1.0-std::count(pairsft.begin(), pairsft.end(), -1)/(double)pairsft.size() > 0.1) {
        finalftpairs = pairsft;
        finalcloud = spill;

        spillnumber = j;
      }
  }
  return finalftpairs;
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
  std::vector<std::vector<EventPacket>> WRSpills; // 2
  std::vector<std::vector<EventPacket>> LowPressureSpills; // 4
  std::vector<std::vector<EventPacket>> HighPressureSpills; // 3
  std::vector<std::vector<EventPacket>> TSSpills;
  std::vector<std::vector<EventPacket>> HCalSpills;

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
      reformat_log(debug) << "Splitting WRRaw" << "\n";
      std::vector<EventPacket> spill;
      std::vector<EventPacket> spillhighp;
      std::vector<EventPacket> spilllowp;
      while( !iter->second.empty()){
        auto e = iter->second.front();
        iter->second.pop();
        //TODO Reconsider this type casting
        std::vector<uint8_t> datachar = e.data();
        std::vector<uint32_t>* datap = (std::vector<uint32_t>*)&datachar;
        //reformat_log(fatal) << "Event channel: " << (*datap).at(2) << "\n";
        if( (*datap).at(2) == 1){
            WRSpills.push_back(spill);
            HighPressureSpills.push_back(spillhighp);
            LowPressureSpills.push_back(spilllowp);
            reformat_log(fatal) << "Added new spill, total: " << WRSpills.size()+1 << " with events: " << spill.size() <<"\n";
            spill.clear();          
            spillhighp.clear();
            spilllowp.clear();
            //reformat_log(fatal) << data.at(2) << "\n";
        }
        if( (*datap).at(2) == 2){
          spill.push_back(e);
        }
        if( (*datap).at(2) == 3){
          spillhighp.push_back(e);
        }
        if( (*datap).at(2) == 4){
          spilllowp.push_back(e);
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

 
  //Sketch of finding WR to FT delay 
  /*
  double maxEff = 0;
  int bestDelay = 0;
  for(int delay = 2096000; delay <= 2099000; delay += 10){
    reformat_log(fatal) << delay;
    std::string filenamewr = "WRRaw";
    utility::TimestampCloud spillwr(WRSpills.at(1), filenamewr);
    std::string filenameft50 = "FT50Raw";
    utility::TimestampCloud spillft50(FT50Spills.at(1), filenameft50);
    spillft50.translate(delay);
   
    std::vector<int> pairsft50;
    pairsft50 = spillwr.nnpair(spillft50);

    double eff = 1.0-std::count(pairsft50.begin(), pairsft50.end(), -1)/(double)pairsft50.size();
    if(eff > maxEff){
      reformat_log(fatal) << "eff " << eff << "\n";
      maxEff = eff;
      bestDelay = delay;
    }

  }
  reformat_log(fatal) << "eff bestDelay  " << maxEff << " : " << bestDelay; 
  */

  // deltaT between scintillator telescope triggers in WR and downstream fiber trackers = -2096660 ns
  
  //TODO Sketch of TS to WR Alignment 
  reformat_log(info) << "Pairing TS and WR\n";
 
  //Skip zeroeth spill in the white rabbit, since it seems to be always empty
  int WRSpillOffset = 1;

  /*
   * Calibrate dT between trigger and cherenkovs
   */

  uint64_t lowPressuredT = 0;
  uint64_t highPressuredT = 0;

  reformat_log(fatal) << LowPressureSpills.size() << std::endl;

  /*
  std::string filenamewr = "WRRaw";
  utility::TimestampCloud spillwr(WRSpills.at(10), filenamewr);
  
  std::string filenamelowpressure = "LowPressureRaw";
  utility::TimestampCloud spilllowpressure(LowPressureSpills.at(10), filenamelowpressure);
 
  reformat_log(fatal) << "Created spill" << std::endl;

  //for(int i = 0; i < 20; i++){
  //  reformat_log(fatal) << "dT " << spillwr.points.at(i) << " " << spilllowpressure.points.at(i) << std::endl;
  //}
  */

  double bestLowPressureEff = 0.0;

  for(int k = 0; k < 30; k++){
    for(int skipTSEvents = 0; skipTSEvents < 10; skipTSEvents++){
      std::vector<int> pairs;
      std::string filenamewr = "WRRaw";
      utility::TimestampCloud spillwr(WRSpills.at(10), filenamewr);
      std::string filenamelowpressure = "LowPressureRaw";
      utility::TimestampCloud spilllowpressure(LowPressureSpills.at(10), filenamelowpressure);
      if(spillwr.points.size() > 10){ //No need to attempt alignment on very small spills
        double dt = 0;

        spilllowpressure.translate(-spilllowpressure.points.at(0));
        spilllowpressure.translate(-spilllowpressure.points.at(skipTSEvents));
        //One clock tick is ~8 ns
        //spillts.scale(8.000046470962321);
        //dt += spillwr.points.at(0);
        spillwr.translate(-spillwr.points.at(0));
        dt += spillwr.points.at(k);
        spillwr.translate(-spillwr.points.at(k));

        pairs = spilllowpressure.nnpair(spillwr);

        double eff = 1.0-std::count(pairs.begin(), pairs.end(), -1)/(double)pairs.size();

        if(eff > bestLowPressureEff){
          lowPressuredT = dt;
          bestLowPressureEff = eff;
        }
      }
    }
  }

  reformat_log(fatal) << "lowPressuredT: " << lowPressuredT << std::endl;
  reformat_log(fatal) << "bestLowPressureEff: " << bestLowPressureEff << std::endl;

  /*
   * Find the WR spill corresponding to the zeroeth TS spill, for the TS frequency calibration
   */

  int firstWRSpill = -1;
  bool foundSpillCorrespondance = false;

  for(int i = WRSpillOffset; i < WRSpills.size(); i++){
    if(!foundSpillCorrespondance){
      for(int k = 0; k < 30; k++){
        for(int skipTSEvents = 0; skipTSEvents < 10; skipTSEvents++){
          std::vector<int> pairs;
          std::string filenamewr = "WRRaw";
          utility::TimestampCloud spillwr(WRSpills.at(i), filenamewr);
          std::string filenamets = "TSGrouped";
          utility::TimestampCloud spillts(TSSpills.at(0), filenamets);
          if(spillwr.points.size() > 10){ //No need to attempt alignment on very small spills
            double dt = 0;

            spillts.translate(-spillts.points.at(0));
            spillts.translate(-spillts.points.at(skipTSEvents));
            //One clock tick is ~8 ns
            spillts.scale(8.000046470962321);
            dt += spillwr.points.at(0);
            spillwr.translate(-spillwr.points.at(0));
            dt += spillwr.points.at(k);
            spillwr.translate(-spillwr.points.at(k));

            pairs = spillts.nnpair(spillwr);

            double eff = 1.0-std::count(pairs.begin(), pairs.end(), -1)/(double)pairs.size();

            if(eff > 0.2){
              firstWRSpill = i;
              foundSpillCorrespondance = true;

            }
          }
        }
      }
    }
  }

  if(firstWRSpill != -1){
    WRSpillOffset = firstWRSpill;  
  }

  /*
   * TS Frequency Calibration
   */

  double TSFrequency = -1;
  double record = 0;

  for(int i = 0; i < 30; i++){

    for(int k = 0; k < 30; k++){
      for(int skipTSEvents = 0; skipTSEvents < 10; skipTSEvents++){
        std::vector<int> pairs;
        std::string filenamewr = "WRRaw";
        utility::TimestampCloud spillwr(WRSpills.at(firstWRSpill), filenamewr);
        std::string filenamets = "TSGrouped";
        utility::TimestampCloud spillts(TSSpills.at(0), filenamets);
        if(spillwr.points.size() > 10){ //No need to attempt alignment on very small spills
          double dt = 0;

          spillts.translate(-spillts.points.at(0));
          spillts.translate(-spillts.points.at(skipTSEvents));
          //One clock tick is ~8 ns
          //spillts.scale(8.000046470962321);
          spillts.scale(8.00004 + 0.000001*i );
          //spillts.scale(8.6);
          dt += spillwr.points.at(0);
          spillwr.translate(-spillwr.points.at(0));
          dt += spillwr.points.at(k);
          spillwr.translate(-spillwr.points.at(k));

          pairs = spillts.nnpair(spillwr);

          //If succesful TS to WR alignment, add FT
          double eff = 1.0-std::count(pairs.begin(), pairs.end(), -1)/(double)pairs.size();
          if(eff > record){
            record = eff;
            TSFrequency = 8.00004 + 0.000001*i;//+0.00001;
          }
        }
      }
    }
    reformat_log(fatal) << TSFrequency << " : ";
    reformat_log(fatal) << record << "\n";
  }
 
  reformat_log(fatal) << "Frequency: " << std::setprecision(15) << TSFrequency << "\n";

  /*
   * ALIGN ALL SPILLS
   *
   */


  for(int TSSpillNumber = 0; TSSpillNumber < TSSpills.size(); TSSpillNumber++){
    reformat_log(fatal) << "TSSpillNumber " << TSSpillNumber << "\n";
    
    //Results of alignment
    //Aligned WR to TS (the right spill has been *found*)
    bool found = false;
   
    double finaldt;

    utility::TimestampCloud finaltscloud;

    std::vector<int> finalwrpairs;
    utility::TimestampCloud finalwrcloud;
    int finalwrspillindex;

    bool foundFT50 = false;
    std::vector<int> finalft50pairs;
    int finalft50spillindex;
    utility::TimestampCloud finalft50cloud;
    
    bool foundFT51 = false;
    std::vector<int> finalft51pairs;
    int finalft51spillindex;
    utility::TimestampCloud finalft51cloud;
    
    bool foundLowPressure = false;
    std::vector<int> finallowpressurepairs;
    int finallowpressurespillindex;
    utility::TimestampCloud finallowpressurecloud;
    
    bool foundHighPressure = false;
    std::vector<int> finalhighpressurepairs;
    int finalhighpressurespillindex;
    utility::TimestampCloud finalhighpressurecloud;

    uint64_t startOfSpill = 0;

    /*
     * SUBSYSTEM ALIGNMENT
     */
    if(!found){
    for(int i = WRSpillOffset; i < WRSpills.size(); i++){
      //reformat_log(fatal) << "Trying WR Spill " << i << "\n";
      
      //if(!found){
      
         
        for(int k = 0; k < 15; k++){
          for(int skipTSEvents = 0; skipTSEvents < 5; skipTSEvents++){
            if(!found){
            std::vector<int> pairs;
            std::string filenamewr = "WRRaw";
            utility::TimestampCloud spillwr(WRSpills.at(i), filenamewr);
            std::string filenamets = "TSGrouped";
            utility::TimestampCloud spillts(TSSpills.at(TSSpillNumber), filenamets);

            //reformat_log(fatal) << "WR Length: " << spillwr.points.size() << "\n";
            //reformat_log(fatal) << "TS Length: " << spillts.points.size() << "\n";

            if(spillwr.points.size() > 10){ //No need to attempt alignment on very small spills
              double dt = 0;

              spillts.translate(-spillts.points.at(0));
              spillts.translate(-spillts.points.at(skipTSEvents));
              //One clock tick is ~8 ns
              //spillts.scale(8.000046470962321);
              spillts.scale(TSFrequency);
              dt += spillwr.points.at(0);
              spillwr.translate(-spillwr.points.at(0));
              dt += spillwr.points.at(k);
              spillwr.translate(-spillwr.points.at(k));

              pairs = spillts.nnpair(spillwr);
              
              //If succesful TS to WR alignment, add FT and cherenkovs
              if(1.0-std::count(pairs.begin(), pairs.end(), -1)/(double)pairs.size() > 0.8) {
                finalwrspillindex = i;

                startOfSpill = spillts.points.at(0);

                finaltscloud = spillts;
                finalwrcloud = spillwr;
                finalwrpairs = pairs;
                found = true;
                WRSpillOffset = i + 1;
                reformat_log(fatal) << "Spill " << i << "\n";
                reformat_log(fatal) << 1.0-std::count(pairs.begin(), pairs.end(), -1)/(double)pairs.size() << "\n";

                
                finalft50pairs = pairTStoFT(spillts, FT50Spills, dt, finalft50spillindex, finalft50cloud);
                if(finalft50pairs.size() > 0){
                  foundFT50 = true;
                }
                finalft51pairs = pairTStoFT(spillts, FT51Spills, dt, finalft51spillindex, finalft51cloud);
                if(finalft51pairs.size() > 0){
                  foundFT51 = true;
                }
               
                finallowpressurepairs = pairTStoFT(spillts, LowPressureSpills, dt, finallowpressurespillindex, finallowpressurecloud, true);
                if(finallowpressurepairs.size() > 0){
                  foundLowPressure = true;
                }
                
                finalhighpressurepairs = pairTStoFT(spillts, HighPressureSpills, dt, finalhighpressurespillindex, finalhighpressurecloud, true);
                if(finalhighpressurepairs.size() > 0){
                  foundHighPressure = true;
                }

                reformat_log(fatal) << "Tried to align FT\n";

                int completeEvent = 0;
                if(finalft50pairs.size() > 0 && finalft51pairs.size() > 0){
                  for(int event_i = 0; event_i < spillts.points.size(); event_i++){
                    if(finalft51pairs.at(event_i) != -1 && finalft50pairs.at(event_i) != -1){
                      completeEvent++;
                    }
                  }
                }
                reformat_log(fatal) << "ALL SUBSYSTEMS " << (double)completeEvent/(double)spillts.points.size() << std::endl;
                
                int lowpressureEvents = 0;
                if(finallowpressurepairs.size() > 0){
                  for(int event_i = 0; event_i < spillts.points.size(); event_i++){
                    if(finallowpressurepairs.at(event_i) != -1){
                      lowpressureEvents++;
                    }                  
                  }                    
                }
                reformat_log(fatal) << "LOW PRESSURE EFF " << (double)lowpressureEvents/(double)spillts.points.size() << std::endl;
                
                int highpressureEvents = 0;
                if(finalhighpressurepairs.size() > 0){
                  for(int event_i = 0; event_i < spillts.points.size(); event_i++){
                    if(finalhighpressurepairs.at(event_i) != -1){
                      highpressureEvents++;
                    }                  
                  }                    
                }
                reformat_log(fatal) << "HIGH PRESSURE EFF " << (double)highpressureEvents/(double)spillts.points.size() << std::endl;


                //Build events!
                /*
                for(int event_i = 0; event_i < spillts.points.size(); event_i++){
                  ldmx::EventHeader& eh = output_event.getEventHeader();
                  eh.setRun(run_);
                  eh.setEventNumber(i_event++);
                  eh.setTimestamp(TTimeStamp()); // related to earliest_ts ???
                  eh.setRealData(true); // probably smart...
                                        
                  output_event.add("QIEstreamUp", TSSpills.at(TSSpillNumber).at(event_i).data());
                  //f(pairsft50.at(event_i) != -1){
                  //  output_event.add("FT50Raw", pairsft50
                  //}

                  output_file.nextEvent(true);
                }
                reformat_log(fatal) << "Added " << spillts.points.size() << " events\n";
                */  
              }
            }
          }
          }
        }
      //}
    }
    }
   
    /*
     * EVENT BUILDING
     */
    if(found){
      for(int event_i = 0; event_i < TSSpills.at(TSSpillNumber).size(); event_i++){
        ldmx::EventHeader& eh = output_event.getEventHeader();
        eh.setRun(run_);
        eh.setEventNumber(++i_event);
        eh.setTimestamp(TTimeStamp()); // related to earliest_ts ???
        eh.setRealData(true); // probably smart...
                              

        //Calculate WR information
        std::vector<uint32_t> WRdata;
        
        uint32_t deltaTTrigger;
        if(finalwrpairs.at(event_i) >= 0){
          deltaTTrigger = (uint32_t)std::clamp(std::abs((int)finaltscloud.points.at(event_i)-(int)finalwrcloud.points.at(finalwrpairs.at(event_i))),0,65535);
        }else{
          deltaTTrigger = 65535;
        }
        WRdata.push_back(deltaTTrigger);
        
        uint32_t deltaTDownstreamHorizontal = 65535;
        if(foundFT50){
          if(finalft50pairs.at(event_i) >= 0){
            deltaTDownstreamHorizontal = (uint32_t)std::clamp(std::abs((int)finaltscloud.points.at(event_i)-(int)finalft50cloud.points.at(finalft50pairs.at(event_i))),0,65535);
          }
        }
        WRdata.push_back(deltaTDownstreamHorizontal);
       

        uint32_t deltaTDownstreamVertical = 65535;
        if(foundFT51){
          if(finalft51pairs.at(event_i) >= 0){
            deltaTDownstreamVertical = (uint32_t)std::clamp(std::abs((int)finaltscloud.points.at(event_i)-(int)finalft51cloud.points.at(finalft51pairs.at(event_i))),0,65535);
          }
        }
        WRdata.push_back(deltaTDownstreamVertical);
       
        //TODO Other WR results, dummy data for now
        uint32_t deltaTUpstreamHorizontal = 0;
        WRdata.push_back(deltaTUpstreamHorizontal);
        uint32_t deltaTUpstreamVertical = 0;
        WRdata.push_back(deltaTUpstreamVertical);
        
        uint32_t deltaTLowPressure = 65535;
        if(foundLowPressure){
          if(finallowpressurepairs.at(event_i) >= 0){
            deltaTLowPressure = (uint32_t)std::clamp(std::abs((int)finaltscloud.points.at(event_i)-(int)finallowpressurecloud.points.at(finallowpressurepairs.at(event_i))),0,65535);
          }
        }
        WRdata.push_back(deltaTLowPressure);
        
        uint32_t deltaTHighPressure = 65535;
        if(foundHighPressure){
          if(finalhighpressurepairs.at(event_i) >= 0){
            deltaTHighPressure = (uint32_t)std::clamp(std::abs((int)finaltscloud.points.at(event_i)-(int)finalhighpressurecloud.points.at(finalhighpressurepairs.at(event_i))),0,65535);
          }
        }
        WRdata.push_back(deltaTHighPressure);

        std::string filenamewr = "WRRaw";
        utility::TimestampCloud realtime(WRSpills.at(finalwrspillindex), filenamewr);
        uint64_t deltaTSpillStart = 0;
        if(found){
          if(finalwrpairs.at(event_i) >= 0){
            deltaTSpillStart = finalwrcloud.points.at(event_i)-startOfSpill;
          }
        }
  

        uint32_t deltaTSpillStartms = (deltaTSpillStart >> 32) & 0xFFFFFFFF; //Split uint64_t into two halves
        uint32_t deltaTSpillStartls = deltaTSpillStart & 0xFFFFFFFF;
        WRdata.push_back(deltaTSpillStartms);
        WRdata.push_back(deltaTSpillStartls);

        uint32_t spillNumber = (uint32_t) TSSpillNumber;
        WRdata.push_back(spillNumber);
       
        //Add WR data to event
        output_event.add("WRResults", WRdata);

        std::vector<uint8_t> fakeData{0}; //Empty data for when the FT is unaligned

        //Add fiber tracker events
        if(foundFT50){
          if(finalft50pairs.at(event_i) >= 0){
            output_event.add("FiberTrackerFT50", FT50Spills.at(finalft50spillindex).at(finalft50pairs.at(event_i)).data());
          }
          else{
            output_event.add("FiberTrackerFT50", fakeData);
          }
        }
        else{
          output_event.add("FiberTrackerFT50", fakeData);
        }
        if(foundFT51){
          if(finalft51pairs.at(event_i) >= 0){
            output_event.add("FiberTrackerFT51", FT51Spills.at(finalft51spillindex).at(finalft51pairs.at(event_i)).data());
          }
          else{
            output_event.add("FiberTrackerFT51", fakeData);
          }
        }
        else{
          output_event.add("FiberTrackerFT51", fakeData);
        }

        output_event.add("QIEstreamUp", TSSpills.at(TSSpillNumber).at(event_i).data());
        //output_event.add("FiberTrackerFT50", fakeData);
        //f(pairsft50.at(event_i) != -1){
        //  output_event.add("FT50Raw", pairsft50
        //}

        output_file.nextEvent(true);
      }
    }
    /*
    if(!foundFT50){
      reformat_log(fatal) << "TS Spill Number: " << TSSpillNumber<<", TS to FT50 efficiency: " << 0.0 << "\n";

      if(false){ //If FT50 alignment required
        continue; //Skip spill
      }
    }
    if(!foundFT51){
      reformat_log(fatal) << "TS Spill Number: " << TSSpillNumber<<", TS to FT51 efficiency: " << 0.0 << "\n";
      
      if(false){ //If FT51 alignment required
        continue; //Skip spill
      }
    }
    */
    //Add empty beam instrumentation data for the whole spill, only TS data remaining
    if(!found){
      reformat_log(fatal) << "Could not align TS spill" << "\n";
      reformat_log(fatal) << "ALL SUBSYSTEMS " << 0.0 << std::endl;

      if(false){ //If WR alignment required
        continue; //Skip spill
      }

      //Add empty events
    }
  }
  
  
  run_header.setRunEnd(std::time(nullptr));
  output_file.close();
  reformat_log(info) << "Conversion complete.";
  
}

}  // namespace reformat
