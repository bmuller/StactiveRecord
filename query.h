namespace stactiverecord {
  using namespace std;

  class Q {
  public:
    // make the following private later
    string key;
    string value;
    SarVector<Q> ored;
    SarVector<Q> anded;

    Q(string _key, string _value) : key(_key), value(_value) {};
    Q operator||(Q& other) { 
      ored << other; 
      return (*this);
    };
    Q operator&&(Q& other) { 
      anded << other; 
      return (*this); 
    };   
    void test(SarMap<string> sm) {
      /*
      if(!sm.has_key(key) || sm[key] != value)
	return false;

      for(unsigned int i=0; i<anded.size(); i++)
	if(!anded[i].test(sm))
	  return false;
      */
    };
    void dump() {
      cout << key << ": " << value << "\n";
      if(anded.size() > 0) {
	cout << "\tanded:\n";
	for(unsigned int i=0; i<anded.size(); i++) {
	  cout << "\t";
	  anded[i].dump();
	}
      }
      if(ored.size() > 0) {
	cout << "\tored:\n";
	for(unsigned int i=0; i<ored.size(); i++) {
	  cout << "\t";
	  ored[i].dump();
	}
      }
    };
  };
  
};
