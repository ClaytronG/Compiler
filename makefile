CC=g++
CFLAGS=-c -std=c++0x -I ./Source/tclap-1.2.1/include
LDFLAGS=
SRCDIR=./Source/Compiler
OUTDIR=./Executables
_SOURCES=administrator.cpp assignment_node.cpp ast_node.cpp ast_node_full_visitor.cpp ast_node_init_visitor.cpp ast_node_print_visitor.cpp binary_node.cpp branch_node.cpp call_node.cpp case_node.cpp compound_node.cpp continue_node.cpp declaration_node.cpp exit_node.cpp expression_node.cpp function_declaration_node.cpp if_node.cpp literal_node.cpp loop_node.cpp main.cpp messenger.cpp null_node.cpp parameter_node.cpp parser.cpp program_node.cpp return_node.cpp scanner.cpp semantic_analyzer.cpp statement_node.cpp token.cpp unary_node.cpp variable_declaration_node.cpp variable_node.cpp code_generator.cpp ast_node_code_visitor.cpp
SOURCES=$(addprefix $(SRCDIR)/, $(_SOURCES))
_OBJECTS=$(_SOURCES:.cpp=.o)
OBJECTS=$(addprefix $(SRCDIR)/, $(_OBJECTS))
_EXECUTABLE=compiler
EXECUTABLE=$(addprefix $(OUTDIR)/, $(_EXECUTABLE))

all: $(SOURCES) $(EXECUTABLE)
	
sources:
	$(SOURCES)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
  
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(addprefix $(SRCDIR)/,*o)
