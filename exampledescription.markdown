---
layout: base
title: StactiveRecord - C++ ORM
---
First, the simple example code, then a line by line description:
{% highlight cpp %}
#include <iostream>
using namespace stactiverecord;
using namespace std;

Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("sqlite://:memory:");
class Person : public Record<Person> {
public:
  static string classname;
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
string Person::classname = "person";

int main() {
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
{% endhighlight %}

## Description
First, open the database connection (more information can be found on the [usage page](usage.html):
{% highlight cpp %}
Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("sqlite://:memory:");
{% endhighlight %}

All object classes must extend the base **Record** template.  They must also specify a static public string variable named *classname*.  The *classname* should be the name of the class (it must contain only upper and lowercase letters, no numbers or other characters).  The following is the simplest and object can get:
{% highlight cpp %}
class Person : public Record<Person> {
public:
  static string classname;
  Person() : Record<Person>() {};
  Person(int id) : Record<Person>(id) {};
};
string Person::classname = "person";
{% endhighlight %}

As in the example, there are two public class attributes, *age* and *fullname*.  These are initialized in the two constructors (one for a new object, the other for an existing one identified by its id).  In the constructor that accepts an id, the attributes are initialized to either the values stored in the database for the given object or with a default value (*0* and *Unknown*) if no value was given when the object was saved.
{% highlight cpp %}
  Person() : Record<Person>() { 
    age = 0; 
    fullname = "Unknown"; 
  };
  Person(int id) : Record<Person>(id) { 
    // Give a default value of 0 if age isn't set
    get("age", age, 0); 
    // Give a default value of Unkown if fullname isn't not set
    get("fullname", fullname, "Unknown"); 
  };
{% endhighlight %}

When the object's save method is called, it's age and fullname are first set (using the parent's method *set*), and then the object is saved using the parent's *save* method.  Note that the object's set methods are not protected and can be called outside of the class (say, for instance, if you don't want to have class attribute variables at all).
{% highlight cpp %}
  void save() { 
    set("age", age); 
    set("fullname", fullname); 
    Record<Person>::save(); 
  };
{% endhighlight %}

For instance, the following code:
{% highlight cpp %}
  Person bob;
  bob.fullname = "Robert Somethingorother";
  bob.age = 50;
  bob.save();
{% endhighlight %}
could be implemented using the following instead:
{% highlight cpp %}
  Person bob;
  bob.set("fullname", "Robert Somethingorother");
  bob.set("age", 50);
  bob.save();
{% endhighlight %}
which would eliminate the need for an overwritten *save* method and instance variables.

The following is an example of a complicated query - more examples can be found on the [usage page](usage.html).
{% highlight cpp %}
  ObjGroup<Person> people = Person::find(Q("age", between(40, 100)) && Q("fullname", startswith("Robert")));
{% endhighlight %}
