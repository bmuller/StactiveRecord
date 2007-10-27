namespace stactiverecord {
  using namespace std;

  template <class T>
  class SarVector : public vector<T> {
  public:
    SarVector() : vector<T>() {};
    bool includes(T value) {
      for(unsigned int i=0; i < this->size(); i++)
	if(this->at(i) == value)
	  return true;
      return false;
    };
    void operator<<(T value) {
      this->push_back(value);
    };
    void dump() {
      for(unsigned int i=0; i < this->size(); i++)
        cout << this->at(i) << ", ";
      cout << "\n";
    };
  };
  
  template <class T>
  class SarMap : public map<string,T> {
  public:
    SarMap() : map<string,T>() {};
    bool has_key(string key) {
      for(SarMap<string>::iterator i=this->begin(); i!=this->end(); ++i) 
	if(string((*i).first) == key) {
	  return true;
	}
      return false;
    };
    void dump() {
      for(typename SarMap<T>::iterator i=this->begin(); i!=this->end(); ++i) 
	cout << (*i).first << ": " << (*i).second << "\n";
    };
    void submap(SarVector<string> keys, SarMap<T>& submap) {
      for(unsigned int i=0; i < keys.size(); i++)
	submap[keys[i]] = this->operator[](keys[i]);
    };
  };

};
