#编译account公共库.a文件
LIB_TARGET = lib/libaccount_common.a
INC_DIR= -I./include/
SERVICE_DIR= src/
PROTO_DIR= proto
PROTO_CC_DIR= proto_cc/
OBJ_DIR=
LIB_DIR=
OBJ_EXT= .o
CXXSRC_EXT= .cc

COMMON_BASE_PLIB= ./../plib
PROTO_LIB_DIR= ../proto_lib
UTILS_DIR=../utils
CONFIG_DIR=../config

SERVICE_OBJECTS = $(patsubst %.cc, %.o, $(notdir $(wildcard $(SERVICE_DIR)*.cc)))

INC =  $(INC_DIR) \
	 -I$(COMMON_BASE_PLIB)/applib/zhibo_util/include \
     -I$(COMMON_BASE_PLIB)/applib/hy_mstore_api_v2/include  \
     -I$(COMMON_BASE_PLIB)/applib/Iris_api_v3/include  \
     -I$(PROTO_LIB_DIR)/include \
     -I$(UTILS_DIR)/include \
     -I$(CONFIG_DIR)/include \
     -I$(COMMON_BASE_PLIB)/applib/bitmap_api/include \
     -I$(COMMON_BASE_PLIB)/mfcgi/include \
     -I$(COMMON_BASE_PLIB)/spp/include  \
     -I${COMMON_BASE_PLIB}/webapp_syncframe_v2/include \
     -I$(COMMON_BASE_PLIB)/mlb/include \
     -I$(COMMON_BASE_PLIB)/outerlib/zkclient-3.4.6/include \
     -I$(COMMON_BASE_PLIB)/baselib/include \
     -I$(COMMON_BASE_PLIB)/outerlib/protobuf-2.6.1/include \
     -I$(COMMON_BASE_PLIB)/outerlib/json-c-1.6/include \
     -I$(COMMON_BASE_PLIB)/outerlib/jsoncpp-1.6.2/include \
     -I$(COMMON_BASE_PLIB)/outerlib/openssl-1.0.2a/include \
     -I$(COMMON_BASE_PLIB)/outerlib/curl-7.45.0/include \
     -I$(COMMON_BASE_PLIB)/outerlib/thrift-0.8.0/include \
     -I$(COMMON_BASE_PLIB)/outerlib/boost-1.41.0/include \
     -I$(COMMON_BASE_PLIB)/applib/zhibo_util/include \

C_FLAGS = -g -O2 -Wall -fPIC -D_GNU_SOURCE -std=c++0x

THRIFT_FLAGS = -DHAVE_NETINET_IN_H -DHAVE_INTTYPES_H -Wreturn-type -Wno-literal-suffix

CGI=
ifeq ($(CGI),1)
C_FLAGS += -DMHTTP_LOG
endif

STD=
ifeq ($(STD),1)
C_FLAGS += -DSTD_LOG
endif

MCP=
ifeq ($(MCP),1)
C_FLAGS += -DMCP_LOG
endif

all: $(LIB_TARGET) 

$(OBJ_DIR)%$(OBJ_EXT):  $(SERVICE_DIR)%$(CXXSRC_EXT)
	@echo -e "\033[1;33m\nCompiling $< ==> $@ \033[0m..."
	$(CXX) $(INC) $(C_FLAGS)  -c $< -o $@
	
$(OBJ_DIR)%$(OBJ_EXT):  $(DAO_DIR)%$(CXXSRC_EXT)
	@echo -e "\033[1;33m\nCompiling $< ==> $@ \033[0m..."
	$(CXX) $(INC) $(C_FLAGS)  -c $< -o $@
	
$(OBJ_DIR)%$(OBJ_EXT):  $(UTIL_DIR)%$(CXXSRC_EXT)
	@echo -e "\033[1;33m\nCompiling $< ==> $@ \033[0m..."
	$(CXX) $(INC) $(C_FLAGS)  -c $< -o $@
	
$(OBJ_DIR)%$(OBJ_EXT):  $(THRIFT_DIR)%.cpp
	@echo -e "\033[1;33m\nCompiling $< ==> $@ \033[0m..."
	$(CXX) $(INC) $(C_FLAGS) -c $< -o $@

$(OBJ_DIR)%$(OBJ_EXT):  $(CONSTANT_DIR)%$(CXXSRC_EXT)
	@echo -e "\033[1;33m\nCompiling $< ==> $@ \033[0m..."
	$(CXX) $(INC) $(C_FLAGS)  -c $< -o $@
	
$(OBJ_DIR)%$(OBJ_EXT):  $(CLIENT_DIR)%$(CXXSRC_EXT)
	@echo -e "\033[1;33m\nCompiling $< ==> $@ \033[0m..."
	$(CXX) $(INC) $(C_FLAGS) $(THRIFT_FLAGS) -c $< -o $@

$(OBJ_DIR)%$(OBJ_EXT):  $(PROTO_CC_DIR)%$(CXXSRC_EXT)
	@echo -e "\033[1;33m\nCompiling $< ==> $@ \033[0m..."
	$(CXX) $(INC) $(C_FLAGS)  -c $< -o $@
	
$(OBJ_DIR)%$(OBJ_EXT):  $(DEPENDENT_SERVICE_DIR)%$(CXXSRC_EXT)
	@echo -e "\033[1;33m\nCompiling $< ==> $@ \033[0m..."
	$(CXX) $(INC) $(C_FLAGS)  -c $< -o $@

$(LIB_TARGET): $(SERVICE_OBJECTS)
	@echo -e "\033[1;33m\$(OBJECTS) ==> $@ \033[0m..."
	$(AR) x $(PROTO_LIB_DIR)/lib/libpbpkg.a
	$(AR) x $(UTILS_DIR)/lib/libutils.a
	$(AR) x $(CONFIG_DIR)/lib/libconfig.a
	$(AR) x $(COMMON_BASE_PLIB)/applib/zhibo_util/lib/libzhibo_util.a
	$(AR) x $(COMMON_BASE_PLIB)/mfcgi/lib/libmfcgi.a
	$(AR) x $(COMMON_BASE_PLIB)/applib/hy_mstore_api_v2/lib/libhy_mstore_api.a
	$(AR) x $(COMMON_BASE_PLIB)/applib/Iris_api_v3/lib/libiris_api.a
	$(AR) x $(COMMON_BASE_PLIB)/baselib/lib/libhy_base.a
	$(AR) x $(COMMON_BASE_PLIB)/mlb/lib/libqos_client.a
	$(AR) x $(COMMON_BASE_PLIB)/outerlib/protobuf-2.6.1/lib/libprotobuf.a
	$(AR) x $(COMMON_BASE_PLIB)/outerlib/json-c-1.6/lib/libjson-c.a
	$(AR) x $(COMMON_BASE_PLIB)/outerlib/jsoncpp-1.6.2/lib/libjsoncpp.a
	$(AR) x $(COMMON_BASE_PLIB)/outerlib/openssl-1.0.2a/lib/libcrypto.a
	$(AR) x $(COMMON_BASE_PLIB)/outerlib/openssl-1.0.2a/lib/libssl.a
	$(AR) x $(COMMON_BASE_PLIB)/outerlib/curl-7.45.0/lib/libcurl.a
	$(AR) x $(COMMON_BASE_PLIB)/outerlib/thrift-0.8.0/lib/libthrift.a
	$(AR) rc  $(LIB_TARGET) *.o 
	rm -f *.o

clean:
	rm -f $(LIB_TARGET) $(SERVICE_OBJECTS)

.PHONY: clean
