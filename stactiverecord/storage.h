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
  public:
    SarVector<int> ints;
    SarVector<std::string> strings;
    void operator<<(int i) { ints << i; };
    void operator<<(std::string s) { strings << s; };
    bool operator!=(Row& other) {
      return !(*this == other);
    };
    bool operator==(Row& other) {
      return ints == other.ints && strings == other.strings;
    };
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
    void dump() {
      std::cout << "Ints:\n";
      ints.dump();
      std::cout << "Strings:\n";
      strings.dump();
    };
  };


  class Sar_Dbi {
  public:
    // config is in form scheme://[user[:password]@]host[:port]/database
    static Sar_Dbi* makeStorage(std::string config, std::string prefix="");
    static Sar_Dbi* dbi;
    static SarMap<std::string> parseconfig(std::string config);
    std::string table_prefix;
    SarVector<std::string> initialized_tables;
    Sar_Dbi(std::string prefix = "") : initialized_tables(), table_prefix(prefix) {};
 
    virtual void initialize_tables(std::string classname) = 0;
    int next_id(std::string classname);
    bool exists(std::string classname, int id);
    void make_existing(std::string classname, int id);
    int current_id(std::string classname);
    void delete_record(int id, std::string classname);
    void delete_records(std::string classname);

    // get std::string values
    void get(int id, std::string classname, SarMap<std::string>& values);
    // get int values
    void get(int id, std::string classname, SarMap<int>& values);
    // get record relations (of a specific class)
    void get(int id, std::string classname, std::string related_classname, SarVector<int>& related);
    // get all related classes
    void get(int id, std::string classname, SarMap< SarVector<int> >& sm);

    // insert/modify std::string values
    void set(int id, std::string classname, SarMap<std::string> values, bool isinsert);
    // insert/modify int values
    void set(int id, std::string classname, SarMap<int> values, bool isinsert);
    // insert record relations
    void set(int id, std::string classname, SarVector<int> related, std::string related_classname);

    // delete std::string / int value
    void del(int id, std::string classname, SarVector<std::string> keys, coltype ct);
    // delete record relations
    void del(int id, std::string classname, SarVector<int> related, std::string related_classname);

    // Some searching/static stuff
    // get all objects of a type
    void get(std::string classname, SarVector<int>& results);
    // using a query with a conditional "where"
    void get_where(std::string classname, std::string key, Where * where, SarVector<int>& results);

    // Convert a where object to a string value for a query
    virtual void where_to_string(Where * where, std::string& swhere);

    SarVector<Row> select(std::string table, SarVector<KVT> cols, Q qwhere, bool distinct=false);
    virtual SarVector<Row> select(std::string table, SarVector<KVT> cols, std::string where="", bool distinct=false) = 0;
    void update(std::string table, SarVector<KVT> cols, Q qwhere);
    virtual void update(std::string table, SarVector<KVT> cols, std::string where="") = 0;
    void remove(std::string table, Q qwhere);
    virtual void remove(std::string table, std::string where="") = 0;
    virtual void insert(std::string table, SarVector<KVT> cols) = 0;
    virtual void execute(std::string query) = 0;

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
  public:
    SQLiteStorage(std::string location, std::string prefix="");
    ~SQLiteStorage() { close(); };
    void execute(std::string query);
    void initialize_tables(std::string classname);    
    SarVector<Row> select(std::string table, SarVector<KVT> cols, std::string where="", bool distinct=false);
    void update(std::string table, SarVector<KVT> cols, std::string where="");
    void remove(std::string table, std::string where="");
    void insert(std::string table, SarVector<KVT> cols);
  };
#endif

#ifdef HAVE_POSTGRESQL
#include <postgresql/libpq-fe.h>
  class PostgreSQLStorage : public Sar_Dbi {
  private:
    void close();
    PGconn *db;
    bool is_closed;
    void test_result(int result, const std::string& context);
  public:
    PostgreSQLStorage(std::string location, std::string prefix="");
    ~PostgreSQLStorage() { close(); };
    void execute(std::string query);
    void initialize_tables(std::string classname);    
    SarVector<Row> select(std::string table, SarVector<KVT> cols, std::string where="", bool distinct=false);
    void update(std::string table, SarVector<KVT> cols, std::string where="");
    void remove(std::string table, std::string where="");
    void insert(std::string table, SarVector<KVT> cols);
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
  public:
    MySQLStorage(std::string config, std::string prefix);
    ~MySQLStorage() { close(); };
    void execute(std::string query);
    void initialize_tables(std::string classname);    
    SarVector<Row> select(std::string table, SarVector<KVT> cols, std::string where="", bool distinct=false);
    void update(std::string table, SarVector<KVT> cols, std::string where="");
    void remove(std::string table, std::string where="");
    void insert(std::string table, SarVector<KVT> cols);
  };
#endif

};
