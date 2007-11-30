#include "../stactive_record.h"

#include <stdio.h>

namespace stactiverecord {
  using namespace std;

  MySQLStorage::MySQLStorage(string config, string prefix) : Sar_Dbi(prefix) {
    is_closed = false;
    db = mysql_init(NULL);
	mysql_real_connect(db, "localhost", "test", NULL, "test", 0, NULL, 0);
    test_result(mysql_errno(db), "opening database");
	string query = "CREATE TABLE IF NOT EXISTS id_maximums "
      "(id INT, classname VARCHAR(255), valuetype INT)";
	int rc = mysql_query(db, query.c_str());
	test_result(rc, "");
  };

  void MySQLStorage::close() {
    if(is_closed)
      return;
  	mysql_close(db);
    is_closed = true;
  };

  void MySQLStorage::test_result(int result, const string& context) {
	if(result != 0){
      string msg = "MySQL Error in Session Manager - " + context + ": " + string(mysql_error(db)) + "\n";
      mysql_close(db);
      is_closed = true;
      throw Sar_DBException(msg);
    }
  };

  int MySQLStorage::next_id(string classname) {
    int maxid;
	MYSQL_BIND bind[1];
    MYSQL_STMT *stmt = mysql_stmt_init(db);
    string query = "SELECT id FROM id_maximums WHERE classname = \"" + classname + "\"";
    int rc = mysql_stmt_prepare(stmt, query.c_str(), query.length());
    if( rc!=0 )
      throw Sar_DBException("error preparing sql query: " + query);
	rc = mysql_stmt_execute(stmt);
	if ( rc!= 0 )
	  throw Sar_DBException("error executing sql query: " + query);

	// bind the result buffers.
	memset(bind, 0, sizeof(bind));
	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (char*)&maxid;

	// fetch the results and insert in no maxid exists.
    if (mysql_stmt_fetch(stmt) == MYSQL_NO_DATA) {
      debug("could not find max id int for " + classname + " - starting at 0");
      mysql_stmt_close(stmt);
      query = "INSERT INTO id_maximums (id,classname) VALUES(0, \"" + classname + "\")";
      execute(query);
      return 0;
    }

    string maxid_s;
    int_to_string(maxid, maxid_s);
    execute("UPDATE id_maximums SET id = " + maxid_s + " WHERE classname = \"" + classname + "\"");
    return maxid;
  };
 
  void MySQLStorage::initialize_tables(string classname) {
    string tablename = classname + "_s";
    int rc;
	if(!table_is_initialized(tablename)) {
      string query = "CREATE TABLE IF NOT EXISTS " + tablename + " (id INT, keyname VARCHAR(255), "
	    "value VARCHAR(" + VALUE_MAX_SIZE_S + "))";
      debug("initializing table " + tablename);
      rc = mysql_query(db, query.c_str());
      test_result(rc, "problem initializing table " + tablename);
      initialized_tables.push_back(tablename);
    }

    tablename = classname + "_i";
    if(!table_is_initialized(tablename)) {
      debug("initializing table " + tablename);
      string query = "CREATE TABLE IF NOT EXISTS " + tablename + " (id INT, keyname VARCHAR(255), value INT)";
      rc = mysql_query(db, query.c_str());
      test_result(rc, "problem initializing table " + tablename);
      initialized_tables.push_back(tablename);
	}
  };

  void MySQLStorage::get(int id, string classname, SarMap<string>& values) {
    MYSQL_STMT *stmt = mysql_stmt_init(db);
	MYSQL_BIND bind[2];
    string tablename = classname + "_s";
    string id_s;
    char c_key[255];
    char c_value[VALUE_MAX_SIZE + 1];
    
	int_to_string(id, id_s);
    string query = "SELECT keyname,value FROM " + tablename + " WHERE id = " + id_s;
    
	int rc = mysql_stmt_prepare(stmt, query.c_str(), query.length());
    
	if( rc!=0 )
      throw Sar_DBException("error preparing sql query: " + query);
	
	rc = mysql_stmt_execute(stmt);
	
	if ( rc!=0 )
		throw Sar_DBException("error executing sql query: " + query);
    // bind the result buffers before fetching.
	memset(bind, 0, sizeof(bind));
	
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = (char*)&c_key;
	bind[0].buffer_length = 255;
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = (char*)&c_value;
	bind[1].buffer_length = 255;
    
	mysql_stmt_bind_result(stmt, bind);

	while(mysql_stmt_fetch(stmt) != MYSQL_NO_DATA){
      values[string(c_key)] = string(c_value);
    }    
    mysql_stmt_close(stmt);
  };

