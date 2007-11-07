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
    void remove(T value) {
      // terribly inefficient ... fix this later
      for(unsigned int i=0; i < this->size(); i++) {
	if(this->at(i) == value) {
	  this->erase(this->begin() + i + 1);
	  remove(value);
	}
      }
    };
    // return a vector containing any new values in others
    SarVector<T> get_new(SarVector<T> others) {
      SarVector<T> newones;
      for(unsigned int i=0; i < others.size(); i++)
	if(!this->includes(others[i]))
	  newones << others[i];
      return newones;
    };
  };
  
  template <class T>
  class SarMap : public map<string,T> {
  public:
    SarMap() : map<string,T>() {};
    bool has_key(string key) {
      for(typename SarMap<T>::iterator i=this->begin(); i!=this->end(); ++i) 
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
      // clear the submap first...
      submap.clear();
      for(unsigned int i=0; i < keys.size(); i++)
	submap[keys[i]] = this->operator[](keys[i]);
    };
    void remove(string key) {
      for(typename SarMap<T>::iterator i=this->begin(); i!=this->end(); ++i) {
	if(string((*i).first) == key) 
	  this->erase(i);
      }
    };
  };

};
