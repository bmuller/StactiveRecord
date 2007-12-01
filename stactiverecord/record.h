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

  class Record : public CUDPropertyRegister {
  private:
    Sar_Dbi *_db;
    bool dirty;
    SarMap<std::string> svalues;
    SarMap<int> ivalues;    
    SarMap< SarVector<int> > rvalues;
    coltype clear_other_values(std::string colname, coltype ct);
  protected:
    Record(std::string _classname) : CUDPropertyRegister(), classname(_classname), id(-1), _db(Sar_Dbi::dbi) { 
      check_classname(classname);
      _db->initialize_tables(classname); 
      dirty = true; 
    };
    Record(std::string _classname, int _id) : CUDPropertyRegister(), classname(_classname), id(_id), _db(Sar_Dbi::dbi) { 
      check_classname(classname);
      _db->initialize_tables(classname); 
      // if id doesn't exist error will be thrown in update
      update(); 
    };
    void update();
  public:
    int id;
    std::string classname;
    
    /** Save record changes to DB.  If there are no changes, nothing will be done. */
    void save();

    /** Determine the column type for the given column. */
    coltype type(std::string colname);
    
    /** Determine whether or not any value is set for the given column. */
    bool isset(std::string colname);

    /** Comparison between two records - will look at classname and id */
    bool operator==(const Record& other) const;

    /** Comparison between two records - will look at classname and id */
    bool operator!=(const Record& other) const;

    /** Set a property with a value */
    void set(std::string key, std::string value);

    /** Set a property with a value */
    void set(std::string key, int value);

    /** Set a property with a value */
    void set(std::string key, bool value);

    /** Get a property's value throw exception if not set*/
    void get(std::string key, std::string& value);

    /** Get a property's value, set to default if not yet set */
    void get(std::string key, std::string& value, std::string alt);

    /** Get a property's value, throw exception if not set */
    void get(std::string key, int& value);

    /** Get a property's value, set to default if not yet set */
    void get(std::string key, int& value, int alt);

    /** Get a property's value, throw exception if not set */
    void get(std::string key, bool& value);

    /** Get a property's value, set to default if not yet set */
    void get(std::string key, bool& value, bool alt);

    /** Delete a property */
    void del(std::string key);

    /** Delete a record */
    void del();

    template<class T> static bool exists(int id) {
      std::string classname = T().classname;
      return Sar_Dbi::dbi->exists(classname, id);      
    };

    /** Delete all records of type T */
    template <class T> static void delete_all() {
      std::string classname = T().classname;
      Sar_Dbi::dbi->delete_records(classname);
    };

    /** Assuming here that r is of type T, set record relationship (one->one) */
    template <class T> void setOne(Record r) {
      if(r.id == -1 || !exists<T>(r.id)) {
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
      // Set each one individuall, and make list of ids
      SarVector<int> og_ids;
      for(unsigned int i=0; i<og.size(); i++) {
	setOne<T>(og[i]);
	og_ids << og[i].id;
      }

      // Determine which should be removed, and remove them
      std::string classname = T().classname;
      SarVector<int> to_delete = og_ids.get_new(rvalues[classname]);
      for(unsigned int i=0; i<to_delete.size(); i++) {
	rvalues[classname].remove(to_delete[i]);
      }
      rvalues[classname].dump();
      if(rvalues.size() > 0)
	register_change(classname, RECORD);
    };

    /** Delete related record (one->one) */
    template <class T> void del() {
      std::string related_classname = T().classname;
      if(rvalues.has_key(related_classname) && rvalues[related_classname].size() > 0) {
	rvalues[related_classname] = SarVector<int>();
	register_change(related_classname, RECORD);
	dirty = true;
      }
    };

    /** Get related record (one->one) */
    template <class T> void getOne(T& record) {
      std::string related_classname = T().classname;
      if(rvalues.has_key(related_classname) && rvalues[related_classname].size() > 0) {
	record = T(rvalues[related_classname][0]);
      } else throw Sar_RecordNotFoundException("Could not find related record \"" + related_classname + "\"");
    };

    /** Get related records (one->many) */
    template <class T> ObjGroup<T> getMany() {
      ObjGroup<T> og;
      std::string related_classname = T().classname;
      if(rvalues.has_key(related_classname)) {
	for(unsigned int i=0; i<rvalues[related_classname].size(); i++)
	  og << T(rvalues[related_classname][i]);
      } else throw Sar_RecordNotFoundException("Could not find related records \"" + related_classname + "\"");
      return og;
    };

    /** Find all objects of type T that match a given query Q */
    template <class T> static ObjGroup<T> find(Q query) {
      std::string classname = T().classname;
      SarVector<int> results = query.test(classname);
      // free pointers in query
      query.free();
      return ObjGroup<T>::from_ids(results);      
    };

    /** Get all objects of type T */
    template <class T> static ObjGroup<T> all() {
      std::string classname = T().classname;
      SarVector<int> results;
      Sar_Dbi::dbi->get(classname, results);
      return ObjGroup<T>::from_ids(results);
    }

  };

};
