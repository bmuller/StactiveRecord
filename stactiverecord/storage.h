namespace stactiverecord {
  using namespace std;

  class Sar_Dbi {
  public:
    // config is in form scheme://[user[:password]@]host[:port]/database
    static Sar_Dbi* makeStorage(string config);
    static Sar_Dbi* dbi;
    SarVector<string> initialized_tables;
    Sar_Dbi() : initialized_tables() {};
 
    virtual void initialize_tables(string classname) {};
    virtual int next_id(string classname) {};
    virtual int current_id(string classname) {};
    virtual void delete_record(int id, string classname) {};
    virtual void delete_records(string classname) {};

    // get string values
    virtual void get(int id, string classname, SarMap<string>& values) {};
    // get int values
    virtual void get(int id, string classname, SarMap<int>& values) {};
    // get record relations (of a specific class)
    virtual void get(int id, string classname, string related_classname, SarVector<int>& related) {};
    // get all related classes
    virtual void get(int id, string classname, SarMap< SarVector<int> >& sm) {};

    // insert/modify string values
    virtual void set(int id, string classname, SarMap<string> values, bool insert) {};
    // insert/modify int values
    virtual void set(int id, string classname, SarMap<int> values, bool insert) {};
    // insert record relations
    virtual void set(int id, string classname, SarVector<int> related, string related_classname) {};

    // delete string / int value
    virtual void del(int id, string classname, SarVector<string> keys, coltype ct) {};
    // delete record relations
    virtual void del(int id, string classname, SarVector<int> related, string related_classname) {};

    // Some searching/static stuff
    // get all objects of a type
    virtual void get(string classname, SarVector<int>& results) {};
    // using a query with a conditional "where"
    virtual void get_where(string classname, string key, Where * where, SarVector<int>& results) {};

    bool table_is_initialized(string tablename);
  };

#ifdef HAVE_SQLITE3
#include <sqlite3.h>
  class SQLiteStorage : public Sar_Dbi {
  private:
    void close();
    sqlite3 *db;
    bool is_closed;
    void test_result(int result, const string& context);
    void execute(string query);
  public:
    SQLiteStorage(string location);
    ~SQLiteStorage() { close(); };
    int next_id(string classname);
    int current_id(string classname);
    void delete_record(int id, string classname);
    void delete_records(string classname);
    void initialize_tables(string classname);
    void get(int id, string classname, SarMap<string>& values);
    void get(int id, string classname, SarMap<int>& values);
    void set(int id, string classname, SarMap<string> values, bool insert);
    void set(int id, string classname, SarMap<int> values, bool insert);
    void del(int id, string classname, SarVector<string> keys, coltype ct);
    void set(int id, string classname, SarVector<int> related, string related_classname);
    void get(int id, string classname, string related_classname, SarVector<int>& related);
    void del(int id, string classname, SarVector<int> related, string related_classname);
    void get(int id, string classname, SarMap< SarVector<int> >& sm);
    void get(string classname, SarVector<int>& results);
    void get(string classname, string key, string value, SarVector<int>& results); 
    void get(string classname, string key, int value, SarVector<int>& results); 
    void get_where(string classname, string key, Where * where, SarVector<int>& results);
  };
#endif

#ifdef HAVE_MYSQL
#include <mysql/mysql.h>
  class MySQLStorage : public Sar_Dbi {
  private:
    void close();
    MYSQL *db;
    bool is_closed;
    void test_result(int result, const string& context);
    void execute(string query);
  public:
    MySQLStorage(string config);
    ~MySQLStorage() { close(); };
    int next_id(string classname);
    void delete_record(int id, string classname);
    void initialize_tables(string classname);
    void get(int id, string classname, SarMap<string>& values);
    void get(int id, string classname, SarMap<int>& values);
    void set(int id, string classname, SarMap<string> values, bool insert);
    void set(int id, string classname, SarMap<int> values, bool insert);
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
    BDBStorage(string location);
    ~BDBStorage() { close(); };
    int next_id(string classname);
    int current_id(string classname);
    void delete_record(int id, string classname);
    void delete_records(string classname);
    void initialize_tables(string classname);
    void get(int id, string classname, SarMap<string>& values);
    void get(int id, string classname, SarMap<int>& values);
    void set(int id, string classname, SarMap<string> values, bool insert);
    void set(int id, string classname, SarMap<int> values, bool insert);
    void del(int id, string classname, SarVector<string> keys, coltype ct);
    void set(int id, string classname, SarVector<int> related, string related_classname);
    void get(int id, string classname, string related_classname, SarVector<int>& related);
    void del(int id, string classname, SarVector<int> related, string related_classname);
    void get(int id, string classname, SarMap< SarVector<int> >& sm);
    void get(string classname, SarVector<int>& results);
    void get(string classname, string key, string value, SarVector<int>& results); 
    void get(string classname, string key, int value, SarVector<int>& results); 
  };
#endif

};
