
#include "Reformat/Converter.h"

#include <ctime>
#include <queue>

#include "Framework/EventFile.h"
#include "Framework/Event.h"
#include "Framework/EventHeader.h"
#include "Framework/RunHeader.h"
#include "TTimeStamp.h"

namespace reformat {

Converter::~Converter() {
  framework::logging::close();
}

void Converter::configure(const framework::config::Parameters& cfg) {
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
  max_diff_ = cfg.getParameter<int>("max_diff");
  keep_all_ = cfg.getParameter<bool>("keep_all");
  event_limit_ = cfg.getParameter<int>("event_limit");

  reformat_log(info) << "Converter parameters: {\n\t"
    << "pass : " << pass_ << ",\n\t"
    << "output_filename : " << output_filename_ << ",\n\t"
    << "run : " << run_ << ",\n\t"
    << "start_event : " << start_event_ << ",\n\t"
    << "max_diff : " << max_diff_ << ",\n\t"
    << "event_limit : " << event_limit_ << ",\n\t"
    << "keep_all : " << keep_all_ << "\n}";

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
  std::set<std::string> names;
  for (const auto& f : input_files_) {
    if (names.find(f->name()) == names.end()) {
      names.insert(f->name());
    } else {
      EXCEPTION_RAISE("NameRepeat",
          "Two input files have the same name.");
    }
  }
  reformat_log(info) << "Configuration complete.";
}

void Converter::convert() {
  reformat_log(info) << "Beginning conversion";
  framework::Event output_event(pass_);
  framework::EventFile output_file({}, output_filename_, 
      nullptr, true, true, false);
  output_file.setupEvent(&output_event);

  ldmx::RunHeader run_header(run_);
  run_header.setRunStart(std::time(nullptr));
  output_file.writeRunHeader(run_header);

  reformat_log(debug) << "Initialized Event, EventFile, and RunHeader";

  // initialize empty queues for all input files
  std::map<std::string,std::queue<EventPacket>> event_queue;
  for (const auto& f : input_files_) {
    event_queue[f->name()];
    reformat_log(debug) << "Inialized event queue for " << f->name();
  }

  int i_event{start_event_};
  while (not input_files_.empty()) {
    // go through input files, removing them if
    // they are done
    auto f_it = input_files_.begin();
    while (f_it != input_files_.end()) {
      // pop the next event packet from this input file
      //  if it is defined, then put the data into that file's queue:w
      auto ep = (*f_it)->next();
      if (ep) {
        event_queue[(*f_it)->name()].push(ep.value());
        reformat_log(debug) << (*f_it)->name() << " popped " << ep.value().data().size() 
          << " bytes from timestamp " << ep.value().timestamp();
        ++f_it;
      } else {
        // file says no more events
        reformat_log(info) << (*f_it)->name() << " is done and removed from files on event " << i_event;
        f_it = input_files_.erase(f_it);
      }
    } // loop over input files

    // get earliest timestamp from all files
    EventPacket::TimestampType earliest_ts{EventPacket::TimestampMax};
    for (const auto& [name, q] : event_queue) {
      if (q.size() == 0) continue;
      if (q.front().timestamp() < earliest_ts) earliest_ts = q.front().timestamp();
    }
    reformat_log(debug) << "Earliest timestamp " << earliest_ts;

    // pop events that are within X of earliest ts
    std::map<std::string,EventPacket> aligned_event;
    for (auto& [name,q] : event_queue) {
      if (q.size() == 0) continue;
      if (q.front().timestamp() - earliest_ts < max_diff_) {
        reformat_log(debug) << name << " aligned at " << q.front().timestamp() << " queue size " << q.size();
        aligned_event[name] = q.front();
        q.pop();
      }
    }

    // if we are keeping all events or if all files contributed an event packet
    // put the popped event packets into the event bus and save this event
    if (aligned_event.size() > 0 and keep_all_ or aligned_event.size() == event_queue.size()) {
      reformat_log(info) << "keeping event " << i_event << " with " 
        << aligned_event.size() << "/" << event_queue.size() << " files at "
        << earliest_ts;
      // initialize event header for input files
      ldmx::EventHeader& eh = output_event.getEventHeader();
      eh.setRun(run_);
      eh.setEventNumber(i_event++);
      eh.setTimestamp(TTimeStamp()); // related to earliest_ts ???
      eh.setRealData(true); // probably smart...
      for (auto& [name,ep] : aligned_event) {
        output_event.add(name, ep.data());
      }
      // go to next event in output file.
      output_file.nextEvent(true);
    } else if (not keep_all_) {
      reformat_log(debug) << "skipping partial event";
    }

    if (event_limit_ > 0 and i_event - start_event_ > event_limit_) {
      reformat_log(info) << "exiting early at event " << i_event;
      break;
    }
  }   // loop until no more input files

  run_header.setRunEnd(std::time(nullptr));
  output_file.close();
  reformat_log(info) << "Conversion complete.";
}

}  // namespace reformat
