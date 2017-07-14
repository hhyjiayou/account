LIB_TARGET = zhibo-account-open.so
CC  = gcc
CXX = g++
SRC_DIR= src/
CFLAGS = -g -fPIC -shared -O2  -Wall
ACCOUNT_COMMON_DIR = ../account_common
ACCOUNT_COMMON_INCLUDE_DIR = ../account_common/include
PROTO_LIB_DIR=../proto_lib
UTILS_DIR=../utils
CONFIG_DIR=../config
COMMON_BASE_PLIB= ./../plib

INC = -I${ACCOUNT_COMMON_INCLUDE_DIR}/ \
-I${SRC_DIR} \
-I${ACCOUNT_COMMON_DIR}/proto_cc \
-I$(PROTO_LIB_DIR)/include \
-I$(RELEASE_LIB_DIR)/include \
-I$(UTILS_DIR)/include \
-I$(CONFIG_DIR)/include \
-I$(COMMON_BASE_PLIB)/applib/zhibo_util/include \
-I$(COMMON_BASE_PLIB)/applib/Iris_api_v3/include \
-I$(COMMON_BASE_PLIB)/applib/hy_mstore_api_v2/include  \
-I$(COMMON_BASE_PLIB)/mlb/include \
-I$(COMMON_BASE_PLIB)/applib/bitmap_api/include \
-I${COMMON_BASE_PLIB}/spp/include/ \
-I${COMMON_BASE_PLIB}/baselib/include/ \
-I$(COMMON_BASE_PLIB)/outerlib/protobuf-2.6.1/include \
-I$(COMMON_BASE_PLIB)/outerlib/json-c-1.6/include \
-I$(COMMON_BASE_PLIB)/outerlib/jsoncpp-1.6.2/include \
-I$(COMMON_BASE_PLIB)/outerlib/openssl-1.0.2a/include \
-I$(COMMON_BASE_PLIB)/outerlib/curl-7.45.0/include \
-I$(COMMON_BASE_PLIB)/outerlib/thrift-0.8.0/include \
-I$(COMMON_BASE_PLIB)/outerlib/boost-1.41.0/include

LIB  = -L$(ACCOUNT_COMMON_DIR)/lib -laccount_common \
-L$(PROTO_LIB_DIR)/lib -lpbpkg \
-L$(UTILS_DIR)/lib -lutils \
-L$(COMMON_BASE_PLIB)/applib/zhibo_util/lib/libzhibo_util.a \
-L$(COMMON_BASE_PLIB)/applib/Iris_api_v3/lib -liris_api \
-L$(COMMON_BASE_PLIB)/applib/bitmap_api/lib -lbitmap_api \
-L$(COMMON_BASE_PLIB)/applib/hy_mstore_api_v2/lib/ -lhy_mstore_api \
-L$(COMMON_BASE_PLIB)/baselib/lib -lhy_base \
-L$(COMMON_BASE_PLIB)/outerlib/protobuf-2.6.1/lib/ -lprotobuf \
-L$(COMMON_BASE_PLIB)/outerlib/json-c-1.6/lib -ljson-c \
-L$(COMMON_BASE_PLIB)/mlb/lib -lqos_client \
-L$(COMMON_BASE_PLIB)/outerlib/jsoncpp-1.6.2/lib -ljsoncpp \
-L$(COMMON_BASE_PLIB)/outerlib/openssl-1.0.2a/lib -lssl -lcrypto -ldl \
-L$(COMMON_BASE_PLIB)/outerlib/thrift-0.8.0/lib -lthrift \
-L$(COMMON_BASE_PLIB)/outerlib/boost-1.41.0/lib -lboost_regex \
-L$(COMMON_BASE_PLIB)/outerlib/boost-1.41.0/lib -lboost_system  -lboost_filesystem \
-L$(COMMON_BASE_PLIB)/outerlib/curl-7.45.0/lib -lcurl -ldl -lz -lldap -lidn -lrt

OBJECTS = $(patsubst %.cc, %.o, $(notdir $(wildcard ${SRC_DIR}*.cc)))

all: clean $(LIB_TARGET)
	
%.o: ${SRC_DIR}%.cc
	$(CXX) $(CFLAGS) -c -o $@ $(INC) $<
	
$(LIB_TARGET): $(OBJECTS)

	@echo -e "\033[1;33m\$(OBJECTS) ==> $@ \033[0m..."
	$(CXX) -o $@ $^ -fPIC -shared $(LIB)


clean:
	rm -rf *.o $(LIB_TARGET)

