CC = g++

CXXFLAGS = -std=c++17 -Ofast #g -Wall -Wshadow -fsanitize=address,undefined -fno-omit-frame-pointer

INCLUDE_FOLDER = ./include/
OBJ_FOLDER = ./obj/
SRC_FOLDER = ./src/

TARGET = a
SRC = $(wildcard $(SRC_FOLDER)*.cpp)
OBJ = $(patsubst $(SRC_FOLDER)%.cpp, $(OBJ_FOLDER)%.o, $(SRC))

$(OBJ_FOLDER)%.o: $(SRC_FOLDER)%.cpp
	mkdir $(OBJ_FOLDER) -p && $(CC) $(CXXFLAGS) -c $< -o $@ -I$(INCLUDE_FOLDER)

all: $(OBJ)
	$(CC) $(CXXFLAGS) -o ./$(TARGET) $(OBJ)

clean:
	@rm -rf $(OBJ_FOLDER)
