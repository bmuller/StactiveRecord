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
  /** \class CUDPropertyRegister
   * \brief An observer of sorts - keeps track of changes to extending classes
   * \author bmuller <bmuller@butterfat.net>
   */
  class CUDPropertyRegister {
  private:
    std::map<int, SarVector<std::string> > newprop;
    std::map<int, SarVector<std::string> > changedprop;
    std::map<int, SarVector<std::string> > deletedprop;
  protected:
    /** register new property */
    void register_new(std::string key, coltype ct);
    /** register changed property */
    void register_change(std::string key, coltype ct);
    /** register deleted property */
    void register_delete(std::string key, coltype ct);

    /** determine if a property has been registered new */
    bool is_registered_new(std::string key, coltype ct);
    /** determine if a property has been registered changed */
    bool is_registered_changed(std::string key, coltype ct);
    /** determine if a property has been registered deleted */
    bool is_registered_deleted(std::string key, coltype ct);

    /** unregister new property */
    void unregister_new(std::string key, coltype ct);
    /** unregister changed property */
    void unregister_change(std::string key, coltype ct);
    /** unregister deleted property */
    void unregister_delete(std::string key, coltype ct);

    /** get all new properties */
    void get_new(SarVector<std::string>& v, coltype ct);
    /** get all changed properties */
    void get_changed(SarVector<std::string>& v, coltype ct);
    /** get all deleted properties */
    void get_deleted(SarVector<std::string>& v, coltype ct);

    /** clear all records */
    void clear_registers();
    /** print the names of all new/changed/deleted properties */
    void dump_registers();
  };

};
