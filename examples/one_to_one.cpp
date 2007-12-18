#include "stactive_record.h"
using namespace stactiverecord;
using namespace std;
Sar_Dbi * Sar_Dbi::dbi;

/**
   This is an example of a one to one object relationship.  A lock has a single key and a key fits
   a single lock.  The class definitions are more complicated than they need to be - this is 
   just a typical situation.
**/

// forward declaration of Key.
class Key;

class Lock : public Record<Lock> {
public:
  static string classname;
  string name;
  Key * key;
  Lock() : Record<Lock>() { init();  };
  Lock(int id) : Record<Lock>(id) { init(); };
  void init() {
    get("name", name, "Unknown"); 
    if(isset<Key>())
      getOne<Key>(*key);
  };
  void save() {
    set("name", name);
    // if key variable has been set
    if(key->has_been_saved())
      setOne<Key>(*key);
    Record<Lock>::save();
  };
  void update() {
    Record<Lock>::update();
    init();
  };
};
string Lock::classname = "lock";

class Key : public Record<Key> {
public:
  static string classname;
  string name;
  Lock lock;
  Key() : Record<Key>() { init(); };
  Key(int id) : Record<Key>(id) { init(); };
  void init() {
    get("name", name, "Unknown"); 
    if(isset<Lock>())
      getOne<Lock>(lock);
  };
  void save() {
    set("name", name);
    // if teacher variable has been set
    if(lock.has_been_saved())
      setOne<Lock>(lock);
    Record<Key>::save();
  };
  void update() {
    Record<Key>::update();
    init();
  };
};
string Key::classname = "key";


int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: ./one_to_many <scheme://[user[:password]@host[:port]/]database>\n";
    return 1;
  }
  Sar_Dbi::dbi = Sar_Dbi::makeStorage(std::string(argv[1]));

  Key key;
  key.name = "The Key";
  key.save();

  Lock lock;
  lock.name = "The Lock";
  lock.key = key;
  lock.save;

  key.update();
  cout << "The lock for " << key.name << " is " << key.lock.name << "\n";

  delete Sar_Dbi::dbi;
  return 0;
};