  void MySQLStorage::get(int id, string classname, SarMap<int>& values) {
    MYSQL_STMT *stmt = mysql_stmt_init(db);
	MYSQL_BIND bind[2];
    string tablename = classname + "_s";
    string id_s;
    char c_key[255];
	int value;
    
	int_to_string(id, id_s);
    string query = "SELECT keyname,value FROM " + tablename + " WHERE id = " + id_s;
    int rc = mysql_stmt_prepare(stmt, query.c_str(), query.length());
    if( rc!=0 )
      throw Sar_DBException("error preparing sql query: " + query);
    rc = mysql_stmt_execute(stmt);
	if( rc!=0 )
	  throw Sar_DBException("error executing sql query: " + query);
	
	// bind the result buffers before fetching.
	memset(bind, 0, sizeof(bind));
	
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = (char*)&c_key;
	bind[0].buffer_length = 255;
	bind[1].buffer_type = MYSQL_TYPE_LONG;
	bind[1].buffer = (char*)&value;

    while(mysql_stmt_fetch(stmt) != MYSQL_NO_DATA) 
      values[string(c_key)] = value;
	mysql_stmt_close(stmt); 
  };

  void MySQLStorage::set(int id, string classname, SarMap<string> values, bool insert) {
    string tablename = classname + "_s";
    string query,id_s;
    int_to_string(id, id_s);
    if(insert) {
      for(map<string,string>::iterator i=values.begin(); i!=values.end(); ++i) {
        query = "INSERT INTO " + tablename + " (id,keyname,value) VALUES (" + id_s + ",\"" + string((*i).first) + "\",\"" + string((*i).second) + "\")";
        execute(query);
      }
    } else {
      for(map<string,string>::iterator i=values.begin(); i!=values.end(); ++i) {
        query = "UPDATE " + tablename + " SET keyname=\"" + string((*i).first) + "\", value=\"" + string((*i).second) + "\" WHERE id=" + id_s;
        execute(query);
      }
    }
  };

  void MySQLStorage::set(int id, string classname, SarMap<int> values, bool insert) {
	string tablename = classname + "_s";
    string query,id_s,value_s;
    int_to_string(id, id_s);
    
	if (insert) {
	  for(map<string,int>::iterator i=values.begin(); i!=values.end(); ++i) {
	    int_to_string((*i).second, value_s);
        query = "INSERT INTO " + tablename + " (id,keyname,value) VALUES (" + id_s +",\"" + string((*i).first) + "\"," + value_s + ")";
        execute(query);
	  }
	} else {
	  for(map<string,int>::iterator i=values.begin(); i!=values.end(); ++i) {
	    int_to_string((*i).second,value_s);
	    query = "UPDATE " + tablename + " SET keyname=\"" + string((*i).first) + "\", value=" + value_s + " WHERE id=" + id_s;
		execute(query);
	  }
	}
  };

  void MySQLStorage::execute(string query) {
    MYSQL_STMT *stmt = mysql_stmt_init(db);
	int rc = mysql_stmt_prepare(stmt, query.c_str(), query.length());
	test_result(rc, "preparing sql query: " + query);
	rc = mysql_stmt_execute(stmt);
	test_result(rc, "executing sql query: " + query);
  }

  void MySQLStorage::delete_record(int id, string classname) {
    int rc;
    string id_s, query, tablename;
    int_to_string(id, id_s);

    // delete all string values
    tablename = classname + "_s";
    query = "DELETE FROM " + tablename + " WHERE id = " + id_s;
    rc = mysql_query(db, query.c_str());
    test_result(rc, "problem deleting record");

    // delete all int values
    tablename = classname + "_i";
    query = "DELETE FROM " + tablename + " WHERE id = " + id_s;
    rc = mysql_query(db, query.c_str());
    test_result(rc, "problem deleting record");
  };
 
};
