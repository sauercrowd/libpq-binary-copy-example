#include "numbers.pb.h"
#include "text.pb.h"
#include "config.h"

#include <libpq-fe.h>
#include <arpa/inet.h>

void main(){
    PGconn* conn = setup();
    if(conn==nullptr){
        return;
    }

    startCopy(conn);
    addResult()
    endCopy(conn);
     
}

PGconn* setup(){
    //connect to db
    conn = PQconnectdb(("dbname="+POSTGRES_DB+" user="+POSTGRES_USER+" password="+POSTGRES_PASS+" hostaddr="+POSTGRES_HOST+" port=5432").c_str());
    if(PQstatus(conn) != CONNECTION_OK){
       std::cout << "Could not connect to DB: " <<  std::endl;
       return nullptr;
    }
    
    //create table if not exist
    PGresult* r = PQexec(conn, "CREATE TABLE IF NOT EXISTS data(name text, bytes bytea)");
    if(PQresultStatus(r) != PGRES_COMMAND_OK){
        std::cout << "Could not create table: "<< PQerrorMessage(conn);
        return nullptr;
    }
    return conn;
}

int startCopy(PGconn* conn){
    //initiate copy
    PGresult* r = PQexec(conn, "COPY data FROM STDIN (FORMAT binary)");
    if(PQresultStatus(r) != PGRES_COPY_IN){
        std::cout << "Could not start copy: " <<PQerrorMessage(conn);
        PQclear(r);
        return -1;
    }

    //write binary copy header to connection
    char* header            = "PGCOPY\n\377\r\n";              //10
    int nullbyte            = 0;                               //1
    int flags               = htonl(0);                        // no idea what they do and what they describe, but need to be present
    int header_extionsion   = htonl(0);                        // same

    //calc full size for buffer(header, zero byte, flags, header_extension, trailer)
    int size_buffer = 10 + 1 + SIZE_INT + SIZE_INT;

    void* buffer = malloc(size_buffer);
    //copy everything into the buffer
    int offset = 0;
    std::memcpy(buffer+offset, (void*) header, 10);
    offset+=10;
    std::memcpy(buffer+offset, (void*) &nullbyte, 1);
    offset+=1;
    std::memcpy(buffer+offset, (void*) &flags,4);
    offset+=4;
    std::memcpy(buffer+offset, (void*) &header_extionsion,4);
    
    int r_i = PQputCopyData(conn, (const char*) buffer, buffer_size);

    free(buffer);

    //check if error happened
    if(r == 1) return 0; //success
    if(r == 0){
        std::cout << "could not queue data" << std::endl;
        return -1
    }
    if(r == -1){
        std::cout << "could not queue data" << PQerrorMessage(p) << std::endl;
        return -1;
    }

}

#define SIZE_SHORT 2
#define SIZE_INT 4
#define SIZE_DOUBLE 8
#define SIZE_LONG 8

Numbers addResult(PGconn* conn, std::string name, const char* bytes, int size){
    int buffer_size = SIZE_SHORT;                // for tuple (number of fields)
    buffer_size+= SIZE_INT + name.length();      // size_field + name size
    buffer_size+= SIZE_INT + size;               // size_field + buffer size

    int offset = 0;
    void* buffer = malloc(buffer_size);

    short number_fields = htons(2); //two fields, name and bytes, needs to be in network order, 
    //set number of fields per tuple
    std::memcpy(buffer+offset, (void*) &number_fields, SIZE_SHORT);
    offset+=2;

    //
    int size_int    = htonl(SIZE_INT);
    int size_double = htonl(SIZE_DOUBLE);
    int size_long   = htonl(SIZE_LONG);
    int size_short  = htonl(SIZE_SHORT);
  
    //name
    int size_name = 
    std::memcpy(buffer+offset, (void*) &size_long, SIZE_INT); //set size
    offset+=SIZE_INT;
    long simId_n = htonll(simId);
    std::memcpy(buffer+offset, (void*) &simId_n, SIZE_LONG);
    offset+=SIZE_LONG;

    //agentid
    int agentid_size = htonl(agentId.size());
    std::memcpy(buffer+offset, (void*) &agentid_size, SIZE_INT);
    offset+=SIZE_INT;
    std::memcpy(buffer+offset, (void*) agentId.data(), agentId.size());
    offset+=agentId.size();

    //agenttypeid
    std::memcpy(buffer+offset, (void*) &size_long, SIZE_INT);
    offset+=SIZE_INT;
    long agentTypeId_n = htonll(agentTypeId);
    std::memcpy(buffer+offset, (void*) &agentTypeId_n, SIZE_LONG);
    offset+=SIZE_LONG;

    //simtime
    std::memcpy(buffer+offset, (void*) &size_double, SIZE_INT);
    offset+=SIZE_INT;
    double simTime_n = htond(simTime);
    std::memcpy(buffer+offset, (void*) &simTime_n, SIZE_DOUBLE);
    offset+=SIZE_DOUBLE;

    //data
    int data_size_network = htonl(data_size);
    std::memcpy(buffer+offset, (void*) &data_size_network, SIZE_INT);
    offset+=SIZE_INT;
    std::memcpy(buffer+offset, data, data_size);
    offset+=data_size;


}