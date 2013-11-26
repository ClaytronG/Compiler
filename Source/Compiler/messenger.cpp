// Author: Clayton Green (kgreen1@unbc.ca)

// Visual Studio complains about using fopen() because it is deprecated and 
// suggests using fopen_s() but G++ only has fopen().
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <fstream>
#include <iostream>

#include "messenger.h"

Messenger::Messenger() : show_trace_(false), output_file_(stdout), 
    error_list_() {}

Messenger::Messenger(const std::string &filename) 
  : show_trace_(false), error_list_() {
    output_file_ = fopen(filename.c_str(), "w");
}

void Messenger::AddError(const std::string &filename, const int line_number, 
                         const std::string &message) {
  auto pair = std::make_pair(line_number, message);
  auto error = std::make_pair(filename, pair);
  error_list_.push_back(error);
}

void Messenger::PrintErrors() const {
  if (!error_list_.empty()) {
    fprintf(output_file_, "\nErrors:\n");
    for (auto error = error_list_.begin(); error != error_list_.end(); 
         ++error) {
      fprintf(output_file_, "%s(%d): %s\n", error->first.c_str(), 
              error->second.first, error->second.second.c_str());
    }
  }
}

void Messenger::PrintMessage(const std::string &message) const {
  if (show_trace_) {
    fprintf(output_file_, "%s", message.c_str());
  }
}
