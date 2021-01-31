# https://stackoverflow.com/questions/3774568/makefile-issue-smart-way-to-scan-directory-tree-for-c-files
# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html
# Notes:
# 	$(LIB) must be last after list of objects when building
# 	When modifying header files, be sure to make clean
# 	SOIL Must be before opengl32
#	-mwindows will hide the console when the program is run

OUT = gg
SRC_DIR = src
HDR_DIR = src
OUT_DIR = build
BIN_DIR = KomradesKitchen/assets

CC = g++
FLAG = -ggdb -Wall -pthread -Isrc -g
LIBS = -lws2_32 -lsfml-audio -lwsock32 -lprotobuf -lassimp -lglfw3 -lgdi32 -luser32 -lSOIL -lopengl32
SRC := $(shell find $(SRC_DIR) -name "*.cpp")
HDR := $(shell find $(HDR_DIR) -name "*.h")
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OUT_DIR)/%.o) # Replace variables src/%.cpp to out/%.o in SRC variable

BINARY := $(BIN_DIR)/$(OUT)

default: PROTO $(BINARY) LOBBY

PROTO:
	cd src/schema/ && $(MAKE)

$(BINARY): $(OBJ)
	mkdir -p $(shell dirname $@)
	$(CC) $(FLAG) $^ -o $@ $(LIBS)

# $(OBJ): $(SRC) -- $< only refers to first class in this case
$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(shell dirname $@)
	$(CC) $(FLAG) -c $< -o $@

LOBBY:
	g++ -ggdb -Wall -pthread -mwindows -Isrc -g LobbySetup.cpp -o KomradesKitchen/Assets/LobbySetup.exe -luser32 -lgdi32

clean:
	rm -rf $(OUT_DIR)
	rm -f $(BIN_DIR)/gg.exe
