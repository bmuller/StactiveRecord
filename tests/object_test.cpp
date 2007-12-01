#include  "../stactiverecord/stactive_record.h"

using namespace stactiverecord;
using namespace std;

Sar_Dbi * Sar_Dbi::dbi;

void assert(bool v, string msg) {
  if(v) return;
  throw Sar_AssertionFailedException("Failed when testing " + msg);
}

class Test : public Record {
public:
  Test() : Record("test") {};
  Test(int id) : Record("test", id) {};
};

class TestTwo : public Record {
public:
  TestTwo() : Record("testtwo") {};
  TestTwo(int id) : Record("testtwo", id) {};
};

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: ./object_test <db config string>\n";
    return 1;
  }
  Sar_Dbi::dbi = Sar_Dbi::makeStorage(std::string(argv[1]));

  Test t;
  t.set("number", 40);
  t.set("boolean", true);
  t.set("string_one", "one");
  t.set("string_two", "two");
  t.save();

  // test some queries
  ObjGroup<Test> og = Record::find<Test>(Q("number", between(30,50)) && Q("boolean", true));
  assert(og.size() == 1 && og[0].id == t.id, "performing find query");
  og = Record::find<Test>(Q("number", nequals(40)));
  assert(og.size() == 0, "performing find query with no results");
    
  TestTwo tt;
  tt.set("number", 10);
  tt.save();
  t.setOne<TestTwo>(tt);
  t.save();

  // test reverse relationship
  og = tt.getMany<Test>();
  assert(og.size() == 1 && og[0].id == t.id, "testing reverse relationship between objects");

  delete Sar_Dbi::dbi;
  cout << "No errors were found.\n";
  return 0;
}
