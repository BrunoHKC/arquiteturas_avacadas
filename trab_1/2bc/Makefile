CC = gcc
CPP = g++
LD = g++
CFLAGS = $(FLAGS)
CPPFLAGS = $(FLAGS)
BIN_NAME = orcs
RM = rm -f

FLAGS =   -O3 -ggdb -Wall -Wextra -Werror
LDFLAGS = -ggdb

########################################################################

LIBRARY = -lz

SRC_PACKAGE = 		opcode_package.cpp 

SRC_TRACE_READER = 	trace_reader.cpp

SRC_PROCESSOR =	 	btb.cpp processor.cpp

SRC_CORE =  simulator.cpp orcs_engine.cpp\
			$(SRC_TRACE_READER)	\
			$(SRC_PACKAGE) \
			$(SRC_PROCESSOR)

########################################################
OBJS_CORE = ${SRC_CORE:.cpp=.o}
OBJS = $(OBJS_CORE)

#######################################################
# Defina o diretório raiz
ROOT_DIR := ../

# Diretório de benchmarks
BENCHMARKS_DIR := $(ROOT_DIR)/benchmarks

########################################################
# implicit rules
%.o : %.cpp %.hpp
	$(CPP) -c $(CPPFLAGS) $< -o $@

########################################################

all: orcs

orcs: $(OBJS_CORE)
	$(LD) $(LDFLAGS) -o $(BIN_NAME) $(OBJS) $(LIBRARY)

test:
	@echo "Running tests..."
	@for benchmark in $(BENCHMARKS_DIR)/*; do \
		if [ -d "$$benchmark" ]; then \
			benchmark_name=$$(basename "$$benchmark"); \
			benchmark_file="$${benchmark}/$${benchmark_name}.CINT.PP200M.tid0.dyn.out.gz"; \
			if [ -f "$$benchmark_file" ]; then \
				echo "Running: $$benchmark_name"; \
				./orcs -t "../benchmarks/$$benchmark_name/$$benchmark_name.CINT.PP200M"; \
			else \
				benchmark_file="$${benchmark}/$${benchmark_name}.CFP.PP200M.tid0.dyn.out.gz"; \
				if [ -f "$$benchmark_file" ]; then \
					echo "Running: $$benchmark_name"; \
					./orcs -t "../benchmarks/$$benchmark_name/$$benchmark_name.CFP.PP200M"; \
				else \
					echo "Arquivo CINT.PP200M/CFP.PP200M não encontrado para o benchmark: $$benchmark_name"; \
				fi; \
			fi; \
		fi; \
	done

clean:
	-$(RM) $(OBJS)
	-$(RM) $(BIN_NAME)
	@echo OrCS cleaned!
	@echo
