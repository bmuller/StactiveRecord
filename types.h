namespace stactiverecord {
  using namespace std;
  
  template <class T>
  class SarMap : public map<string,T,mapStrCmp> {
  public:
    SarMap() : map<string,T,mapStrCmp>() {};
    bool has_key(string key) {
      for(SarMap<string>::iterator i=this->begin(); i!=this->end(); ++i) 
	if(string((*i).first) == key)
	  return true;
      return false;
    };
  };

  template <class T>
  class SarVector : public vector<T> {
  public:
    SarVector() : vector<T>() {};
    bool includes(T value) {
      for(unsigned int i=0; i < this.size(); i++)
	if(this[i] == value)
	  return true;
      return false;
    };
  };

};
