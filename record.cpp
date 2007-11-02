#include  "stactive_record.h"

/*
 * Three things that can be done to variables:
 * add, modify, delete.
 *
 * To add:
 * if exists in delete queue, then this is a modify
 * else add to add queue if not exists
 * 
 * To modify:
 * remove from delete queue
 * add to modify queue if not exists
 * 
 * To delete:
 * remove prop from add queue and mod queue
 * remove prop from 
 * add to delete queue if not exists
 *
 * update process:
 * delete everything in delete queue
 * add everything in add queue
 * modify everything in modify queue
 */

namespace stactiverecord {

  void Record::update() {
    if(id == -1) 
      throw Sar_NoSuchObjectException("Cannot update an object with id of -1");
    else {
      _db->get(id, classname, svalues);
      //_db->get(id, classname, ivalues);
    }
  };

  void Record::save() {
    if(id == -1) {
      id = _db->next_id(classname);
      // everything will now be inserts
      _db->set(id, classname, svalues, true);
    } else if(dirty) {
      SarVector<string> propkeys;
      SarMap<string> propvalues;

      // new strings
      dump_registers();
      get_new(propkeys, STRING);
      svalues.submap(propkeys, propvalues);
      dump_registers();
      _db->set(id, classname, propvalues, true);
      return;
      // changed strings
      propkeys.clear();
      get_changed(propkeys, STRING);
      svalues.submap(propkeys, propvalues);
      _db->set(id, classname, propvalues, false);      

      // deleted strings
      propkeys.clear();
      get_deleted(propkeys, STRING);
      _db->del(id, classname, propkeys, STRING);

      clear_registers();
    }
  };

  void Record::set(string key, string value) { 
    if(svalues.has_key(key)) {
      register_change(key, STRING);
    } else if(is_registered_deleted(key, STRING)) {
      // in this case it's now a modify
      unregister_delete(key, STRING);
      register_change(key, STRING);
    } else {
      register_new(key, STRING);
    }
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

  void Record::del(string key) {
    coltype ct = get_col_type(key);
    if(ct == NONE) return;

    if(is_registered_new(key, ct))
      unregister_new(key, ct);
    else {// preexisting
      unregister_change(key, ct);
      register_delete(key, ct);
    }

    if(svalues.has_key(key)) 
      svalues.remove(key); 
  };

  // Return coltype if found, NONE if not found
  coltype Record::get_col_type(string colname) {
    if(svalues.has_key(colname) || is_registered_new(colname, STRING))
      return STRING;
    //if(ivalues.has_key(colname) || is_registered_new(colname, INTEGER))
    //return INTEGER;
    return NONE;
  };

  void Record::del() {};

};
