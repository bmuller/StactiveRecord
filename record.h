namespace stactiverecord {
  using namespace std;

  class Record {
  private:
    Sar_Dbi *_db;
    string classname;
    bool dirty;
    SarMap<string> svalues;
    SarVector<string> modified_svalues;
    SarVector<string> new_svalues;
    SarMap<int> ivalues;    
    SarVector<string> modified_ivalues;
    SarVector<string> new_ivalues;
  protected:
    Record(string _classname) : classname(_classname), id(-1), _db(Sar_Dbi::dbi) { 
      check_classname(classname);
      _db->initialize_tables(classname); 
      dirty = true; 
    };
    Record(string _classname, int _id) : classname(_classname), id(_id), _db(Sar_Dbi::dbi) { 
      check_classname(classname);
      _db->initialize_tables(classname); 
      update(); 
    };
    void update();
  public:
    int id;

    void save();

    void set(string key, string value);
    void set(string key, int value);
    void set(string key, Record r);

    void get(string key, string& value);
    int get(string key);
    void get(string key, Record& value);
  };

};
