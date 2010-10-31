---
layout: base
title: StactiveRecord - C++ ORM
---
This is an example of a one to many relationship.  This file is included in the examples directory.
{% highlight cpp %}
#include "stactive_record.h"
using namespace stactiverecord;
using namespace std;
Sar_Dbi * Sar_Dbi::dbi;

/**
   This is an example of a one to many object relationship.  Teachers have many classes, but classes
   only have one teacher.  The class definitions are more complicated than they need to be - this is 
   just a typical situation.
**/

// forward declaration of SchoolClass.
class SchoolClass;

class Teacher : public Record<Teacher> {
public:
  static string classname;
  string name;
  ObjGroup<SchoolClass> classes;
  Teacher() : Record<Teacher>() { init();  };
  Teacher(int id) : Record<Teacher>(id) { init(); };
  void init() {
    get("name", name, "Unknown"); 
    classes = getMany<SchoolClass>();
  };
  void save() {
    set("name", name);
    setMany<SchoolClass>(classes);
    Record<Teacher>::save();
  };
  void update() {
    Record<Teacher>::update();
    init();
  };
};
string Teacher::classname = "teacher";

class SchoolClass : public Record<SchoolClass> {
public:
  static string classname;
  string name;
  Teacher teacher;
  SchoolClass() : Record<SchoolClass>() { init(); };
  SchoolClass(int id) : Record<SchoolClass>(id) { init(); };
  void init() {
    get("name", name, "Unknown"); 
    if(isset<Teacher>())
      getOne<Teacher>(teacher);
  };
  void save() {
    set("name", name);
    // if teacher variable has been set
    if(teacher.has_been_saved())
      setOne<Teacher>(teacher);
    Record<SchoolClass>::save();
  };
  void update() {
    Record<SchoolClass>::update();
    init();
  };
};
string SchoolClass::classname = "schoolclass";


int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: ./one_to_many <scheme://[user[:password]@host[:port]/]database>\n";
    return 1;
  }
  Sar_Dbi::dbi = Sar_Dbi::makeStorage(std::string(argv[1]));

  Teacher krabappel;
  krabappel.name = "Ms. Krabappel";

  SchoolClass english;
  english.name = "English";
  english.save();
  krabappel.classes << english;

  SchoolClass math;
  math.name = "Math";
  math.save();
  krabappel.classes << math;

  krabappel.save();
  cout << krabappel.name << " teaches " << krabappel.classes[0].name << " and " << krabappel.classes[1].name << "\n";

  english.update();
  cout << english.name << " is taught by " << english.teacher.name << "\n";

  math.update();
  cout << math.name << " is taught by " << math.teacher.name << "\n";

  delete Sar_Dbi::dbi;
  return 0;
};
{% endhighlight %}
