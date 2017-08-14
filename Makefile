CPP_OUT=cpp_out
PROTO_FILES=$(patsubst %.proto, $(CPP_OUT)/%.pb.cc, $(shell ls *.proto))
TARGET_FILES=$(patsubst %.proto, %, $(shell ls *.cpp))
PROTO_URL="https://github.com/google/protobuf/releases/download/v3.3.0/protobuf-cpp-3.3.0.tar.gz"
PROTO_DIST_DIR=$(shell pwd)/protobuf_dist

THREADS=8

all: protobuf $(CPP_OUT) $(PROTO_FILES)

# Target files
%: %.cpp
	$(CXX) -I `pwd`/$(CPP_OUT) $<

# Protobuf
$(CPP_OUT):
	mkdir $(CPP_OUT)

$(CPP_OUT)/%.pb.cc: %.proto
	$(PROTO_DIST_DIR)/bin/protoc -I . --cpp_out=$(CPP_OUT) $<

# Protobuf Libs
protobuf: $(PROTO_DIST_DIR)/lib

$(PROTO_DIST_DIR)/lib: protolib $(PROTO_DIST_DIR)
	cd protolib && ./autogen.sh
	cd protolib && ./configure --prefix=$(PROTO_DIST_DIR)
	$(MAKE) -j $(THREADS) -C protolib
	$(MAKE) -C protolib install

$(PROTO_DIST_DIR): 
	mkdir $(PROTO_DIST_DIR)

protolib: protobuf.tar.gz
	mkdir tmp
	cp protobuf.tar.gz tmp/
	cd tmp && tar xvf protobuf.tar.gz
	cd tmp && rm protobuf.tar.gz
	cd tmp && mv * protolib
	mv tmp/protolib protolib
	rm -r tmp

protobuf.tar.gz:
	wget -qO protobuf.tar.gz $(PROTO_URL)

clean:
	rm -rf $(CPP_OUT) protobuf.tar.gz protolib

.PHONY: clean protobuf