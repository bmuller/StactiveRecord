#include "stactive_record.h"
using namespace stactiverecord;
using namespace std;
Sar_Dbi * Sar_Dbi::dbi;

// forward declaration of Reader
class Reader;

class Book : public Record {
public:
  string name;
  ObjGroup<Reader> readers;
  Book() : Record("book") { init(); };
  Book(int id) : Record("book", id) { init(); };
  void init() {
    get("name", name, "Unknown"); 
    readers = getMany<Reader>();
  };
  void save() {
    set("name", name);
    setMany<Reader>(readers);
    Record::save();
  };
};

class Reader : public Record {
public:
  string name;
  ObjGroup<Book> books;
  Reader() : Record("reader") { init();  };
  Reader(int id) : Record("reader", id) { init(); };
  void init() {
    get("name", name, "Unknown"); 
    books = getMany<Book>();
  };
  void save() {
    set("name", name);
    setMany<Book>(books);
    Record::save();
  };
};

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: ./many_to_many <scheme://[user[:password]@host[:port]/]database>\n";
    return 1;
  }
  Sar_Dbi::dbi = Sar_Dbi::makeStorage(std::string(argv[1]));

  return 0;

  Reader bob;
  bob.name = "Robert";

  Book tale;
  tale.name = "A Tale of Two Cities";
  tale.save();

  bob.books << tale;
  bob.save();

  Reader new_bob(bob.id);
  cout << new_bob.name << "'s favorite book is: " << new_bob.books[0].name << "\n";
  
  tale.update();
  cout << "The reader for " << tale.name << " is " << tale.readers[0].name << "\n";

  delete Sar_Dbi::dbi;
  return 0;
};
