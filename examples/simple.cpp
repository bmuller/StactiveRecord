#include "stactiverecord/stactive_record.h"
#include <iostream>
using namespace stactiverecord;
using namespace std;

/**
   This is a simple example for saving/finding a simple object.
**/


Sar_Dbi * Sar_Dbi::dbi;

class Person : public Record<Person> {
public:
  SAR_INIT();
  int age;
  string fullname;
  Person() : Record<Person>() { 
    age = 0; 
    fullname = "Unknown"; 
  };
  Person(int id) : Record<Person>(id) { 
    get("age", age, 0); 
    get("fullname", fullname, "Unknown"); 
  };
  void save() { 
    set("age", age); 
    set("fullname", fullname); 
    Record<Person>::save(); 
  };
  void sayage() { 
    std::cout << "I'm " << fullname << " and I'm " << age << " years old.\n"; 
  };
};
SAR_SET_CLASSNAME(Person, "Person");

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: ./simple <scheme://[user[:password]@host[:port]/]database>\n";
    return 1;
  }
  Sar_Dbi::dbi = Sar_Dbi::makeStorage(std::string(argv[1]));

  Person bob;
  bob.fullname = "Robert Somethingorother";
  bob.age = 50;
  bob.save();

  ObjGroup<Person> people = Person::find(Q("age", between(40, 100)) && Q("fullname", startswith("Robert")));
  people[0].sayage();
  cout << "Name is: " << people[0].fullname << "\n";
  delete Sar_Dbi::dbi; // only cleanup necessary
  return 0;
};
