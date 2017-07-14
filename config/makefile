TARGET_FILE=lib
TARGET=libconfig.a
PLIB_FILE=../plib
UTILS_FILE=../utils

C_FLAGS=-g -fPIC -shared -fpermissive -Wno-write-strings

OBJ_EXT=.o
OBJ_FILE=
OBJS=$(patsubst %.cc, %.o, $(notdir $(wildcard $(SRC_FILE)/*.cc)))

SRC_FILE=src
SRC_EXT=.cc

INC+=-Iinclude
INC+=-I$(UTILS_FILE)/include
INC+=-I$(PLIB_FILE)/mfcgi/include
INC+=-I$(PLIB_FILE)/spp/include/
INC+=-I$(PLIB_FILE)/baselib/include
INC+=-I$(PLIB_FILE)/outerlib/protobuf-2.6.1/include
INC+=-I$(PLIB_FILE)/outerlib/json-c-1.6/include
INC+=-I$(PLIB_FILE)/outerlib/openssl-1.0.2a/include
INC+= -I$(PLIB_FILE)/applib/zhibo_util/include

LIB+=-L$(UTILS_FILE)/lib -lutils
LIB+=-L$(PLIB_FILE)/mfcgi/lib -lmfcgi
LIB+=-L$(PLIB_FILE)/baselib/lib -lhy_base
LIB+=-L$(PLIB_FILE)/outerlib/protobuf-2.6.1/lib -lprotobuf
LIB+=-L$(PLIB_FILE)/outerlib/json-c-1.6/lib -ljson-c


$(TARGET_FILE)/$(TARGET):$(OBJS)
	@echo -e "\033[1;33m\$(OBJS) ==> $@ \033[0m..."
	mkdir -p ./$(TARGET_FILE)
	$(AR) rc $(TARGET_FILE)/$(TARGET) $(OBJS)
	rm *.o

$(OBJ_FILE)%$(OBJ_EXT):$(SRC_FILE)/%$(SRC_EXT)
	@echo -e "\033[1;33m\nCompiling $< ==> $@ \033[0m..."
	$(CXX) $(C_FLAGS) $(INC) $(LIB) -c $< -o $@ 

all:
	$(TARGET)
	
clean:
	rm -rf *.o $(TARGET_FILE)/$(TARGET)
