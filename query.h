namespace stactiverecord {
  using namespace std;

  class Q {
  private:
    string key;
    string value;
    SarVector<Q> ored;
    SarVector<Q> anded;
  public:
    Q(string _key, string _value) : key(_key), value(_value) {};
    Q & operator||(Q& other);
    Q & operator&&(Q& other);
    SarVector<int> test(string classname, Sar_Dbi *sar_dbi);
    bool test(SarMap<string> sm);
    void dump();
  };
  
};
