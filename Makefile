CC = g++

CXXFLAGS = -std=c++17 -Ofast #g -Wall -Wshadow -fsanitize=address,undefined -fno-omit-frame-pointer

INCLUDE_FOLDER = ./include/
OBJ_FOLDER = ./obj/
SRC_FOLDER = ./src/
CADICAL_FOLDER = ./cadical/src
CADICAL_BUILD = ./cadical/build

TARGET = a
SRC = $(wildcard $(SRC_FOLDER)*.cpp)
OBJ = $(patsubst $(SRC_FOLDER)%.cpp, $(OBJ_FOLDER)%.o, $(SRC))

$(OBJ_FOLDER)%.o: $(SRC_FOLDER)%.cpp cadical
	mkdir $(OBJ_FOLDER) -p && $(CC) $(CXXFLAGS) -c $< -o $@ -I$(INCLUDE_FOLDER) -I$(CADICAL_FOLDER)

all: $(OBJ)
	$(CC) $(CXXFLAGS) -o ./$(TARGET) $(OBJ) -L$(CADICAL_BUILD) -lcadical

cadical:
	cd cadical && ./configure && $(MAKE)

clean:
	@rm -rf $(OBJ_FOLDER)
