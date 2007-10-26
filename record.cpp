#include  "stactive_record.h"

namespace stactiverecord {

  void Record::update() {
    if(id == -1) 
      throw Sar_NoSuchObjectException("Cannot update an object with id of -1");
    else {
      _db->get(id, classname, svalues);
    }
  };

  void Record::save() {
    if(id == -1) {
      id = _db->next_id(classname);
      // everything will now be inserts
      _db->set(id, classname, svalues, true);
    } else if(dirty) {
      // determine what's an insert and what's an update
    }
  };

  void Record::set(string key, string value) {
    svalues[key] = value;
    dirty = true;
  };

  void Record::set(string key, int value) {

    dirty = true;
  };

  void Record::set(string key, Record r) {

    dirty = true;
  };

  void Record::get(string key, string& value) {
    if(svalues.has_key(key))
      value = svalues[key];
    else throw Sar_NoSuchPropertyException("property \"" + key + "\" does not exist");
  };

  int Record::get(string key) {

  };
  
  void Record::get(string key, Record& value) {

  };

};
