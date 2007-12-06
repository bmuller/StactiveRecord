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

#include "../stactive_record.h"

namespace stactiverecord {

  MySQLStorage::MySQLStorage(std::string location, std::string prefix) : Sar_Dbi(prefix) {
    debug("Attempting to open Mysql DB at " + location);
    is_closed = false;
    db = mysql_init(NULL);
    SarMap<std::string> config = parseconfig(location);
    mysql_real_connect(db,
                       (config.has_key("host") ? config["host"].c_str() : "localhost"),
                       (config.has_key("user") ? config["user"].c_str() : NULL),
                       (config.has_key("password") ? config["password"].c_str() : NULL),
                       config["database"].c_str(),
                       (config.has_key("port") ? string_to_int(config["port"]) : 0),
                       NULL,
                       0);
    test_result(mysql_errno(db), "problem opening database");
    execute("CREATE TABLE IF NOT EXISTS " + table_prefix + "id_maximums (id INT, classname VARCHAR(255))");
    execute("CREATE TABLE IF NOT EXISTS " + table_prefix + "relationships (class_one VARCHAR(255), "
	    "class_one_id INT, class_two VARCHAR(255), class_two_id INT)");
    debug("Mysql database opened successfully");
  };

  void MySQLStorage::close() {
    if(is_closed)
      return;
    is_closed = true;
    mysql_close(db);
  };

  void MySQLStorage::execute(std::string query) {
    MYSQL_STMT *stmt = mysql_stmt_init(db);
    int rc = mysql_stmt_prepare(stmt, query.c_str(), query.length());
    test_result(rc, "preparing sql query: " + query);
    rc = mysql_stmt_execute(stmt);
    test_result(rc, "executing sql query: " + query);
  }

  void MySQLStorage::test_result(int result, const std::string& context) {
    if(result != 0){
      std::string msg = "Mysql Error - " + context + ": " + std::string(mysql_error(db)) + "\n";
      mysql_close(db);
      is_closed = true;
      throw Sar_DBException(msg);
    }
  };

  void MySQLStorage::initialize_tables(std::string classname) {
    std::string tablename;

    // make table for string values
    tablename = table_prefix + classname + "_s";
    if(!table_is_initialized(tablename)) {
      debug("initializing table " + tablename);
      std::string query = "CREATE TABLE IF NOT EXISTS " + tablename + " (id INT, keyname VARCHAR(255), "
	"value VARCHAR(" + VALUE_MAX_SIZE_S + "))";
      execute(query);
      initialized_tables.push_back(tablename);
    }

    // make table for int values
    tablename = table_prefix + classname + "_i";
    if(!table_is_initialized(tablename)) {
      debug("initializing table " + tablename);
      execute("CREATE TABLE IF NOT EXISTS " + tablename + " (id INT, keyname VARCHAR(255), value INT)");
      initialized_tables.push_back(tablename);
    }

    // make table for int values
    tablename = table_prefix + classname + "_dt";
    if(!table_is_initialized(tablename)) {
      debug("initializing table " + tablename);
      execute("CREATE TABLE IF NOT EXISTS " + tablename + " (id INT, keyname VARCHAR(255), value INT)");
      initialized_tables.push_back(tablename);
    }

    // make table for exiting objects
    tablename = table_prefix + classname + "_e";
    if(!table_is_initialized(tablename)) {
      debug("initializing table " + tablename);
      execute("CREATE TABLE IF NOT EXISTS " + tablename + " (id INT)");
      initialized_tables.push_back(tablename);
    }

    debug("Finished initializing tables for class " + classname);
  };
 
  void MySQLStorage::insert(std::string table, SarVector<KVT> cols) {
    std::string columns, sint;
    std::string values = "";
    SarVector<std::string> s_cols;
    for(unsigned int i = 0; i < cols.size(); i++)
      s_cols << cols[i].key;
    join(s_cols, ",", columns);
    for(unsigned int i = 0; i < cols.size(); i++) {
      if(cols[i].type == STRING) {
	values += "\"" + cols[i].svalue + "\"";
      } else if(cols[i].type == INTEGER) {
	int_to_string(cols[i].ivalue, sint);
	values += sint;
      }
      if(i != cols.size() - 1)
	values += ",";
    } 
    std::string query = "INSERT INTO " + table + " (" + columns + ") VALUES(" + values + ")";
    execute(query);
  };

  void MySQLStorage::remove(std::string table, std::string where) {
    execute("DELETE FROM " + ((where=="") ? table : table + " WHERE " + where));
  };

  void MySQLStorage::update(std::string table, SarVector<KVT> cols, std::string where) {
    std::string setstring = "";
    std::string sint;
    for(unsigned int i = 0; i < cols.size(); i++) {
      if(cols[i].type == STRING) {
	setstring += cols[i].key + "=\"" + cols[i].svalue + "\"";
      } else if(cols[i].type == INTEGER) {
	int_to_string(cols[i].ivalue, sint);
	setstring += cols[i].key + "=" + sint;
      }
      if(i != cols.size() - 1)
	setstring += ",";
    }
    execute("UPDATE " + table + " SET " + ((where=="") ? setstring : setstring + " WHERE " + where));
  };

  SarVector<Row> MySQLStorage::select(std::string table, SarVector<KVT> cols, std::string where, bool distinct) {
    std::string columns;
    SarVector<std::string> s_cols;
    for(unsigned int i = 0; i < cols.size(); i++)
      s_cols << cols[i].key;

    SarVector<Row> result;
    join(s_cols, ",", columns);
    std::string query = (distinct ? "SELECT DISTINCT " : "SELECT ") + columns + " FROM " + ((where=="") ? table : table + " WHERE " + where);
    debug(query);
    int rc = mysql_real_query(db, query.c_str(), query.length());
    if( rc!=0 )
      throw Sar_DBException("error preparing sql query: " + query);
    MYSQL_RES *res = mysql_use_result(db);
    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))) {
      Row r;
      for(unsigned int i=0; i<cols.size(); i++) {
	if(cols[i].type == INTEGER) {
	  r << string_to_int(std::string(row[i]));
	} else if(cols[i].type == STRING) {
	  char c_key[255];
	  snprintf(c_key, 255, "%s", row[i]);
	  r << std::string(c_key);
	}
      }
      result << r;
    }
    return result;
  };

};
