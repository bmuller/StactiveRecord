#include "stactive_record.h"

using namespace stactiverecord;

Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("hello");

class Test : public Record {
public:
  Test() : Record("test") {};
  Test(int id) : Record("test", id) {};
};


int main() {
  Test f;
  f.set("one", "blah");
  f.save();

  f.set("one", 1);  
  f.save();
  f.save();
  f.save();

  //cout << "Saved one as integer 1\n";

  //Test t(f.id);
  //cout << "value should be 1: " << t.get("one") << "\n";
  /*
  t.del("one");
  t.set("one", "four");
  t.set("one", "five");
  
  f.save();


  Test t(f.id);
  t.del("one");
  t.set("one", 2);
  t.del("one");
  t.set("one", 3);
  t.save();
  */
  //cout << "id: " << t.id << "\n";
  //string value;
  //t.get("one", value);
  //cout << "one: " << value << "\n";
  //t.set("one", "three");
  delete Sar_Dbi::dbi;
  return 0;
};
