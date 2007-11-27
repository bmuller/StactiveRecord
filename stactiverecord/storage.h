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

  class KVT {
  public:
    std::string key, svalue;
    int ivalue;
    coltype type;
    KVT(std::string _key, std::string _value) : key(_key), svalue(_value), type(STRING) {};
    KVT(std::string _key, int _value) : key(_key), ivalue(_value), type(INTEGER) {};
    KVT(std::string _key, coltype ct) : key(_key), type(ct) {};
  };

  class Row {
  private:
    SarVector<int> ints;
    SarVector<std::string> strings;
  public:
    void operator<<(int i) { ints << i; };
    void operator<<(std::string s) { strings << s; };
    void get_string(int position, std::string& s) { 
      if(position > strings.size() - 1) 
	throw Sar_ColumnNotFoundException("String column not found.");
      s = strings[position];
    };
    int get_int(int position) { 
      if(position > ints.size() - 1) 
	throw Sar_ColumnNotFoundException("Int column not found.");
      return ints[position];
    };    
  };


  class Sar_Dbi {
  public:
    // config is in form scheme://[user[:password]@]host[:port]/database
    static Sar_Dbi* makeStorage(std::string config);
    static Sar_Dbi* dbi;
    SarVector<std::string> initialized_tables;
    Sar_Dbi() : initialized_tables() {};
 
    virtual void initialize_tables(std::string classname) {};
    virtual int next_id(std::string classname) {};
    virtual int current_id(std::string classname) {};
    virtual void delete_record(int id, std::string classname) {};
    virtual void delete_records(std::string classname) {};

    // get std::string values
    virtual void get(int id, std::string classname, SarMap<std::string>& values) {};
    // get int values
    virtual void get(int id, std::string classname, SarMap<int>& values) {};
    // get record relations (of a specific class)
    virtual void get(int id, std::string classname, std::string related_classname, SarVector<int>& related) {};
    // get all related classes
    virtual void get(int id, std::string classname, SarMap< SarVector<int> >& sm) {};

    // insert/modify std::string values
    virtual void set(int id, std::string classname, SarMap<std::string> values, bool insert) {};
    // insert/modify int values
    virtual void set(int id, std::string classname, SarMap<int> values, bool insert) {};
    // insert record relations
    virtual void set(int id, std::string classname, SarVector<int> related, std::string related_classname) {};

    // delete std::string / int value
    virtual void del(int id, std::string classname, SarVector<std::string> keys, coltype ct) {};
    // delete record relations
    virtual void del(int id, std::string classname, SarVector<int> related, std::string related_classname) {};

    // Some searching/static stuff
    // get all objects of a type
    virtual void get(std::string classname, SarVector<int>& results) {};
    // using a query with a conditional "where"
    virtual void get_where(std::string classname, std::string key, Where * where, SarVector<int>& results) {};

    bool table_is_initialized(std::string tablename);
  };

#ifdef HAVE_SQLITE3
#include <sqlite3.h>
  class SQLiteStorage : public Sar_Dbi {
  private:
    void close();
    sqlite3 *db;
    bool is_closed;
    void test_result(int result, const std::string& context);
    void execute(std::string query);
  public:
    SQLiteStorage(std::string location);
    ~SQLiteStorage() { close(); };
    int next_id(std::string classname);
    int current_id(std::string classname);
    void delete_record(int id, std::string classname);
    void delete_records(std::string classname);
    void initialize_tables(std::string classname);
    void get(int id, std::string classname, SarMap<std::string>& values);
    void get(int id, std::string classname, SarMap<int>& values);
    void set(int id, std::string classname, SarMap<std::string> values, bool insert);
    void set(int id, std::string classname, SarMap<int> values, bool insert);
    void del(int id, std::string classname, SarVector<std::string> keys, coltype ct);
    void set(int id, std::string classname, SarVector<int> related, std::string related_classname);
    void get(int id, std::string classname, std::string related_classname, SarVector<int>& related);
    void del(int id, std::string classname, SarVector<int> related, std::string related_classname);
    void get(int id, std::string classname, SarMap< SarVector<int> >& sm);
    void get(std::string classname, SarVector<int>& results);
    void get(std::string classname, std::string key, std::string value, SarVector<int>& results); 
    void get(std::string classname, std::string key, int value, SarVector<int>& results); 
    void get_where(std::string classname, std::string key, Where * where, SarVector<int>& results);

    SarVector<Row> select(std::string table, SarVector<KVT> cols, std::string key, Where * where);
    SarVector<Row> select(std::string table, SarVector<KVT> cols, std::string where="");
    void where_to_string(Where * where, std::string& swhere);
  };
#endif

#ifdef HAVE_MYSQL
#include <mysql/mysql.h>
  class MySQLStorage : public Sar_Dbi {
  private:
    void close();
    MYSQL *db;
    bool is_closed;
    void test_result(int result, const std::string& context);
    void execute(std::string query);
  public:
    MySQLStorage(std::string config);
    ~MySQLStorage() { close(); };
    int next_id(std::string classname);
    void delete_record(int id, std::string classname);
    void initialize_tables(std::string classname);
    void get(int id, std::string classname, SarMap<std::string>& values);
    void get(int id, std::string classname, SarMap<int>& values);
    void set(int id, std::string classname, SarMap<std::string> values, bool insert);
    void set(int id, std::string classname, SarMap<int> values, bool insert);
  };
#endif

#ifdef HAVE_DB_H 
#include <db_cxx.h>
  class BDBStorage : public Sar_Dbi {
  private:
    void close();
    Db db;
    bool is_closed;
  public:
    BDBStorage(std::string location);
    ~BDBStorage() { close(); };
    int next_id(std::string classname);
    int current_id(std::string classname);
    void delete_record(int id, std::string classname);
    void delete_records(std::string classname);
    void initialize_tables(std::string classname);
    void get(int id, std::string classname, SarMap<std::string>& values);
    void get(int id, std::string classname, SarMap<int>& values);
    void set(int id, std::string classname, SarMap<std::string> values, bool insert);
    void set(int id, std::string classname, SarMap<int> values, bool insert);
    void del(int id, std::string classname, SarVector<std::string> keys, coltype ct);
    void set(int id, std::string classname, SarVector<int> related, std::string related_classname);
    void get(int id, std::string classname, std::string related_classname, SarVector<int>& related);
    void del(int id, std::string classname, SarVector<int> related, std::string related_classname);
    void get(int id, std::string classname, SarMap< SarVector<int> >& sm);
    void get(std::string classname, SarVector<int>& results);
    void get(std::string classname, std::string key, std::string value, SarVector<int>& results); 
    void get(std::string classname, std::string key, int value, SarVector<int>& results); 
  };
#endif

};
