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

  static std::map<std::string, int> buildMap(std::string csv, std::string column)
  {
      //using namespace std;
      //using namespace boost;
      //string data("/Users/matthewsolt/ldmx/ldmx-sw/Hcal/data/testbeam_connections.csv");

      std::ifstream in(csv.c_str());
      //if (!in.is_open()) return 1;

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
        //std::cout<<vec[hgcroc_index]<<"  "<<vec[channel_index]<<"  "<<vec[index]<<std::endl;
          std::string key = vec[hgcroc_index] + "," + vec[channel_index];
          std::cout<<column+"  "<<index<<" "<<vec[index]<<std::endl;
          m.insert(std::pair<std::string, int>(key, std::stoi(vec[index])));
        // vector now contains strings from one row, output to cout here
        //copy(vec.begin(), vec.end(), ostream_iterator<string>(cout, "|"));

        //cout << "\n----------------------" << endl;
        //cout << vec[0] << vec[1] << vec[2] << vec[3] << vec[4] << vec[5] << vec[6] << endl;
      }
      return m;
  }

  static std::map<std::string, int> getCMBMap(std::string csv){
    return buildMap(csv, "CMB");
  }

  static std::map<std::string, int> getQuadbarMap(std::string csv){
    return buildMap(csv, "Quadbar");
  }

  static std::map<std::string, int> getBarMap(std::string csv){
    return buildMap(csv, "Bar");
  }

  static std::map<std::string, int> getPlaneMap(std::string csv){
    return buildMap(csv, "Plane");
  }

private:

/*int main(){

  std::string csvfile = "/Users/matthewsolt/ldmx/ldmx-sw/Hcal/data/testbeam_connections.csv";
  std::map<std::string, int> cmb_map = getCMBMap(csvfile);
  std::map<std::string, int> quadbar_map = getQuadbarMap(csvfile);
  std::map<std::string, int> bar_map = getBarMap(csvfile);
  std::map<std::string, int> plane_map = getPlaneMap(csvfile);
  std::map<std::string, int> m = plane_map;
  std::map<std::string, int>::iterator it;
  for (it = m.begin(); it != m.end(); it++)
{
    std::cout << it->first    // string (key)
              << ':'
              << it->second   // string's value
              << std::endl;
}
}*/

};
} // namespace eudaq

#endif // EUDAQ_CSVPARSER_H
