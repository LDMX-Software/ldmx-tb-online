
#include "Reformat/Converter.h"

#include <ctime>
#include <queue>

#include "Framework/EventFile.h"
#include "Framework/Event.h"
#include "Framework/EventHeader.h"
#include "Framework/RunHeader.h"
#include "TTimeStamp.h"

namespace reformat {

void Converter::configure(const framework::config::Parameters& cfg) {
  pass_ = cfg.getParameter<std::string>("pass_name");  // pass is a keyword in Python
  output_filename_ = cfg.getParameter<std::string>("output_filename");
  run_ = cfg.getParameter<int>("run");
  start_event_ = cfg.getParameter<int>("start_event");

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
}

void Converter::convert() {
  framework::Event output_event(pass_);
  framework::EventFile output_file({}, output_filename_, 
      nullptr, true, true, false);
  output_file.setupEvent(&output_event);

  ldmx::RunHeader run_header(run_);
  run_header.setRunStart(std::time(nullptr));
  output_file.writeRunHeader(run_header);

  std::map<std::string,std::queue<EventPacket>> event_queue;

  int i_event{start_event_};
  while (not input_files_.empty()) {
    // initialize event header for input files
    ldmx::EventHeader& eh = output_event.getEventHeader();
    eh.setRun(run_);
    eh.setEventNumber(i_event++);
    eh.setTimestamp(TTimeStamp());

    // go through input files, removing them if
    // they are done
    auto f_it = input_files_.begin();
    while (f_it != input_files_.end()) {
      auto ep = (*f_it)->next();
      if (ep) {
        event_queue[(*f_it)->name()].push(ep.value());
        ++f_it;
      } else {
        // file says no more events
        f_it = input_files_.erase(f_it);
      }

      // get earliest timestamp
      uint32_t earliest_ts{0xffffffff};
      for (const auto& [name, q] : event_queue) {
        if (q.size() == 0) continue;
        if (q.front().timestamp < earliest_ts) earliest_ts = q.front().timestamp;
      }

      // pop events that are within X of earliest ts
      std::map<std::string,EventPacket> aligned_event;
      for (auto& [name,q] : event_queue) {
        if (q.size() == 0) continue;
        if (q.front().timestamp - earliest_ts < 100 /*max diff to allow alignment*/) {
          aligned_event[name] = q.front();
          q.pop();
        }
      }

      if (true /*keep_all_*/ or aligned_event.size() == event_queue.size()) {
        for (auto& [name,ep] : aligned_event) {
          output_event.add(name, ep.data);
        }
        // go to next event in output file.
        output_file.nextEvent(true);
      }
    }

  }

  run_header.setRunEnd(std::time(nullptr));
  output_file.close();
}

}  // namespace reformat
