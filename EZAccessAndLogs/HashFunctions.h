////////////////////////////////////////////////////////////////////////////
//**********************Hash Functions***********************************///
////////////////////////////////////////////////////////////////////////////

#include <XxHash_arduino.h>

String toHash(String in) {
  char hashOut[9];
  String ret;
  xxh32(hashOut, in.c_str());
  ret = String(hashOut);
  return ret;
}