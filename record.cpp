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
      _db->get(id, classname, ivalues);
      _db->get(id, classname, rvalues);
    }
  };

  bool Record::operator==(const Record& other) const {
    return (this->id == other.id && this->classname == other.classname);
  };

  bool Record::operator!=(const Record &other) const {
    return !(*this == other);
  }

  void Record::save() {
    // only save if a value was changed, or if this object has never 
    // been saved before (if the id is -1)
    if(dirty || id == -1) {
      if(id == -1) 
	id = _db->next_id(classname);

      SarVector<string> propkeys;
      SarMap<string> spropvalues;
      SarMap<int> ipropvalues;

      // new strings
      get_new(propkeys, STRING);
      svalues.submap(propkeys, spropvalues);
      _db->set(id, classname, spropvalues, true);

      // changed strings
      propkeys.clear();
      get_changed(propkeys, STRING);
      svalues.submap(propkeys, spropvalues);
      _db->set(id, classname, spropvalues, false);      

      // deleted strings
      propkeys.clear();
      get_deleted(propkeys, STRING);
      _db->del(id, classname, propkeys, STRING);

      // new ints
      propkeys.clear();
      get_new(propkeys, INTEGER);
      ivalues.submap(propkeys, ipropvalues);
      _db->set(id, classname, ipropvalues, true);

      // changed ints
      propkeys.clear();
      get_changed(propkeys, INTEGER);
      ivalues.submap(propkeys, ipropvalues);
      _db->set(id, classname, ipropvalues, false);      

      // deleted ints
      propkeys.clear();
      get_deleted(propkeys, INTEGER);
      _db->del(id, classname, propkeys, STRING);

      // added / deleted related records
      propkeys.clear();
      get_changed(propkeys, RECORD);
      SarVector<int> related_ids;
      string related_classname;
      for(unsigned int i=0; i<propkeys.size(); i++) {
	related_ids.clear();
	related_classname = propkeys[i];
	_db->get(id, classname, related_classname, related_ids);
	SarVector<int> new_ids = related_ids.get_new(rvalues[related_classname]);
	SarVector<int> deleted_ids = rvalues[related_classname].get_new(related_ids);
	_db->set(id, classname, new_ids, related_classname);
	_db->del(id, classname, deleted_ids, related_classname);	
      }

      clear_registers();
      dirty = false;
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
      clear_other_values(key, STRING);
      register_new(key, STRING);
    }
    svalues[key] = value;
    dirty = true;
  };

  void Record::set(string key, int value) {   
    if(ivalues.has_key(key)) {
      register_change(key, INTEGER);
    } else if(is_registered_deleted(key, INTEGER)) {
      // in this case it's now a modify
      unregister_delete(key, INTEGER);
      register_change(key, INTEGER);
    } else {
      clear_other_values(key, INTEGER);
      register_new(key, INTEGER);
    }
    ivalues[key] = value;
    dirty = true;
  };

  void Record::get(string key, string& value) {
    if(svalues.has_key(key))
      value = svalues[key];
    else throw Sar_NoSuchPropertyException("property \"" + key + "\" does not exist");
  };

  int Record::get(string key) {
    if(ivalues.has_key(key))
      return ivalues[key];
    throw Sar_NoSuchPropertyException("property \"" + key + "\" does not exist");
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

    switch(ct) {
      case INTEGER:
	ivalues.remove(key);
	break;
      case STRING:
	svalues.remove(key);
	break;
    };
  };

  // Return coltype if found, NONE if not found
  coltype Record::get_col_type(string colname) {
    if(svalues.has_key(colname) || is_registered_new(colname, STRING))
      return STRING;
    if(ivalues.has_key(colname) || is_registered_new(colname, INTEGER))
      return INTEGER;
    return NONE;
  };

  void Record::del() {
    if(id != -1)
      _db->delete_record(id, classname);
  };

  // Delete any previous values for key that isn't coltype ct
  // this is done to prevent two concurrent types for any key value
  coltype Record::clear_other_values(string colname, coltype ct) {
    coltype existing_ct = get_col_type(colname);
    if(existing_ct != NONE && existing_ct != ct) {
      string coltypename;
      coltype_to_name(existing_ct, coltypename);
      debug("getting rid of old value for " + colname + " and type " + coltypename);
      if(is_registered_new(colname, existing_ct)) {
	unregister_new(colname, ct);
      } else if(is_registered_changed(colname, existing_ct)) {
	unregister_change(colname, existing_ct);
      }
      register_delete(colname, existing_ct);
      switch(ct) {
        case INTEGER:
	  ivalues.remove(colname);
	  break;
        case STRING:
	  svalues.remove(colname);
	  break;
      };
    }
  };

  SarVector<Record> Record::makeContainer() { return ObjGroup(); };
  SarVector<Record> Record::makeContainer(SarVector<Record> sr) { return ObjGroup(sr); };
};
