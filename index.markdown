---
layout: base
title: StactiveRecord - C++ ORM
---
# Introduction
**StactiveRecord** is a C++ library designed to make simple database use simple.  It was inspired by Ruby on Rail's [Active Record](http://wiki.rubyonrails.org/rails/pages/ActiveRecord), however, no similar look, feel, or performance is guaranteed.  It uses an [Object-relational mapping](http://en.wikipedia.org/wiki/Object-relational_mapping) pattern to represent records as objects.  It also provides persistent (basic) object relationships (one to many, many to many, one to one).

# Quick Example For Those In A Rush 
See the [simple example description page](exampledescription.html) for  more detailed example. 
{% highlight cpp %}
#include <stactive_record.h>
#include <iostream>
using namespace stactiverecord;
using namespace std;

Sar_Dbi * Sar_Dbi::dbi = Sar_Dbi::makeStorage("sqlite://:memory:");
class Person : public Record<Person> {
public:
  static string classname;
  Person() : Record<Person>() {};
  Person(int id) : Record<Person>(id) {};
  void sayhi() {
    cout << "Hello\n";
  };
};
string Person::classname = "person";

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
{% endhighlight %}

# Installation

## Prerequisites 

One or more of the following databases and their dev components:
 * libsqlite: [SQLite C libs](http://www.sqlite.org)
 * mysql c libs: distributed with the mysql client program - [dev.mysql.com](http://dev.mysql.com/downloads/mysql/)
 * postgres c libs: come with PostgreSQL core - [postgresql.org](http://www.postgresql.org/download/)

## Get The Source 
You can download the current code using git:
{% highlight bash %}
git clone git://github.com/bmuller/StactiveRecord.git
{% endhighlight %}

*Note that if you download a development release you will need current versions of the autotools installed, and you must run ./autogen.sh first before following these instructions.*

## Compile 
Enter the StactiveRecord directory and type:
{% highlight bash %}
./configure
{% endhighlight %}
You can use the following to see additional configuration options:
{% highlight bash %}
./configure --help
{% endhighlight %}

Then:
{% highlight bash %}
make
su root
make install
{% endhighlight %}

# Docs
 * There is a [usage page](usage.html) with examples and the API in brief.
 * The full API is available by running doxygen in the root directory.

