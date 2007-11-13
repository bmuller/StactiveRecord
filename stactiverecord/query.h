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
  public:
    Q(string _key, Where *_w) : key(_key) { ct = _w->ct; where = _w; };
    Q(string _key, string _value) : key(_key), value(_value) { ct = STRING; where = equals(_value); };
    Q(string _key, int _ivalue) : key(_key), ivalue(_ivalue) { ct = INTEGER; where = equals(_ivalue); };
    Q & operator||(Q other);
    Q & operator&&(Q other);
    SarVector<int> test(string classname, Sar_Dbi *sar_dbi);
    bool test(SarMap<string> sm);
    void dump();
    void free();
  };
  
};
