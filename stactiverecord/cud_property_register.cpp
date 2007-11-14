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

#include "stactive_record.h"

namespace stactiverecord {

  void CUDPropertyRegister::register_new(std::string key, coltype ct) {
    if(!newprop[ct].includes(key))
      newprop[ct].push_back(key);
  };

  void CUDPropertyRegister::register_change(std::string key, coltype ct) {
    if(!changedprop[ct].includes(key))
      changedprop[ct].push_back(key);
  };

  void CUDPropertyRegister::register_delete(std::string key, coltype ct) {
    if(!deletedprop[ct].includes(key))
      deletedprop[ct].push_back(key);
  };

  void CUDPropertyRegister::get_new(SarVector<std::string>& v, coltype ct) {
    for(unsigned int i=0; i < newprop[ct].size(); i++)
      v.push_back(newprop[ct][i]);
  };

  void CUDPropertyRegister::get_changed(SarVector<std::string>& v, coltype ct) {
    for(unsigned int i=0; i < changedprop[ct].size(); i++)
      v.push_back(changedprop[ct][i]);
  };

  void CUDPropertyRegister::get_deleted(SarVector<std::string>& v, coltype ct) {
    for(unsigned int i=0; i < deletedprop[ct].size(); i++)
      v.push_back(deletedprop[ct][i]);
  };

  bool CUDPropertyRegister::is_registered_new(std::string key, coltype ct) {
    return newprop[ct].includes(key);
  };

  bool CUDPropertyRegister::is_registered_changed(std::string key, coltype ct) {
    return changedprop[ct].includes(key);
  };

  bool CUDPropertyRegister::is_registered_deleted(std::string key, coltype ct) {
    return deletedprop[ct].includes(key);
  };

  void CUDPropertyRegister::unregister_new(std::string key, coltype ct) {
    newprop[ct].remove(key);
  };

  void CUDPropertyRegister::unregister_change(std::string key, coltype ct) {
    changedprop[ct].remove(key);
  };

  void CUDPropertyRegister::unregister_delete(std::string key, coltype ct) {
    deletedprop[ct].remove(key);
  };

  void CUDPropertyRegister::clear_registers() {
    std::map<int, SarVector<std::string> >::iterator i;
    for(i=newprop.begin(); i!=newprop.end(); ++i) 
      (*i).second.clear();
    for(i=changedprop.begin(); i!=changedprop.end(); ++i) 
      (*i).second.clear();
    for(i=deletedprop.begin(); i!=deletedprop.end(); ++i) 
      (*i).second.clear();
  };

  void CUDPropertyRegister::dump_registers() {
    std::cout << "New props:\n";
    std::cout << "Strings: ";
    newprop[STRING].dump();
    std::cout << "Integers: ";
    newprop[INTEGER].dump();

    std::cout << "\nChanged props:\n";
    std::cout << "Strings: ";
    changedprop[STRING].dump();
    std::cout << "Integers: ";
    changedprop[INTEGER].dump();

    std::cout << "\nDeleted props:\n";
    std::cout << "Strings: ";
    deletedprop[STRING].dump();
    std::cout << "Integers: ";
    deletedprop[INTEGER].dump();
  };

};
