#ifndef EUDAQ_TESTBEAMEVENTDISPLAYMONITOR_H
#define EUDAQ_TESTBEAMEVENTDISPLAYMONITOR_H

//---< eudaq >---//
#include "eudaq/ROOTMonitor.hh"

#include "TH2.h"

namespace eudaq {

class TestBeamEventDisplayMonitor : public eudaq::ROOTMonitor {
public:
  TestBeamEventDisplayMonitor(const std::string &name, const std::string &runcontrol)
    : eudaq::ROOTMonitor(name, "TestBeamEventDisplayMonitor", runcontrol) {}

  ~TestBeamEventDisplayMonitor() = default;

  void AtConfiguration() override; 
  void AtEventReception(eudaq::EventSP ev) override; 

  static const uint32_t factory_id_{eudaq::cstr2hash("TestBeamEventDisplayMonitor")};
  
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
  std::map<int, TH2D*> hcal_event_map;
  TH2D* ts_event;
  int nPlanes;
  double time_reset; //seconds
  int n;
};
} // namespace eudaq
#endif // EUDAQ_TESTBEAMEVENTDISPLAYMONITOR_H
