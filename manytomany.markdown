---
layout: base
title: StactiveRecord - C++ ORM
---
# Many to Many
This is an example of a many to many relationship.  The file is included in the examples directory.

{% highlight cpp %}
#include "stactiverecord/stactive_record.h"
using namespace stactiverecord;
using namespace std;
Sar_Dbi * Sar_Dbi::dbi;

/**
   This is an example of a many to many object relationship.  Books have many readers, and readers 
   have many books.  The example shows that you can do something like "book.readers[0].books" to 
   get all the books some reader likes in addition to some given book.  The class definitions are 
   more complicated than they need to be - this is just a typical situation.
**/

// forward declaration of Reader.
class Reader;

class Book : public Record<Book> {
public:
  SAR_INIT();
  string name;
  ObjGroup<Reader> readers;
  Book() : Record<Book>() { init(); };
  Book(int id) : Record<Book>(id) { init(); };
  void init() {
    get("name", name, "Unknown"); 
    readers = getMany<Reader>();
  };
  void save() {
    set("name", name);
    setMany<Reader>(readers);
    Record<Book>::save();
  };
  void update() {
    Record<Book>::update();
    init();
  };
};
SAR_SET_CLASSNAME(Book, "Book");

class Reader : public Record<Reader> {
public:
  SAR_INIT();
  string name;
  ObjGroup<Book> books;
  Reader() : Record<Reader>() { init();  };
  Reader(int id) : Record<Reader>(id) { init(); };
  void init() {
    get("name", name, "Unknown"); 
    books = getMany<Book>();
  };
  void save() {
    set("name", name);
    setMany<Book>(books);
    Record<Reader>::save();
  };
  void update() {
    Record<Reader>::update();
    init();
  };
};
SAR_SET_CLASSNAME(Reader, "Reader");


int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: ./many_to_many <scheme://[user[:password]@host[:port]/]database>\n";
    return 1;
  }
  Sar_Dbi::dbi = Sar_Dbi::makeStorage(std::string(argv[1]));

  Reader bob;
  bob.name = "Robert";

  Book tale;
  tale.name = "A Tale of Two Cities";
  tale.save();

  bob.books << tale;
  bob.save();
  Reader new_bob(bob.id);
  cout << new_bob.name << "'s first book is: " << new_bob.books[0].name << "\n";

  // this tales' readers array should be updated
  tale.update();
  cout << "The reader for " << tale.name << " is " << tale.readers[0].name << "\n";

  delete Sar_Dbi::dbi;
  return 0;
};
{% endhighlight %}

To compile this example (after installing the library), use:
{% highlight bash %}
g++ $(pkg-config libstactiverecord --cflags --libs) many_to_many.cpp many_to_many
{% endhighlight %}
