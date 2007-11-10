namespace stactiverecord {
  using namespace std;

  class Record : public CUDPropertyRegister {
  private:
    Sar_Dbi *_db;
    string classname;
    bool dirty;
    SarMap<string> svalues;
    SarMap<int> ivalues;    
    SarMap< SarVector<int> > rvalues;
    coltype clear_other_values(string colname, coltype ct);
  protected:
    Record(string _classname) : CUDPropertyRegister(), classname(_classname), id(-1), _db(Sar_Dbi::dbi) { 
      check_classname(classname);
      _db->initialize_tables(classname); 
      dirty = true; 
    };
    Record(string _classname, int _id) : CUDPropertyRegister(), classname(_classname), id(_id), _db(Sar_Dbi::dbi) { 
      check_classname(classname);
      _db->initialize_tables(classname); 
      update(); 
    };
    void update();
  public:
    int id;

    void save();
    coltype get_col_type(string colname);

    bool operator==(const Record& other) const;
    bool operator!=(const Record& other) const;

    void set(string key, string value);
    void set(string key, int value);
    void get(string key, string& value);
    int get(string key);
    void get(string key, Record& value);
    void del(string key);
    void del();

    template <class T> static void delete_all() {
      string classname = T().classname;
      Sar_Dbi::dbi->delete_records(classname);
    };

    // Assuming here that r is of type T
    template <class T> void set(Record r) {
      string key = r.classname;
      if(!rvalues.has_key(key)) 
	rvalues[key] = SarVector<int>();

      // if the following is not true, then no change is being made
      if(!rvalues[key].includes(r.id)) {
	rvalues[key] << r.id;
	register_change(key, RECORD);
	dirty = true;
      }
    };

    // Assuming here that each r is of type T
    template <class T> void setMany(ObjGroup<T> og) {
      // Set each one individuall, and make list of ids
      SarVector<int> og_ids;
      for(unsigned int i=0; i<og.size(); i++) {
	set<T>(og[i]);
	og_ids << og[i].id;
      }

      // Determine which should be removed, and remove them
      string classname = T().classname;
      SarVector<int> to_delete = og_ids.get_new(rvalues[classname]);
      for(unsigned int i=0; i<to_delete.size(); i++) {
	rvalues[classname].remove(to_delete[i]);
      }
      rvalues[classname].dump();
      if(rvalues.size() > 0)
	register_change(classname, RECORD);
    };

    template <class T> void del() {
      string related_classname = T().classname;
      if(rvalues.has_key(related_classname) && rvalues[related_classname].size() > 0) {
	rvalues[related_classname] = SarVector<int>();
	register_change(related_classname, RECORD);
	dirty = true;
      }
    };

    template <class T> void get(T& record) {
      string related_classname = T().classname;
      if(rvalues.has_key(related_classname) && rvalues[related_classname].size() > 0) {
	record = T(rvalues[related_classname][0]);
      } else throw Sar_RecordNotFoundException("Could not find related record \"" + related_classname + "\"");
    };

    template <class T> ObjGroup<T> getMany() {
      ObjGroup<T> og;
      string related_classname = T().classname;
      if(rvalues.has_key(related_classname)) {
	for(unsigned int i=0; i<rvalues[related_classname].size(); i++)
	  og << T(rvalues[related_classname][i]);
      } else throw Sar_RecordNotFoundException("Could not find related records \"" + related_classname + "\"");
      return og;
    };

    // some static search stuff
    template <class T> static ObjGroup<T> find_by(string key, string value) {
      string classname = T().classname;
      SarVector<int> results;
      Sar_Dbi::dbi->get(classname, key, value, results);
      return ObjGroup<T>::from_ids(results);      
    };

    template <class T> static ObjGroup<T> find_by(string key, int value) {
      string classname = T().classname;
      SarVector<int> results;
      Sar_Dbi::dbi->get(classname, key, value, results);
      return ObjGroup<T>::from_ids(results);      
    };

    template <class T> static ObjGroup<T> find(Q query) {
      string classname = T().classname;
      SarVector<int> results = query.test(classname, Sar_Dbi::dbi);
      return ObjGroup<T>::from_ids(results);      
    };

    template <class T> static ObjGroup<T> all() {
      string classname = T().classname;
      SarVector<int> results;
      Sar_Dbi::dbi->get(classname, results);
      return ObjGroup<T>::from_ids(results);
    }

  };

};
