#include  "../stactive_record.h"

using namespace stactiverecord;

void assert(bool v, string msg) {
  if(v) return;
  throw Sar_AssertionFailedException("Failed when testing " + msg);
}

int main() {
  debug("Testing queries..."); 

  Q q("one", "two");
  Q tq("two", "three");
  Q blah("four", "five");
  Q ttq = (q && (tq || blah));
  ttq.dump();

  debug("If you're at this point, no errors were found.");

  return 0;
}
