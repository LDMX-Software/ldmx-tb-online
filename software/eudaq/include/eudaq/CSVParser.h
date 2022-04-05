#ifndef EUDAQ_CSVPARSER_H
#define EUDAQ_CSVPARSER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <boost/tokenizer.hpp>
#include <map>


namespace eudaq {

class CSVParser {
public:

  static std::map<std::string, int> buildDaqMap(std::string csv, std::string column)
  {   
      std::ifstream in(csv.c_str());
      std::cout<<csv.c_str()<<std::endl;

      typedef boost::tokenizer< boost::escaped_list_separator<char> > Tokenizer;
      std::vector< std::string > vec;
      std::string line;

      int index = 9999;
      int hgcroc_index = 9999;
      int channel_index = 9999;
      bool firstrow = true;
      std::map<std::string, int> m;

      while (getline(in,line))
      {
          Tokenizer tok(line);
          vec.assign(tok.begin(),tok.end());
          if(firstrow){
            for(int j = 0; j < vec.size(); j++){
              if(vec[j] == column){
                index = j;
              }
              if(vec[j] == "HGCROC"){
                hgcroc_index = j;
              }
              if(vec[j] == "Channel"){
                channel_index = j;
              }
            }
            firstrow = false;
            continue;
          }
          if((index > vec.size() - 1) || (hgcroc_index > vec.size() - 1) || (channel_index > vec.size() - 1)){
            std::cout<<"Index is larger than length of row."<<std::endl;
            std::cout<<"Most likely the selected column name does not exist."<<std::endl;
          }
          std::string key = vec[hgcroc_index] + ":" + vec[channel_index];
          m.insert(std::pair<std::string, int>(key, std::stoi(vec[index])));
      }
      return m;
  }

  static std::map<std::string, std::string> buildThresholdMap(std::string csv, std::string column)
  {
      std::ifstream in(csv.c_str());

      typedef boost::tokenizer< boost::escaped_list_separator<char> > Tokenizer;
      std::vector< std::string > vec;
      std::string line;

      int index = 9999;
      int elloc_index = 9999;
      bool firstrow = true;
      bool secondrow = true;
      std::map<std::string, std::string> m;

      while (getline(in,line))
      {
        if(firstrow){
          firstrow = false;
          continue;
        }
        Tokenizer tok(line);
        vec.assign(tok.begin(),tok.end());
        if(secondrow){
          for(int j = 0; j < vec.size(); j++){
            if(vec[j] == column){
              index = j;
            }
            if(vec[j] == "ElLoc"){
              elloc_index = j;
            }
          }
          secondrow = false;
          continue;
        }
        if(column == "TOT_GAIN"){ //This is a hack becuase I can't figure out whitespace issues!
          index = 5;
        }
        if((index > vec.size() - 1) || (elloc_index > vec.size() - 1)){
          std::cout<<"Index is larger than length of row."<<std::endl;
          std::cout<<"Most likely the selected column name does not exist."<<std::endl;
      }
      std::string key = vec[elloc_index];
      m.insert(std::pair<std::string, std::string>(key, vec[index]));
    }
    return m;
}

  static std::map<std::string, int> getCMBMap(std::string csv){
    return buildDaqMap(csv, "CMB");
  }

  static std::map<std::string, int> getQuadbarMap(std::string csv){
    return buildDaqMap(csv, "Quadbar");
  }

  static std::map<std::string, int> getBarMap(std::string csv){
    return buildDaqMap(csv, "Bar");
  }

  static std::map<std::string, int> getPlaneMap(std::string csv){
    return buildDaqMap(csv, "Plane");
  }

  static std::map<std::string, int> getDetIDMap(std::string csv){
    std::map<std::string, std::string> m = buildThresholdMap(csv, "DetID");
    std::map<std::string, std::string>::iterator it;
    std::map<std::string, int> outmap;
    for (it = m.begin(); it != m.end(); it++){
      outmap.insert(std::pair<std::string, int>(it->first, std::stoi(it->second)));
    }
    return outmap;
  }

  static std::map<std::string, double> getADCPedMap(std::string csv){
    std::map<std::string, std::string> m = buildThresholdMap(csv, "ADC_PEDESTAL");
    std::map<std::string, std::string>::iterator it;
    std::map<std::string, double> outmap;
    for (it = m.begin(); it != m.end(); it++){
      outmap.insert(std::pair<std::string, double>(it->first, std::stod(it->second)));
    }
    return outmap;
  }

  static std::map<std::string, double> getADCGainMap(std::string csv){
    std::map<std::string, std::string> m = buildThresholdMap(csv, "ADC_GAIN");
    std::map<std::string, std::string>::iterator it;
    std::map<std::string, double> outmap;
    for (it = m.begin(); it != m.end(); it++){
      outmap.insert(std::pair<std::string, double>(it->first, std::stod(it->second)));
    }
    return outmap;
  }

  static std::map<std::string, double> getTOTPedMap(std::string csv){
    std::map<std::string, std::string> m = buildThresholdMap(csv, "TOT_PEDESTAL");
    std::map<std::string, std::string>::iterator it;
    std::map<std::string, double> outmap;
    for (it = m.begin(); it != m.end(); it++){
      outmap.insert(std::pair<std::string, double>(it->first, std::stod(it->second)));
    }
    return outmap;
  }

  static std::map<std::string, double> getTOTGainMap(std::string csv){
    std::map<std::string, std::string> m = buildThresholdMap(csv, "TOT_GAIN");
    std::map<std::string, std::string>::iterator it;
    std::map<std::string, double> outmap;
    for (it = m.begin(); it != m.end(); it++){
      outmap.insert(std::pair<std::string, double>(it->first, std::stod(it->second)));
    }
    return outmap;
  }

private:

};
} // namespace eudaq

#endif // EUDAQ_CSVPARSER_H
