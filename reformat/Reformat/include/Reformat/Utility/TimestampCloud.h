#ifndef REFORMAT_UTILITY_TIMESTAMPCLOUD_H_
#define REFORMAT_UTILITY_TIMESTAMPCLOUD_H_

#include <vector>
#include <queue>
#include <string>

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
      //Very naive matching algorithm
      double matches = 0;
      double misses = 0;

      std::vector<int> pairs;

      for(int t1 =0; t1 != points.size(); t1++){
        int index = -1;
        bool match = false;
        bool duplicate = false;
        //for(auto t2 = c.points.begin(); t2 != c.points.end(); t2++){
        for(int t2 = 0; t2 != c.points.size(); t2++){
            if((points.at(t1)-c.points.at(t2)) < 500){
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
