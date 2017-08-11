CPP_OUT=cpp_out
PROTO_FILES=$(patsubst %.proto, $(CPP_OUT)/%.pb.cc, $(shell ls *.proto))
TARGET_FILES=$(patsubst %.proto, %, $(shell ls *.cpp))
PROTOLIBS_URL="https://github.com/google/protobuf/releases/download/v3.3.0/protobuf-cpp-3.3.0.tar.gz"

all: $(CPP_OUT) $(PROTO_FILES)

# Protobuf
$(CPP_OUT):
	mkdir $(CPP_OUT)

$(CPP_OUT)/%.pb.cc: %.proto
	protoc -I . --cpp_out=$(CPP_OUT) $<

# Protobuf Libs

protolib: protobuf.tar.gz
	mkdir tmp
	cp protobuf.tar.gz tmp/
	cd tmp && tar xvf protobuf.tar.gz
	cd tmp && rm protobuf.tar.gz
	cd tmp && mv * protolib
	mv tmp/protolib protolib
	rm -r tmp

protobuf.tar.gz:
	wget -O protobuf.tar.gz https://github.com/google/protobuf/releases/download/v3.3.0/protobuf-cpp-3.3.0.tar.gz


# Target files
%: %.cpp
	gcc -I `pwd`/$(CPP_OUT) $<

clean:
	rm -rf $(CPP_OUT)

.PHONY: clean