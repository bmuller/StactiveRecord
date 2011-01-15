# Introduction
StactiveRecord is a C++ library designed to make simple database use as simple and streamlined as possible in a static language. It was inspired by Ruby on Rail's Active Record, however, no similar look, feel, or performance is guaranteed. It uses an Object-relational mapping pattern to represent records as objects. It also provides persistent object relationships (one to many, many to many, one to one).

# Installation
## Prerequisites
One or more of the following databases and their dev components:

* [libsqlite](http://www.sqlite.org): SQLite C libs
* [mysql c libs](http://dev.mysql.com/downloads/mysql/): distributed with the mysql client program 
* [postgres c libs](http://www.postgresql.org/download/): come with PostgreSQL core

## Compiling
Stable releases can be found at [http://findingscience.com/StactiveRecord/releases.html](http://findingscience.com/StactiveRecord/releases.html).  Development releases can be fetched using:
    git clone git://github.com/bmuller/StactiveRecord.git

You'll need to have the autotools installed and run ./autogen.sh if you download the development version.

To compile and install:
    ./configure && make && sudo make install

Confiuration options can be found with:
    ./configure --help

## Testing
    export DBCONFIG=sqlite://:memory:
    make test

# Usage
StactiveRecord provides both typical basic ORM functionality as well as a general database agnostic interface for CRUD operations.  As an example of ORM usage:
    #include <stactivrecord/stactive_record.h>
    #include <iostream>
    using namespace stactiverecord;
    using namespace std;

    // Initialize the DB connection
    Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("sqlite://:memory:");

    class Person : public Record<Person> {
    public:
      SAR_INIT();
      Person() : Record<Person>() {};
      Person(int id) : Record<Person>(id) {};
      void sayhi() { cout << "Hello\n"; };
    };
    SAR_SET_CLASSNAME(Person, "Person");

    int main() {
      Person bob;
      bob.set("fullname", "Robert Somethingorother");
      bob.set("age", 50);
      bob.save();

      ObjGroup<Person> people = Person::find(Q("age", between(40, 100)) && Q("fullname", startswith("Robert")));
      people[0].sayhi();
      SarVector<string> fullnames = people.get_property<string>("fullname");
      cout << "Name is: " << fullnames[0] << "\n";
      delete Sar_Dbi::dbi; // only cleanup necessary
      return 0;
    };

Then, to compile your program (assuming you saved it in *simple.cpp*):
    g++ $(pkg-config libstactiverecord --cflags --libs) simple.cpp -o simple

# Documentation
API documentation can be created using the command:
    Doxygen
    firefox api/html/index.html

Further examples (including examples with object relationships) can be found in the examples directory.  Additional documentation is available online at [http://findingscience.com/StactiveRecord](http://findingscience.com/StactiveRecord)