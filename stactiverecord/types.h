/*
Copyright (C) 2007 Butterfat, LLC (http://butterfat.net)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Created by bmuller <bmuller@butterfat.net>
*/

namespace stactiverecord {

  template <class T>
  class SarVector : public std::vector<T> {
  public:
    SarVector() : std::vector<T>() {};
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
	  std::cout << this->at(i) << ", ";
      std::cout << "\n";
    };
    void dumpeach() {
      for(unsigned int i=0; i < this->size(); i++)
	this->at(i).dump();
    };
    void remove(T value) {
      // terribly inefficient ... fix this later
      for(unsigned int i=0; i < this->size(); i++) {
	if(this->at(i) == value) {
	  this->erase(this->begin() + i);
	  remove(value);
	}
      }
    };
    int count(T value) {
      int count = 0;
      for(unsigned int i=0; i < this->size(); i++) 
	if(this->at(i) == value) 
	  count++;
      return count;
    };
    // return a vector containing any new values in others
    SarVector<T> get_new(SarVector<T> others) {
      SarVector<T> newones;
      for(unsigned int i=0; i < others.size(); i++)
	if(!this->includes(others[i]))
	  newones << others[i];
      return newones;
    };
    void unionize(SarVector<T> others) {
      for(unsigned int i=0; i < others.size(); i++)
	if(!this->includes(others[i]))
	  this->push_back(others[i]);
    };
    SarVector<T> intersects(SarVector<T> others) {
      SarVector<T> joined;
      for(unsigned int i=0; i < others.size(); i++)
	if(this->includes(others[i]))
	  joined << others[i];
      return joined;
    };
    void operator+(SarVector<T> others) {
      for(unsigned int i=0; i < others.size(); i++)
	this->push_back(others[i]);      
    };
    void operator-(SarVector<T> others) {
      for(unsigned int i=0; i < others.size(); i++)
	this->remove(others[i]);
    };
    // if order doesn't matter, consider equality
    bool operator==(SarVector<T>& others) {
      if(this->size() != others.size())
	return false;
      for(unsigned int i=0; i < others.size(); i++)
	if(others.count(this->at(i)) != 1)
	  return false;
      return true;
    };
    bool operator!=(SarVector<T>& others) {
      return !(*this == others);
    };
  };
  
  template <class T>
  class SarMap : public std::map<std::string,T> {
  public:
    SarMap() : std::map<std::string,T>() {};
    bool has_key(std::string key) {
      for(typename SarMap<T>::iterator i=this->begin(); i!=this->end(); ++i) 
	if(std::string((*i).first) == key) {
	  return true;
	}
      return false;
    };
    void dump() {
      for(typename SarMap<T>::iterator i=this->begin(); i!=this->end(); ++i) 
	std::cout << (*i).first << ": " << (*i).second << "\n";
    };
    void dumpeach() {
      for(typename SarMap<T>::iterator i=this->begin(); i!=this->end(); ++i) {
	std::cout << (*i).first << ": ";
	((*i).second).dump();
	std::cout << "\n";
      }
    };    
    void submap(SarVector<std::string> keys, SarMap<T>& submap) {
      // clear the submap first...
      submap.clear();
      for(unsigned int i=0; i < keys.size(); i++)
	submap[keys[i]] = this->operator[](keys[i]);
    };
    void remove(std::string key) {
      for(typename SarMap<T>::iterator i=this->begin(); i!=this->end(); ++i) {
	if(std::string((*i).first) == key) 
	  this->erase(i);
      }
    };
    SarVector<std::string> keys() {
      SarVector<std::string> keys;
      for(typename SarMap<T>::iterator i=this->begin(); i!=this->end(); ++i) 
	keys << std::string((*i).first);      
      return keys;
    };
    bool operator==(SarMap<T>& others) {
      if(this->size() != others.size() || this->keys() != others.keys())
	return false;
      std::string key;
      for(typename SarMap<T>::iterator i=this->begin(); i!=this->end(); ++i) {
	key = std::string((*i).first);
	if(this->operator[](key) != others[key]) 
	  return false;
      }
      return true;
    };
    bool operator!=(SarMap<T>& others) const {
      return !(*this == others);
    };
  };

  template <class T>
  class ObjGroup : public SarVector<T> {
  private:
    SarVector<int> ids;
    bool slow_inits;
    void ids_to_objs() {
      for(unsigned int i=0; i < ids.size(); i++)
	this->push_back(T(ids[i]));
      slow_inits = false;
    };
  public:
    ObjGroup() : SarVector<T>(), slow_inits(false) {};
    ObjGroup(SarVector<T>& sr) : SarVector<T>(), slow_inits(false) {
      for(unsigned int i=0; i<sr.size(); i++)
	push_back(sr[i]);
    };
    ObjGroup(SarVector<int> _ids) : SarVector<T>(), slow_inits(true) { ids = _ids; };
    int size() {
      if(slow_inits)
	return ids.size();
      return SarVector<T>::size();
    };
    SarVector<int> get_ids() {
      if(slow_inits)
	return ids;
      SarVector<int> sv;
      for(unsigned int i=0; i < this->size(); i++)
	sv.push_back(this->at(i).id);
      return sv;
    };
    /*
    template <T> SarVector<T> get_property(std::string name) {
      SarVector<T> sv;
      T value;
      for(unsigned int i=0; i < this->size(); i++) {
	this->at(i).get(name, value);
	sv << value;
      }
      return sv;
    };
    */
    bool has_id(int id) {
      return get_ids().includes(id);
    };
    bool has(T r) {
      for(unsigned int i=0; i < this->size(); i++)
	if(this->at(i) == r)
	  return true;
      return false;
    };
    T& operator[](int i) {
      return this->at(i);
    };
    T& at(int i) {
      if(slow_inits) ids_to_objs();
      return SarVector<T>::at(i);
    };
  };
};


