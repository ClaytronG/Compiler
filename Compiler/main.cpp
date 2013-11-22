// Clayton Green (kgreen1@unbc.ca)
// Last Updated: October 12, 2013

// A macro to disallow the copy constructor and operator= functions.
// This should be used in the private: Declarations for a class.
//#define DISALLOW_COPY_AND_ASSIGN(TypeName)  \
 // TypeName(const TypeName&);                \
 // void operator=(const TypeName&)

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <tclap/CmdLine.h>

#include "administrator.h"

int main(int argc, char* argv[]) {
	try {
    TCLAP::CmdLine cmd("Command description message", ' ', "0.2");
    
    TCLAP::SwitchArg lex_switch("l", "lex", "Process up to the Lexer phase");
    TCLAP::SwitchArg parse_switch("p", "parse", 
                                  "Process up to the Parser phase");
    TCLAP::SwitchArg sem_switch("s", "sem", 
                                "Process up to the Semantic Analyser phase");
    TCLAP::SwitchArg tup_switch("t", "tup", 
                                "Process up to the Tuple phase");
    TCLAP::SwitchArg compile_switch("c", "compile", 
                                    "Process all phases and compile (default)",
                                    true);
    TCLAP::SwitchArg quiet_switch("q", "quiet", 
                                  "Only display error messages (default)", true);
    TCLAP::SwitchArg verbose_switch("v", "verbose", 
                                    "Display all Trace messages");
    TCLAP::ValueArg< std::string > out_arg("o", "out", 
                                           "Output file (default -out=STDOUT)", 
                                           false, "out", "string");
    TCLAP::ValueArg< std::string > err_arg("e", "err", 
                                           "Error file (default -err=STDOUT)", 
                                           false, "err", "string");
    TCLAP::UnlabeledMultiArg<std::string> input_args("input", "Input files", true, "string");
    
    std::vector<TCLAP::Arg*> args;
    args.push_back(&lex_switch);
    args.push_back(&parse_switch);
    args.push_back(&sem_switch);
    args.push_back(&tup_switch);
    args.push_back(&compile_switch);
    cmd.xorAdd(args);
    cmd.add(quiet_switch);
    cmd.add(verbose_switch);
    cmd.add(out_arg);
    cmd.add(err_arg);
    cmd.add(input_args);
    
    cmd.parse(argc, argv);
    
    std::vector< std::string > inputFiles = input_args.getValue();
    if (inputFiles.size() == 0) {
      fprintf(stderr, "No input files. Please specify 1 or more .cs13 files using -I <file>\n");
      return -1;
    }

    Administrator *admin;
    std::string err = err_arg.getValue();
    if (err.compare("err") != 0) {  
      // Have to create the error file to be able to write to it
      std::ofstream filestream(err_arg.getValue());
      if (!filestream) {
        fprintf(stderr, "Error creating %s\n", err_arg.getValue().c_str());
        return -1;
      }
      filestream.clear();
      filestream.close();
      admin = new Administrator(inputFiles, err_arg.getValue());
    } else {
      admin = new Administrator(inputFiles);
    }
    std::string out = out_arg.getValue();
    if (out.compare("out") != 0) {
      admin->set_output_file(out_arg.getValue());
    }

    if (verbose_switch.isSet()) {
      admin->ShowTrace(true);
    }
    
    if (lex_switch.isSet()) {
      admin->LexerPhase();
    } else if (parse_switch.isSet()) {
      admin->ParserPhase();
    } else if (sem_switch.getValue()) {
		  admin->SemanticAnalysisPhase();
    } else if (tup_switch.isSet()) {
      admin->TupleGenerationPhase();
    } else {
      // TODO: complete compilation
      return -1;
    }    
	} catch(TCLAP::ArgException &e) {
    fprintf(stderr, "error: %s for arg %s\n", e.error().c_str(), e.argId().c_str());
	}

  return 0;
}
