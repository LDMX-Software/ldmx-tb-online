#ifndef REFORMAT_UTILITY_TIMESTAMPCLOUD_H_
#define REFORMAT_UTILITY_TIMESTAMPCLOUD_H_

#include <vector>
#include <queue>
#include <string>
#include <algorithm>

#include "Reformat/EventPacket.h"

namespace reformat::utility {

/**
 * Structure holding a one-dimensional point "cloud" of timestamps from a whole spill.
 */
struct TimestampCloud {

    TimestampCloud(std::vector<EventPacket> packets, std::string fileName){
        if(fileName == "TSGrouped"){
          for(auto iter = packets.begin(); iter != packets.end(); iter++){
              uint64_t timestamp = iter->timestamp();
              //ns conversion
              points.push_back( timestamp & 0xffffffff) ;
          }

        }
        else{
          for(auto iter = packets.begin(); iter != packets.end(); iter++){
              uint64_t timestamp = iter->timestamp();
              //ns conversion
              points.push_back( ((timestamp & 0xffffffff00000000) >> 32)*1e9 + (timestamp & 0xffffffff)*8);
          }
        }

        fileName_ = fileName;

    }
    TimestampCloud(){};

    /*
     * Point cloud
     */
    //In ns, probably since epoch but not strictly necessary
    //If the nearest neighbor searches turn out to be too slow, make this a k-d tree!
    std::vector<uint64_t> points;

    //TODO
    //Nearest neighbor search w.r.t. another cloud, to pair events
    std::vector<int> nnpair(TimestampCloud& c){
      uint64_t tolerance = 400;
      
      //Very naive matching algorithm
      double matches = 0;
      double misses = 0;

      std::vector<int> pairs;

      /*
      uint64_t min = *std::min_element(points.begin(), points.end());
      uint64_t max = *std::max_element(points.begin(), points.end());
      uint64_t minc = *std::min_element(c.points.begin(), c.points.end());
      uint64_t maxc = *std::max_element(c.points.begin(), c.points.end()); 

      int t1start = 0;
      for(int i = points.size()-1; i >= 0; i--){
        if (points.at(i) > minc - tolerance){
          t1start = i;
        }
      }
      int t1stop = 0;
      for(int i = 0; i < points.size(); i++){
        if(points.at(i) < maxc + tolerance){
          t1stop = i;
        }
      }
      */

      for(int t1 = 0; t1 != points.size(); t1++){
      //for(int t1 = t1start; t1 != t1stop; t1++){
        int index = -1;
        bool match = false;
        bool duplicate = false;
        //for(auto t2 = c.points.begin(); t2 != c.points.end(); t2++){
        for(int t2 = 0; t2 != c.points.size(); t2++){
            if((points.at(t1)-c.points.at(t2)) < tolerance){
                if(match){
                  duplicate = true;
                }
                match = true;

                index = t2;
            }
        }
        if(!duplicate){
          pairs.push_back(index);
        }
        else{
          pairs.push_back(-1);
        }
      }
        

        return pairs;
    }

    int getInterval(uint64_t deltaT){
      int index = 0;
      uint64_t t0 = points.at(0);
      for(int i = 0; i < points.size(); i++){
        if(points.at(i) < t0 + deltaT){
          index = i;
        }
      }
      return index;
    }

    //TODO
    //Quick comparison between spills, to see if the two clouds are worth trying to match

    //TODO
    //There are many more sofisticated algorithms for "point set registration" possible, if that will be needed
    //Iterative closest point between two clouds

    //TODO
    //Handle transformations properly
    void scale(long double factor){
      for(auto iter = points.begin(); iter != points.end(); iter++){
        *iter = (uint64_t)(((long double)*iter)*factor);
      }
    };

    //Translate by t ns
    void translate(uint64_t t) {
      for(auto iter = points.begin(); iter != points.end(); iter++){
        *iter += t;
      }
    };

private:
    std::string fileName_;

}; // TimestampCloud

} // namespace reformat::utility

#endif // REFORMAT_UTILITY_TIMESTAMPCLOUD_H_
