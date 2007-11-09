#include  "../stactive_record.h"

using namespace stactiverecord;

void assert(bool v, string msg) {
  if(v) return;
  throw Sar_AssertionFailedException("Failed when testing " + msg);
}

int main() {
  debug("Testing queries..."); 

  SarMap<string> sm;
  sm["one"] = "two";
  sm["bar"] = "foo";

  Q q("one", "two");
  Q tq("two", "three");
  Q blah("bar", "foo");

  Q test = ((q && blah) || (tq && blah));
  test.dump();

  //  if(((q && blah) || (tq && blah)).test(sm))
  //  debug("Passed!");

  debug("If you're at this point, no errors were found.");

  return 0;
}
