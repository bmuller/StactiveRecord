namespace stactiverecord {
  using namespace std;

  struct mapStrCmp {
    bool operator()( string s1, string s2 ) const {
      return s1 == s2;
    }
  };

  // In utils.cpp
  void int_to_string(int i, string& s);
  void debug(string s);
  void check_classname(string classname);
  vector<string> explode(string s, string e);
  void coltype_to_name(coltype ct, string& name);
};
  
