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

class TestThree : public Record {
public:
  TestThree() : Record("testthree") {};
  TestThree(int id) : Record("testthree", id) {};
};

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: ./object_test <scheme://[user[:password]@host[:port]/]database>\n";
    return 1;
  }
  Sar_Dbi::dbi = Sar_Dbi::makeStorage(std::string(argv[1]));

  //std::cout << "datetime int is: " << DateTime(5, 29, 1984).to_int() << "\n";

  Test t;
  t.set("number", 40);
  t.set("boolean", true);
  t.set("dob", DateTime(5, 29, 1984));
  t.save();

  // test some queries
  ObjGroup<Test> og = Record::find<Test>(Q("number", between(30,50)) && Q("boolean", true) && Q("dob", greaterthan(DateTime(5,1,1984))));
  assert(og.size() == 1 && og[0].id == t.id, "performing find query");
  og = Record::find<Test>(Q("number", nequals(40)));
  assert(og.size() == 0, "performing find query with no results");
  og = Record::find<Test>(Q("dob", DateTime(5, 29, 1984)));
  assert(og.size() == 1, "find query with exact DateTime value");

  TestTwo tt;
  tt.set("number", 10);
  tt.save();
  t.setOne<TestTwo>(tt);
  t.save();

  // test reverse relationship
  Test q(t.id);
  ObjGroup<TestTwo> ogtt = q.getMany<TestTwo>();
  assert(ogtt.size() == 1 && ogtt[0].id == t.id, "testing reverse relationship between objects");

  // test complicated query
  Test o;
  o.set("stringone", "one");
  o.set("stringtwo", "two");
  o.set("integer", 40);
  o.save();

  Test oo;
  oo.set("stringone", "another");
  oo.set("stringtwo", "andanother");
  oo.set("integer", 50);
  oo.save();  

  og = Record::find<Test>((Q("stringone", startswith("ano")) || Q("stringone", endswith("ne"))) && Q("integer", between(39, 51)));
  assert(og.size() == 2, "testing a complicated query");

  //test lots and lots of relationships
  Test popular;
  ObjGroup<TestTwo> friends;
  int number = 50;
  for(int i=0; i<number; i++) {
    TestTwo unpop_friend;
    unpop_friend.save();
    friends << unpop_friend;
  }
  popular.setMany<TestTwo>(friends);
  popular.save();

  ogtt = popular.getMany<TestTwo>();
  assert(ogtt.size() == number, "testing object relationship with lots of objects");

  TestThree tthree;
  tthree.set("one", "one");
  tthree.set("two", 2);
  tthree.save();
  TestThree another_tthree;
  another_tthree.set("one", 1);
  another_tthree.save();
  ObjGroup<TestThree> ogtthree = Record::find<TestThree>(Q("one", nisnull()));
  assert(ogtthree.size() == 2, "testing nisnull()");

  ogtthree = Record::find<TestThree>(Q("two", isnull()));
  assert(ogtthree.size() == 1 && ogtthree[0].id == another_tthree.id, "testing isnull()");

  delete Sar_Dbi::dbi;
  cout << "No errors were found.\n";
  return 0;
}
