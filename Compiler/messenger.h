// Author: Clayton Green (kgreen1@unbc.ca)
//
//

#ifndef KOMPILER_MESSENGER_H_
#define KOMPILER_MESSENGER_H_

#include <list>
#include <string>

//
class Messenger {
 public:
	typedef std::list<std::pair<std::string, 
                              std::pair<int, std::string> > > ErrorList;

  // Construct a messenger object that prints to STDOUT by default.
  Messenger();
  
  // Construct a messenger object with a corresponding output file.
  Messenger(const std::string &filename);

  // When set to true, trace messages will be printed.
  void set_show_trace(const bool show_trace) {
    show_trace_ = show_trace;
  }

  // Adds an error message and corresponding filename and line number to be 
  // printed at a later time.
  void AddError(const std::string &filename, const int line_number, 
                const std::string &message);

  // Prints the list of recorded errors, if any.
  void PrintErrors() const;

  // If trace is enabled, it prints the message to the current stream.
  void PrintMessage(const std::string &message) const;

 private:
  // A list of error messages with corresponding filenames and line numbers.
	ErrorList error_list_;

  // Where to output trace and error messages. STDOUT by default.
  FILE* output_file_;

  // If true, PrintMessage() will display output.
  bool show_trace_;

  //DISALLOW_COPY_AND_ASSIGN(Messenger);
};

#endif	// KOMPILER_MESSENGER_H_
