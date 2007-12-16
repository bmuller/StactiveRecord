#include "stactive_record.h"
using namespace stactiverecord;
using namespace std;
Sar_Dbi * Sar_Dbi::dbi;
/*
class Person : public Record {
public:
  std::string firstname;
  std::string lastname;
  int age;
  bool iscool;
  DateTime dob;
  Person() : Record("person") { init(); };
  Person(int id) : Record("person", id) { init(); }
  void init() {
    get("firstname", firstname, "Unknown");
    get("lastname", lastname, "Unkown");
    get("age", age, 0);
    get("iscool", iscool, false);
    get("dob", dob, DateTime(1,1,2000));
  };
  void save() {
    set("firstname", firstname);
    set("lastname", lastname);
    set("age", age);
    set("iscool", iscool);
    set("dob", dob);
    Record::save();
  };
  void setBuddy(Person p) { 
    setOne<Person>(p);
  };
  void getBuddy(Person &p) {
    getOne<Person>(p);
  };
};

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
*/
int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: ./db_test <scheme://[user[:password]@host[:port]/]database>\n";
    return 1;
  }
  Sar_Dbi::dbi = Sar_Dbi::makeStorage(std::string(argv[1]));
  /*
  Person p;
  p.age = 23;
  p.firstname = "bob";
  //p.iscool = false;
  p.save();
  return 0;
  Person q;
  q.setBuddy(p);
  q.save();

  Person qq(q.id);
  Person z;
  qq.getBuddy(z);
  std::cout << z.firstname << "\n";
  */
  /*
  Person qq(q.id);
  Person z;
  qq.getOne<Person>(z);
  std::cout << z.firstname << "\n";
  */

  /*
  Test t;
  t.set("name", "bob");
  //t.set("fname", "whoops");
  t.set("age", 70);
  t.save();

  TestTwo tt;
  //tt.set("name", "fred");
  tt.set("age", 55);
  tt.set<Test>(t);
  tt.save();

  //  ObjGroup<Test> og = Record::find<Test>(Q("age", between(40, 56)) && Q("name", "fred"));
  //ObjGroup<Test> og = Record::find<Test>(Q("age", nbetween(40, 56)) || Q("name", "fred"));
  ObjGroup<TestTwo> og = Record::find<TestTwo>(Q("age", between(40, 56)) && Q(hasobject(t)));

  //ObjGroup<Test> og = Record::find<Test>(Q("name", "fred") || (Q("name", "bob") && Q("fname", "whoops")));
  for(unsigned int i=0; i < og.size(); i++)
    std::cout << og[i].id << "\n"; 

    
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
