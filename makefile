CC						= g++
STD						= -std=c++17
WARNINGS				= -Wall -Wextra -Wpedantic #-Werror
CXXFLAGS				= -g $(STD) $(WARNINGS) -I$(INC_DIR) -DICE_CPP11_MAPPING
LDFLAGS					= $(STD) $(WARNINGS) -pthread -lIce++11
DEPFLAGS			 	= -MT $@ -MD -MP -MF $(DEP_DIR)/$*.d

MASTER_SERVER_EXE		= server
CHILD_SERVER_EXE		= chatRoom
CLIENT_EXE				= client

SLICE_SRC				= chat.ice

COMMON_SRC				+=$(SLICE_SRC:.ice=.cpp)
COMMON_SRC				+=ChatUtils.cpp

MASTER_SERVER_SRC		+=Server.cpp
MASTER_SERVER_SRC		+=chatServerI.cpp 
MASTER_SERVER_SRC		+=$(COMMON_SRC)

CHILD_SERVER_SRC		+=ChatRoomServer.cpp
CHILD_SERVER_SRC		+=chatRoomI.cpp
CHILD_SERVER_SRC		+=chatRoomFactoryI.cpp
CHILD_SERVER_SRC		+=$(COMMON_SRC)

CLIENT_SRC				+=Client.cpp
CLIENT_SRC				+=UserI.cpp
CLIENT_SRC				+=$(COMMON_SRC)

EXE_DIR					= bin
SRC_DIR					= src
INC_DIR					= include
OBJ_DIR					= .obj
DEP_DIR					= .dep

MASTER_SERVER_OBJ		= $(MASTER_SERVER_SRC:.cpp=.o)
CHILD_SERVER_OBJ		= $(CHILD_SERVER_SRC:.cpp=.o)
CLIENT_OBJ				= $(CLIENT_SRC:.cpp=.o)
DEP						= $(MASTER_SERVER_OBJ_PATH:.o=.d) $(CHILD_SERVER_OBJ_PATH:.o=.d) $(CLIENT_OBJ_PATH:.o=.d)

MASTER_SERVER_EXE_PATH	= $(patsubst %, $(EXE_DIR)/%, $(MASTER_SERVER_EXE))
CHILD_SERVER_EXE_PATH	= $(patsubst %, $(EXE_DIR)/%, $(CHILD_SERVER_EXE))
CLIENT_EXE_PATH			= $(patsubst %, $(EXE_DIR)/%, $(CLIENT_EXE))
MASTER_SERVER_OBJ_PATH	= $(patsubst %, $(OBJ_DIR)/%, $(MASTER_SERVER_OBJ))
CHILD_SERVER_OBJ_PATH	= $(patsubst %, $(OBJ_DIR)/%, $(CHILD_SERVER_OBJ))
CLIENT_OBJ_PATH			= $(patsubst %, $(OBJ_DIR)/%, $(CLIENT_OBJ))
SLICE_SRC_PATH			= $(patsubst %, $(SRC_DIR)/%, $(SLICE_SRC))

all: $(MASTER_SERVER_EXE_PATH) $(CHILD_SERVER_EXE_PATH) $(CLIENT_EXE_PATH)

slice: $(SLICE_SRC_PATH)
	slice2cpp $^ --output-dir="$(SRC_DIR)"
	mv -f $(^:.ice=.h) $(INC_DIR)

$(MASTER_SERVER_EXE_PATH): $(MASTER_SERVER_OBJ_PATH)
	$(CC) -o $@ $^ $(LDFLAGS)

$(CHILD_SERVER_EXE_PATH): $(CHILD_SERVER_OBJ_PATH)
	$(CC) -o $@ $^ $(LDFLAGS)

$(CLIENT_EXE_PATH): $(CLIENT_OBJ_PATH)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEP_DIR)/%.d
	$(CC) $(DEPFLAGS) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o $(MASTER_SERVER_EXE_PATH) $(CHILD_SERVER_EXE_PATH) $(CLIENT_EXE_PATH)

.PHONY: cleandep
cleandep:
	rm -f $(DEP_DIR)/*.d

.PRECIOUS: $(DEP_DIR)/%.d
$(DEP_DIR)/%.d: ;

-include $(DEP)
