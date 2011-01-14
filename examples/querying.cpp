#include "stactive_record.h"
using namespace stactiverecord;
using namespace std;
Sar_Dbi * Sar_Dbi::dbi;

// First define a class that we'll be querying on
class Person : public Record<Person> {
public:
  SAR_INIT();
  Person() : Record<Person>() {};
  Person(int id) : Record<Person>(id) {};
};
SAR_SET_CLASSNAME(Person, "Person");

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: ./db_test <scheme://[user[:password]@host[:port]/]database>\n";
    return 1;
  }
  Sar_Dbi::dbi = Sar_Dbi::makeStorage(std::string(argv[1]));

  Person bob;
  bob.set("name", "Bob");
  bob.set("age", 40);
  bob.save();

  Person fred;
  fred.set("name", "Fred");
  fred.set("age", 35);
  fred.setOne<Person>(bob);
  fred.save();
  
  ObjGroup<Person> people = Person::find(Q("age", between(30, 39)) || Q("name", equals("Bob")));
  cout << "Should be Bob and Fred:\n";
  for(unsigned int i=0; i < people.size(); i++) {
    string name;
    people[i].get("name", name);
    std::cout << name << "\n"; 
  }

  people = Person::find(Q(hasobject(bob)));
  cout << "Should be Fred:\n";
  for(unsigned int i=0; i < people.size(); i++) {
    string name;
    people[i].get("name", name);
    std::cout << name << "\n"; 
  }

  people = Person::find(Q(hasobject(fred)));
  cout << "Should be Bob:\n";
  for(unsigned int i=0; i < people.size(); i++) {
    string name;
    people[i].get("name", name);
    std::cout << name << "\n"; 
  }

  delete Sar_Dbi::dbi;
  return 0;
};
