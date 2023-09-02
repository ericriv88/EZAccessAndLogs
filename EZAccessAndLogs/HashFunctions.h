////////////////////////////////////////////////////////////////////////////
//**********************Hash Functions***********************************///
////////////////////////////////////////////////////////////////////////////

#include <Crypto.h>
#include <SHA256.h>

#define HASH_SIZE 32

String toHash(String in) {
  String ret;                     //initialize return string
  uint8_t result[HASH_SIZE];      //create buffer for resulting hash
  SHA256 sha256;                  //create sha256 instance and update with input and hash size
  sha256.update(in.c_str(), sizeof(in));
  sha256.finalize(result, HASH_SIZE);
  for(int i = 0; i < HASH_SIZE; i++) {  //convert resulting buffer into string 
    ret += result[i];
  }
  sha256.reset();
  return ret;
}