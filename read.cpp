#include "numbers.pb.h"
#include "text.pb.h"
#include <libpq-fe.h>

void main(){
    return;
}

// read and create numbers
Numbers readNumbers(char* bytes){
    Numbers n;
    n.ParseFromArray(bytes);
}

