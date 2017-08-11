CPP_OUT=cpp_out
PROTO_FILES=$(patsubst %.proto, $(CPP_OUT)/%.pb.cc, $(shell ls *.proto))


all: $(CPP_OUT) $(PROTO_FILES)

$(CPP_OUT):
	mkdir $(CPP_OUT)

$(CPP_OUT)/%.pb.cc: %.proto
	protoc -I . --cpp_out=$(CPP_OUT) $<

clean:
	rm -rf $(CPP_OUT)

.PHONY: clean