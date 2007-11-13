namespace stactiverecord {
  using namespace std;

  class Q {
  private:
    string key;
    string value;
    int ivalue;
    SarVector<Q> ored;
    SarVector<Q> anded;
    coltype ct;
    Where *where;
    bool is_a_where;
  public:
    Q(string _key, Where *_w) : key(_key) { ct = _w->ct; where = _w; is_a_where = true; };
    Q(string _key, string _value) : key(_key), value(_value), is_a_where(false) { ct = STRING; };
    Q(string _key, int _ivalue) : key(_key), ivalue(_ivalue), is_a_where(false) { ct = INTEGER; };
    Q & operator||(Q other);
    Q & operator&&(Q other);
    ~Q() { if(is_a_where) delete where; };
    SarVector<int> test(string classname, Sar_Dbi *sar_dbi);
    bool test(SarMap<string> sm);
    void dump();
  };
  
};
