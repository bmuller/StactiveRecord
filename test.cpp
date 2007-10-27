#include "stactive_record.h"

using namespace stactiverecord;

Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("hello");

class Test : public Record {
public:
  Test() : Record("test") {};
  Test(int id) : Record("test", id) {};
};


int main() {
  Test t(0);
  //t.set("one", "three");
  //t.save();
  //cout << "id: " << t.id << "\n";
  string value;
  t.get("one", value);
  cout << "one: " << value << "\n";
  //t.set("one", "three");
  delete Sar_Dbi::dbi;
  return 0;
};
