#include "stactive_record.h"

namespace stactiverecord {
  using namespace std;

  Where * startswith(string value) {
    return new Where(value, STARTSWITH);
  };

  Where * endswith(string value) {
    return new Where(value, ENDSWITH);
  };

  Where * contains(string value) {
    return new Where(value, CONTAINS);
  };
  
  Where * greaterthan(int value) {
    return new Where(value, GREATERTHAN);
  };

  Where * lessthan(int value) {
    return new Where(value, LESSTHAN);
  };

  Where * between(int value, int valuetwo) {
    return new Where(value, valuetwo, BETWEEN);
  };

};
