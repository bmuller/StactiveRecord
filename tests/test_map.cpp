#include "stactive_record.h"

using namespace stactiverecord;

int main() {
  SarMap<string> s;
  //map<string,string> s;
  s["one"] = "two";
  string y = s["one"];
  cout << "asdf: " << y << "\n";
  return 0;
}
