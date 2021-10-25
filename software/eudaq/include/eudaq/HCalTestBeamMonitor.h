#ifndef EUDAQ_HCALTESTBEAMMONITOR_H
#define EUDAQ_HCALTESTBEAMMONITOR_H

//---< eudaq >---//
#include "eudaq/ROOTMonitor.hh"

#include "TH2.h"

namespace eudaq {

class HCalTestBeamMonitor : public eudaq::ROOTMonitor {
public:
  HCalTestBeamMonitor(const std::string &name, const std::string &runcontrol)
    : eudaq::ROOTMonitor(name, "HCalTestBeamMonitor", runcontrol) {}

  ~HCalTestBeamMonitor() = default;

  void AtConfiguration() override; 
  void AtEventReception(eudaq::EventSP ev) override; 

  static const uint32_t factory_id_{eudaq::cstr2hash("HCalTestBeamMonitor")};
  
private:
  int getField(int value, int high_bit, int low_bit) {
    int mask{static_cast<int>(pow(2, (high_bit - low_bit + 1)) - 1)};
    return (value >> low_bit) & mask;
  }
  
    std::map<std::string, std::string> getDaqMap(std::string csvfile) {
    std::map<std::string, std::string> daqmap;
    std::ifstream filemap;
    filemap.open(csvfile);
    std::string line;
    int n = 0;
    while(std::getline(filemap, line)){
      if(n == 0){ //skip header
        n++;
        continue;
      }
      std::stringstream ss(line);
      std::vector<std::string> temp;
      std::string item;
      while (std::getline(ss, item, ',')) {
        temp.push_back(item);
      }
      std::string chan = temp[0] + "," + temp[1];
      std::string geom = temp[2] + "," + temp[3] + "," + temp[4] + "," + temp[5];
      daqmap.insert(std::pair<std::string, std::string>(chan, geom));
      temp.clear();
      line.clear();
      n++;
    }
    return daqmap;
  } //Map from "ROC,Chan" to "CMB,QuadBar,BarPlane"
  
  std::map<std::string, std::string> getPedMap(std::string csvfile) {
    std::map<std::string, std::string> pedmap;
    std::ifstream filemap;
    filemap.open(csvfile);
    std::string line;
    int n = 0;
    while(std::getline(filemap, line)){
      if(n <= 1){ //skip header
        n++;
        continue;
      }
      std::stringstream ss(line);
      std::vector<std::string> temp;
      std::string item;
      while (std::getline(ss, item, ',')) {
        temp.push_back(item);
      }
      std::string chan = temp[0];
      std::string thresh = temp[2] + "," + temp[3];
      pedmap.insert(std::pair<std::string, std::string>(chan, thresh));
      temp.clear();
      line.clear();
      n++;
    }
    return pedmap;
  } 
  
  int getCMB(std::string str){
     std::stringstream ss(str);
     std::vector<std::string> temp;
     std::string item;
     while (std::getline(ss, item, ',')) {
       temp.push_back(item);
     }
    return std::stoi(temp[0]);
  }

  int getQuadBar(std::string str){
     std::stringstream ss(str);
     std::vector<std::string> temp;
     std::string item;
     while (std::getline(ss, item, ',')) {
       temp.push_back(item);
     }
     return std::stoi(temp[1]);
  }
  
  int getBar(std::string str){
     std::stringstream ss(str);
     std::vector<std::string> temp;
     std::string item;
     while (std::getline(ss, item, ',')) {
       temp.push_back(item);
     }
     return std::stoi(temp[2]);
  }
  
  int getPlane(std::string str){
     std::stringstream ss(str);
     std::vector<std::string> temp;
     std::string item;
     while (std::getline(ss, item, ',')) {
       temp.push_back(item);
     }
     return std::stoi(temp[3]);
  }
  
  int getPedest(std::string str){
     std::stringstream ss(str);
     std::vector<std::string> temp;
     std::string item;
     while (std::getline(ss, item, ',')) {
       temp.push_back(item);
     }
     return std::stoi(temp[0]);
  }
  
  int getThresh(std::string str){
     std::stringstream ss(str);
     std::vector<std::string> temp;
     std::string item;
     while (std::getline(ss, item, ',')) {
       temp.push_back(item);
     }
     return std::stoi(temp[1]);
  }
  
  std::map<std::string, std::string> daq_map;
  std::map<std::string, std::string> ped_map;
  std::map<int, TH2D*> hcal_run_map; 
  int nPlanes;
  std::map<std::string, TH2D*> histo_map; 
};
} // namespace eudaq
#endif // EUDAQ_HCALTESTBEAMMONITOR_H
