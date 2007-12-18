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

namespace stactiverecord {
/** 
    All objects should extend Record.  See the simple example in the 
    examples folder to get started.
 **/

  template <class Klass>
  class Record : public CUDPropertyRegister {
  private:
    Sar_Dbi *_db;
    bool dirty;
    SarMap<std::string> svalues;
    SarMap<int> ivalues;    
    SarMap< SarVector<int> > rvalues;
    SarMap<DateTime> dtvalues;
    // lazy updates mean we only initialize values if necessary
    bool initial_update;
    // Delete any previous values for key that isn't coltype ct
    // this is done to prevent two concurrent types for any key value
    coltype clear_other_values(std::string colname, coltype ct) {
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
	case DATETIME:
	  dtvalues.remove(colname);
	  break;
	};
      }
    };
  protected:
    Record() : CUDPropertyRegister(), classname(Klass::classname), id(-1), _db(Sar_Dbi::dbi), initial_update(false) { 
      check_classname(classname);
      _db->initialize_tables(classname); 
      dirty = true; 
    };
    Record(int _id) : CUDPropertyRegister(), classname(Klass::classname), id(_id), _db(Sar_Dbi::dbi), initial_update(false) { 
      check_classname(classname);
      _db->initialize_tables(classname); 
      if(!_db->exists(classname, id)) 
	throw Sar_NoSuchObjectException("There is no " + classname + " with given id.");
    };
  public:
    int id;
    std::string classname;

    /** Update properties of an object.  Will overwrite any changes made since the object was created. **/
    void update() {
      if(id == -1) 
	throw Sar_NoSuchObjectException("Cannot update an object with id of -1");
      else if(!_db->exists(classname, id)) 
	throw Sar_NoSuchObjectException("The object id given does not exist.");
      else {
	_db->get(id, classname, svalues);
	_db->get(id, classname, ivalues);
	_db->get(id, classname, rvalues);
	_db->get(id, classname, dtvalues);
	initial_update = true;
      }     
    };

    bool operator==(const Record& other) const {
      return (this->id == other.id && this->classname == other.classname);
    };

    bool operator!=(const Record &other) const {
      return !(*this == other);
    }

    /** Save record changes to DB.  If there are no changes, nothing will be done. */
    void save() {
      // only save if a value was changed, or if this object has never 
      // been saved before (if the id is -1)
      if(dirty || id == -1) {
	if(id == -1) {
	  id = _db->next_id(classname);
	  _db->make_existing(classname, id);
	} else if (!initial_update) {
	  update();
	}
	
	SarVector<std::string> propkeys;
	SarMap<std::string> spropvalues;
	SarMap<int> ipropvalues;
	SarMap<DateTime> dtpropvalues;

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

	// new datetimes
	propkeys.clear();
	get_new(propkeys, DATETIME);
	dtvalues.submap(propkeys, dtpropvalues);
	_db->set(id, classname, dtpropvalues, true);
	
	// changed datetimes
	propkeys.clear();
	get_changed(propkeys, DATETIME);
	dtvalues.submap(propkeys, dtpropvalues);
	_db->set(id, classname, dtpropvalues, false);      
	
	// deleted datetimes
	propkeys.clear();
	get_deleted(propkeys, DATETIME);
	_db->del(id, classname, propkeys, DATETIME);

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

    void set(std::string key, const char * s) { 
      if(!initial_update && id!=-1) update();
      set(key, std::string(s)); 
    };

    void set(std::string key, std::string value) {
      if(!initial_update && id!=-1) update();
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

    void set(std::string key, int value) {   
      if(!initial_update && id!=-1) update();
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

    void set(std::string key, DateTime value) {   
      if(!initial_update && id!=-1) update();
      // no change
      if(dtvalues.has_key(key) && dtvalues[key] == value)
	return;
      
      if(dtvalues.has_key(key)) {
	register_change(key, DATETIME);
      } else if(is_registered_deleted(key, DATETIME)) {
	// in this case it's now a modify
	unregister_delete(key, DATETIME);
	register_change(key, DATETIME);
      } else {
	clear_other_values(key, DATETIME);
	register_new(key, DATETIME);
      }
      dtvalues[key] = value;
      dirty = true;
    };
    
    void set(std::string key, bool value) {
      if(!initial_update && id!=-1) update();
      set(key, ((value) ? 1 : 0));
    };

    void get(std::string key, std::string& value, std::string alt) {
      if(!initial_update && id!=-1) update();
      if(svalues.has_key(key))
	value = svalues[key];
      else 
	value = alt;
    };

    void get(std::string key, std::string& value) {
      if(!initial_update && id!=-1) update();
      if(svalues.has_key(key))
	value = svalues[key];
      else throw Sar_NoSuchPropertyException("property \"" + key + "\" does not exist");
    };

    void get(std::string key, int& value, int alt) {
      if(!initial_update && id!=-1) update();
      if(ivalues.has_key(key))
	value = ivalues[key];
      else
	value = alt;
    };

    void get(std::string key, int& value) {
      if(!initial_update && id!=-1) update();
      if(ivalues.has_key(key))
	value = ivalues[key];
      else throw Sar_NoSuchPropertyException("property \"" + key + "\" does not exist");
    };
    
    void get(std::string key, bool& value, bool alt) {
      if(!initial_update && id!=-1) update();
      if(ivalues.has_key(key))
	value = (ivalues[key]==0) ? false : true;
      else
	value = alt;
    };

    void get(std::string key, bool& value) {
      if(!initial_update) update();
      if(ivalues.has_key(key))
	value = (ivalues[key]==0) ? false : true;
      else throw Sar_NoSuchPropertyException("property \"" + key + "\" does not exist");
    };  
    
    void get(std::string key, DateTime& value, DateTime alt) {
      if(!initial_update && id!=-1) update();
      if(dtvalues.has_key(key))
	value = dtvalues[key];
      else
	value = alt;
    };

    void get(std::string key, DateTime& value) {
      if(!initial_update && id!=-1) update();
      if(dtvalues.has_key(key))
	value = dtvalues[key];
      else throw Sar_NoSuchPropertyException("property \"" + key + "\" does not exist");
    };  
    
    void del(std::string key) {
      if(!initial_update && id!=-1) update();
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
      case DATETIME:
	dtvalues.remove(key);
	break;
      };
    };

    bool isset(std::string colname) {
      if(!initial_update && id!=-1) update();
      return (type(colname) == NONE);
    };

    template <class T> bool isset() {
      ObjGroup<T> others = getMany<T>();
      return others.size() > 0;
    };

    /** Determine the column type for the given column. 
	Return coltype if found, NONE if not found
    **/
    coltype type(std::string colname) {
      if(!initial_update && id!=-1) update();
      if(svalues.has_key(colname) || is_registered_new(colname, STRING))
	return STRING;
      if(ivalues.has_key(colname) || is_registered_new(colname, INTEGER))
	return INTEGER;
      if(dtvalues.has_key(colname) || is_registered_new(colname, DATETIME))
	return DATETIME;
      return NONE;
    };

    void del() {
      if(!initial_update && id!=-1) update();
      if(id != -1)
	_db->delete_record(id, classname);
    };

    bool has_been_saved() {
      return id != -1;
    };

    /** Assuming here that r is of type T, set record relationship (one->one) */
    template <class T> void setOne(Record<T>& r) {
      if(!initial_update && id!=-1) update();
      if(r.id == -1 || !T::exists(r.id)) {
	std::string msg = "You cannot set an object relation with an object ";
	msg += "that either has not yet been saved or has been deleted.";
	throw Sar_NoSuchObjectException(msg);
      }

      std::string key = r.classname;
      if(!rvalues.has_key(key)) 
	rvalues[key] = SarVector<int>();

      // if the following is not true, then no change is being made
      if(!rvalues[key].includes(r.id)) {
	rvalues[key] << r.id;
	register_change(key, RECORD);
	dirty = true;
      }
    };

    /** Assuming here that each member of og is of type T, set record relationship (one->many) */
    template <class T> void setMany(ObjGroup<T> og) {
      if(!initial_update && id!=-1) update();
      // Set each one individuall, and make list of ids
      SarVector<int> og_ids;
      for(unsigned int i=0; i<og.size(); i++) {
	setOne<T>(og[i]);
	og_ids << og[i].id;
      }

      // Determine which should be removed, and remove them
      std::string classname = T::classname;
      SarVector<int> to_delete = og_ids.get_new(rvalues[classname]);
      for(unsigned int i=0; i<to_delete.size(); i++) {
	rvalues[classname].remove(to_delete[i]);
      }
      if(rvalues.size() > 0)
	register_change(classname, RECORD);
    };

    /** Delete related record (one->one) */
    template <class T> void del() {
      if(!initial_update && id!=-1) update();
      std::string related_classname = T::classname;
      if(rvalues.has_key(related_classname) && rvalues[related_classname].size() > 0) {
	rvalues[related_classname] = SarVector<int>();
	register_change(related_classname, RECORD);
	dirty = true;
      }
    };

    /** Get related record (one->one) */
    template <class T> void getOne(T& record) {
      if(!initial_update && id!=-1) update();
      std::string related_classname = T::classname;
      if(rvalues.has_key(related_classname) && rvalues[related_classname].size() > 0) {
	record = T(rvalues[related_classname][0]);
      } else throw Sar_RecordNotFoundException("Could not find related record \"" + related_classname + "\"");
    };

    /** Get related records (one->many) */
    template <class T> ObjGroup<T> getMany() {
      if(!initial_update && id!=-1) update();
      std::string related_classname = T::classname;
      if(rvalues.has_key(related_classname))
	return ObjGroup<T>(rvalues[related_classname]);
      return ObjGroup<T>();
    };

    /** Find all objects of type T that match a given query Q */
    static ObjGroup<Klass> find(Q query) {
      SarVector<int> results = query.test(Klass::classname);
      // free pointers in query
      query.free();
      return ObjGroup<Klass>(results);      
    };

    /** Get all objects of type T */
    static ObjGroup<Klass> all() {
      SarVector<int> results;
      Sar_Dbi::dbi->get(Klass::classname, results);
      return ObjGroup<Klass>(results);
    }

    static bool exists(int id) {
      return Sar_Dbi::dbi->exists(Klass::classname, id);      
    };

    /** Delete all records of type T */
    static void delete_all() {
      Sar_Dbi::dbi->delete_records(Klass::classname);
    };
  };

};
