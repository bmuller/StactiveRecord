#include "stactiverecord/stactive_record.h"
using namespace stactiverecord;
using namespace std;
Sar_Dbi * Sar_Dbi::dbi;

/**
   This is an example of a one to one object relationship.  A lock has a single key and a key fits
   a single lock.
**/

class Lock : public Record<Lock> {
public:
  Lock() : Record<Lock>() {};
  Lock(int id) : Record<Lock>(id) {};
  SAR_INIT();
};
SAR_SET_CLASSNAME(Lock, "Lock");

class Key : public Record<Key> {
public:
  Key() : Record<Key>() {};
  Key(int id) : Record<Key>(id) {};
  SAR_INIT();
};
SAR_SET_CLASSNAME(Key, "Key");

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: ./one_to_many <scheme://[user[:password]@host[:port]/]database>\n";
    return 1;
  }
  Sar_Dbi::dbi = Sar_Dbi::makeStorage(std::string(argv[1]));

  Key key;
  key.set("name", "The Key");
  key.save();

  Lock lock;
  lock.set("name", "The Lock");
  lock.setOne<Key>(key);
  lock.save();

  Key dbkey(key.id);
  string key_name, lock_name;
  dbkey.get("name", key_name);
  Lock dblock;
  dbkey.getOne<Lock>(dblock);
  dblock.get("name", lock_name);

  cout << "The lock for " << key_name << " is " << lock_name << "\n";

  delete Sar_Dbi::dbi;
  return 0;
};
