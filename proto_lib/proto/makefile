PROTO_CC_DIR = ../proto_cc
PB_INC_DIR = ../include
PB_SRC_DIR = ../src
proto_files = $(wildcard *.proto)
all:
	mkdir -p $(PROTO_CC_DIR)
	@echo -e "\033[1;33m\nCreate pb file\033[0m..."
	protoc $(proto_files) --cpp_out=$(PROTO_CC_DIR)
	mv $(PROTO_CC_DIR)/*.pb.h  $(PB_INC_DIR) 
	mv $(PROTO_CC_DIR)/*.pb.cc $(PB_SRC_DIR) 
