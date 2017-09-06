CXXFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic -g3
CXX=gcc
SRC= p4_main.c p4.c p4_IA_3.c
BIN= p4 

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@
clean: 
	@rm -rf $(BIN)
