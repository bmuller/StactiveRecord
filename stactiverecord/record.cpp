/*
Copyright (C) 2007 Butterfat, LLC (http://butterfat.net)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Created by bmuller <bmuller@butterfat.net>
*/

#include  "stactive_record.h"

namespace stactiverecord {

  void Record::update() {
    if(id == -1) 
      throw Sar_NoSuchObjectException("Cannot update an object with id of -1");
    else if(!_db->exists(classname, id)) 
      throw Sar_NoSuchObjectException("The object id given does not exist.");
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
      if(id == -1) {
	id = _db->next_id(classname);
	_db->make_existing(classname, id);
      }

      SarVector<std::string> propkeys;
      SarMap<std::string> spropvalues;
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
      std::string related_classname;
      for(unsigned int i=0; i<propkeys.size(); i++) {
	related_ids.clear();
	related_classname = propkeys[i];
	_db->get(id, classname, related_classname, related_ids);
	SarVector<int> new_ids = related_ids.get_new(rvalues[related_classname]);
	SarVector<int> deleted_ids = rvalues[related_classname].get_new(related_ids);
	_db->set(id, classname, new_ids, related_classname);
	_db->del(id, classname, deleted_ids, related_classname);	
      }

      // reset registers and make ourselves clean
      clear_registers();
      dirty = false;
    }
  };

  void Record::set(std::string key, std::string value) {
    debug("setting a string value");
    // no change    
    if(svalues.has_key(key) && svalues[key] == value)
      return;

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

  void Record::set(std::string key, int value) {   
    std::cout << "Setting int value: " << value << "\n";
    // no change
    if(ivalues.has_key(key) && ivalues[key] == value)
      return;

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

  void Record::set(std::string key, bool value) {
    set(key, ((value) ? 1 : 0));
  };

  void Record::get(std::string key, std::string& value, std::string alt) {
    if(svalues.has_key(key))
      value = svalues[key];
    else 
      value = alt;
  };

  void Record::get(std::string key, std::string& value) {
    if(svalues.has_key(key))
      value = svalues[key];
    else throw Sar_NoSuchPropertyException("property \"" + key + "\" does not exist");
  };

  void Record::get(std::string key, int& value, int alt) {
    if(ivalues.has_key(key))
      value = ivalues[key];
    else
      value = alt;
  };

  void Record::get(std::string key, int& value) {
    if(ivalues.has_key(key))
      value = ivalues[key];
    else throw Sar_NoSuchPropertyException("property \"" + key + "\" does not exist");
  };

  void Record::get(std::string key, bool& value, bool alt) {
    if(ivalues.has_key(key))
      value = (ivalues[key]==0) ? false : true;
    else
      value = alt;
  };

  void Record::get(std::string key, bool& value) {
    if(ivalues.has_key(key))
      value = (ivalues[key]==0) ? false : true;
    else throw Sar_NoSuchPropertyException("property \"" + key + "\" does not exist");
  };  

  void Record::del(std::string key) {
    coltype ct = type(key);
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

  bool Record::isset(std::string colname) {
    return (type(colname) == NONE);
  };

  // Return coltype if found, NONE if not found
  coltype Record::type(std::string colname) {
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
  coltype Record::clear_other_values(std::string colname, coltype ct) {
    coltype existing_ct = type(colname);
    if(existing_ct != NONE && existing_ct != ct) {
      std::string coltypename;
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
};
