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

  Where * equals(int value) {
    return new Where(value, EQUALS);
  };

  Where * equals(string value) {
    return new Where(value, EQUALS);
  };

  // negated values 

  Where * nstartswith(string value) {
    return new Where(value, STARTSWITH, true);
  };

  Where * nendswith(string value) {
    return new Where(value, ENDSWITH, true);
  };

  Where * ncontains(string value) {
    return new Where(value, CONTAINS, true);
  };
  
  Where * ngreaterthan(int value) {
    return new Where(value, GREATERTHAN, true);
  };

  Where * nlessthan(int value) {
    return new Where(value, LESSTHAN, true);
  };

  Where * nbetween(int value, int valuetwo) {
    return new Where(value, valuetwo, BETWEEN, true);
  };

  Where * nequals(int value) {
    return new Where(value, EQUALS, true);
  };

  Where * nequals(string value) {
    return new Where(value, EQUALS, true);
  };
};
