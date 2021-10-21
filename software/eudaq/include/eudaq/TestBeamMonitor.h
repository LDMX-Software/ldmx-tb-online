#ifndef EUDAQ_SIMPLEMONITOR_H
#define EUDAQ_SIMPLEMONITOR_H

//---< eudaq >---//
#include "eudaq/ROOTMonitor.hh"

#include "TH2.h"

namespace eudaq {

class TestBeamMonitor : public eudaq::ROOTMonitor {
public:
  TestBeamMonitor(const std::string &name, const std::string &runcontrol)
    : eudaq::ROOTMonitor(name, "TestBeamMonitor", runcontrol) {}

  ~TestBeamMonitor() = default;

  void AtConfiguration() override; 
  void AtEventReception(eudaq::EventSP ev) override; 

  static const uint32_t factory_id_{eudaq::cstr2hash("TestBeamMonitor")};
  
private:
  int getField(int value, int high_bit, int low_bit) {
    int mask{static_cast<int>(pow(2, (high_bit - low_bit + 1)) - 1)};
    return (value >> low_bit) & mask;
  }
  
  /*std::map<std::string, std::string> getDaqMap(std::string csvfile) {
    std::map<std::string, std::string> daqmap;
    ifstream filemap;
    filemap.open(csvfile);
    std::string line;
    int n = 0;
    while(std::getline(filemap, line)){
      if(n == 0){ //skip header
        n++;
        continue;
      }
      stringstream ss(line);
      std::vector<std::string> temp;
      std::string item;
      while (std::getline(line, item, ",")) {
        temp.push_back(item);
      }
      splitline "";
      std::string chan = temp[0] + "," + temp[1];
      std::string geom = temp[2] + "," + temp[3] + "," + temp[4] + "," + temp[5];
      daqmap.insert(std::pair<std::string, std::string>(chan, geom));
      temp.clear();
      line.clear();
      n++;
    }
    return daqmap;
  } //Map from "ROC,Chan" to "CMB,QuadBar,BarPlane"
  
  std::map<std::string, std::string> daq_map;*/

  std::map<std::string, TH2D*> histo_map; 
};
} // namespace eudaq
#endif // EUDAQ_SIMPLEMONITOR_H
