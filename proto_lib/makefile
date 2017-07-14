LIB_DIR = ./lib
LIB_TARGET = $(LIB_DIR)/libpbpkg.a
INC_DIR= ./include/
SRC_DIR= src/
OBJ_DIR=
PROTO_DIR= ./proto
OBJ_EXT= .o
CXXSRC_EXT= .cc
PLIB_FILE=../../plib
PB_INC_DIR = ./include
PB_SRC_DIR = ./src
OBJECTS = $(patsubst %.cc, %.o, $(notdir $(wildcard $(SRC_DIR)*.cc)))

INC += -Iinclude
INC += -I$(PLIB_FILE)/outerlib/protobuf-2.6.1/include/


#  -w 选项不打印警告
C_FLAGS = -g -fPIC  -fpermissive -Wno-write-strings


$(OBJ_DIR)%$(OBJ_EXT):  $(SRC_DIR)%$(CXXSRC_EXT)
	@echo -e "\033[1;33m\nCompiling $< ==> $@ \033[0m..."
	$(CXX) $(C_FLAGS) $(INC) $(LIB) -c $< -o $@
        
$(LIB_TARGET): $(OBJECTS)
	@echo -e "\033[1;33m\$(OBJECTS) ==> $@ \033[0m..."
	mkdir -p $(LIB_DIR)
	$(AR) cru $(LIB_TARGET) $(OBJECTS)
	rm *.o

pb:
	cd $(PROTO_DIR);make clean;make;cd ..;

all: 
	complie_pb $(LIB_TARGET) 

cleanpb:
	rm  $(PB_INC_DIR)/*.pb.h
	rm  $(PB_SRC_DIR)/*.pb.cc


clean:
	rm -f $(LIB_TARGET) $(OBJECTS)

