#include "stactive_record.h"

#include <stdio.h>

using namespace stactiverecord;

Sar_Dbi *Sar_Dbi::dbi = Sar_Dbi::makeStorage("mysql");
class Test : public Record {
public:
  Test() : Record("test") {};
  Test(int id) : Record("test", id) {};
};

int main() {
  try {
    Test t(0);
    t.set("one", "two");
    //t.save();
    cout << "id: " << t.id << "\n";
    string value;
    t.get("one", value);
    cout << "one: " << value << "\n";
    delete Sar_Dbi::dbi;
  } catch (Sar_DBException &e) {
    fprintf(stdout, "%s\n", e.what());
  }
  return 0;
};

/*Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("hello");

class Test : public Record {
public:
  Test() : Record("test") {};
  Test(int id) : Record("test", id) {};
};


int main() {
  Test t;
  delete Sar_Dbi::dbi;
  return 0;
};
*/
