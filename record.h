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

    void set(string key, string value);
    void set(string key, int value);
    void get(string key, string& value);
    int get(string key);
    void get(string key, Record& value);
    void del(string key);
    void del();
    SarVector<Record> makeContainer();
    SarVector<Record> makeContainer(SarVector<Record> sr);

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
    template <class T> void setMany(SarVector<Record> og) {
      for(unsigned int i=0; i<og.size(); i++)
	set<T>(og[i]);
    };

    template <class T> void get(T& record) {
      string related_classname = T().classname;
      if(rvalues.has_key(related_classname) && rvalues[related_classname].size() > 0) {
	record = T(rvalues[related_classname][0]);
      } else throw Sar_RecordNotFoundException("Could not find related record \"" + related_classname + "\"");
    };

    template <class T> SarVector<Record> getMany() {
      string related_classname = T().classname;
      if(rvalues.has_key(related_classname)) {
	SarVector<Record> og = makeContainer();
	for(unsigned int i=0; i<rvalues[related_classname].size(); i++)
	  og << T(rvalues[related_classname][i]);
	return og;
      } else throw Sar_RecordNotFoundException("Could not find related records \"" + related_classname + "\"");
    };

  };

};
