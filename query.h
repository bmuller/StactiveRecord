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
    void operator||(Q other) { ored << other; };
    void operator&&(Q other) { anded << other; };   
    void test(SarMap<string> sm) {
      /*
      if(!sm.has_key(key) || sm[key] != value)
	return false;

      for(unsigned int i=0; i<anded.size(); i++)
	if(!anded[i].test(sm))
	  return false;
      */
    };
  };

};
