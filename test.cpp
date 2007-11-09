#include "stactive_record.h"

using namespace stactiverecord;

Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("sqlite://./sqlitedb.db");

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

int main() {
  Test t;
  t.set("name", "bob");
  t.save();

  Test tt;
  tt.set("name", "fred");
  tt.save();

  Q q("name", "fred");
  ObjGroup<Test> og = Record::find<Test>(q);
  cout << og[0].id << "\n";
  /*
  Test ttt;
  ttt.set("name", "asdf");
  ttt.save();

  ObjGroup<Test> og = Record::find_by<Test>("name", "fred");
  og.get_ids().dump();

  ObjGroup<Test> og;
  og << t;
  og << tt;

  TestTwo f;
  f.setMany<Test>(og);
  f.save();

  ObjGroup<Test> secondog;
  secondog << t;
  Test x;
  x.set("name", "yourmamma");
  x.save();
  secondog << x;

  f.setMany<Test>(secondog);
  f.save();

  TestTwo ff(f.id);
  ObjGroup<Test> oogg = ff.getMany<Test>();
  string n1, n2;
  oogg[0].get("name", n1);
  oogg[1].get("name", n2);
  cout << n1 << " " << n2 << "\n";

  ff.del<Test>();
  ff.save();
  
  Test f;
  f.set("one", "blah");
  f.save();

  f.set("one", 1);  
  f.save();
  f.save();
  f.save();

  f.del();
  */
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
