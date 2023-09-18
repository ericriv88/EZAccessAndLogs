////////////////////////////////////////////////////////////////////////////
//**********************Hash Functions***********************************///
////////////////////////////////////////////////////////////////////////////

#include <Crypto.h>
#include <SHA256.h>

#define HASH_SIZE 32

String toHash(String in) {
  String ret;
  uint8_t result[HASH_SIZE];
  SHA256 sha256;              //initialize sha256 algorithm
  sha256.update(in.c_str(), in.length()); // Use the correct length of the input string
  sha256.finalize(result, HASH_SIZE);     //compute the hash
  for (int i = 0; i < HASH_SIZE; i++) {
    ret += String(result[i], HEX); // Convert the result to a hexadecimal string
  }
  sha256.reset();
  return ret;
}