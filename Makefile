CC = g++

CXXFLAGS = -std=c++17 -Ofast -static # -march=native g -Wall -Wshadow -fsanitize=address,undefined -fno-omit-frame-pointer

OBJ_FOLDER = ./obj/
SRC_FOLDER = ./src/
LP_SOLVE = ./lp_solve_5.5/

TARGET = a
SRC = $(wildcard $(SRC_FOLDER)*.cpp)
OBJ = $(patsubst $(SRC_FOLDER)%.cpp, $(OBJ_FOLDER)%.o, $(SRC))

$(OBJ_FOLDER)%.o: $(SRC_FOLDER)%.cpp
	mkdir $(OBJ_FOLDER) -p && $(CC) $(CXXFLAGS) -c $< -o $@ -I$(SRC_FOLDER) -I$(LP_SOLVE)

all: $(OBJ)
	$(CC) $(CXXFLAGS) -o ./$(TARGET) $(OBJ) ./lp_solve_5.5/lpsolve55/bin/ux64/liblpsolve55.a -static-libstdc++

clean:
	@rm -rf $(OBJ_FOLDER)
