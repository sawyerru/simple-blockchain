/***********
 * This is a simple hashing algorithm - 
 * meant to simply provide collision resistant output given two hashes to combine.
 * Additionally a hash generation function given a uniqe string to create a 32-bit value.
 * ********/
#pragma once

#include <iostream>
using std::cout; using std::ostream; using std::endl;
#include <string> 
using std::string;
#include <ctime>
#include <utility> //std::pair
using std::pair;
#include <iomanip> //std::hex
#include <sstream>
using std::stringstream;
#include <vector>
using std::vector;

pair<string, long> time_now(){ //Given Current time based on Coordinated Universal Time (UTC) in string and long types
    pair<string, long> result;

    time_t timenow = time(0);

    tm *timer = gmtime(&timenow); //pointer to a time_t typed obj - contains time val
    char* utc_time_s = asctime(timer);
    result.first = utc_time_s;

    long utc_time_l = static_cast<long> (timenow);
    result.second = utc_time_l;

    return result;
}

long compression( long arg1, long arg2){   //compress two longs into one using bit operations 
    return (arg1 * arg2) ^ ((arg1 << 3) + (arg2 >> 2));
}

long ID_hash(string name){  //Given a unique string this function returns a unique long as a address
      
    unsigned long mask = 0xa7406e9c; //magic number
    unsigned long value;
    stringstream ss;
    for (auto ch: name){
        auto nchar = int(ch);
        ss << std::hex << nchar;
    }
    ss >> value;

    return (value >> 5) | (value << (-5 & mask));
}

unsigned long tx_hash(long sender_ID, long receiver_ID, long data, long timestamp) {//hashing transaction data together in a series of compression steps
    //Follows loosely to Merkle-Damgard Construction - description found here: 
    // http://www.partow.net/programming/hashfunctions/

    long initial_vector = 0x4cc4310e; //Magic number
    long intermediate1 = compression(initial_vector, sender_ID);
    long intermediate2 = compression(intermediate1, receiver_ID);
    long intermediate3 = compression(intermediate2, data);
    long intermediate4 = compression(intermediate3, timestamp);

    return static_cast<unsigned long> (intermediate4);
    //return is always 8 bytes 
}

unsigned long merkle_hash(unsigned long tx_array[BLX_SZ]){ //Hashing
    long interim = 0x7d2bb001; //magic number 
    for (unsigned int i=0; i < BLX_SZ; i++)
        interim = compression(interim, tx_array[i]);
        
    return static_cast<unsigned long> (interim);
}

unsigned long block_hash(unsigned long merkle_hash, unsigned long timestamp, \
                        unsigned long prev_hash, unsigned long number, bool valid){ //Hash all block information together to create a "Block Hash"
    long initial_val = (valid) ? number + 0x00000001 : number + 0x00000000 ; 
    //Magic Number ^^ - Composed from blocknumber, and valid/unvalid block
    //Valid = 0x00000001      Invalid = 0x00000000
    long intermediate1 = compression(initial_val, merkle_hash);
    long intermediate2 = compression(intermediate1, timestamp);
    long intermediate3 = compression(intermediate2, prev_hash);
    
    return intermediate3;
}