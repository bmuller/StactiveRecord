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

  SQLiteStorage::SQLiteStorage(std::string location) {
    debug("Attempting to open SQLite DB at " + location);
    is_closed = false;
    int rc = sqlite3_open(location.c_str(), &db);
    test_result(rc, "problem opening database");
    execute("CREATE TABLE IF NOT EXISTS id_maximums (id INT, classname VARCHAR(255))");
    execute("CREATE TABLE IF NOT EXISTS relationships (class_one VARCHAR(255), class_one_id INT, class_two VARCHAR(255), class_two_id INT)");
    debug("sqlite database opened successfully");
  };

  void SQLiteStorage::close() {
    if(is_closed)
      return;
    is_closed = true;
    test_result(sqlite3_close(db), "problem closing database");
  };

  void SQLiteStorage::test_result(int result, const std::string& context) {
    if(result != SQLITE_OK){
      std::string msg = "SQLite Error - " + context + ": " + std::string(sqlite3_errmsg(db)) + "\n";
      sqlite3_close(db);
      is_closed = true;
      throw Sar_DBException(msg);
    }
  };

  void SQLiteStorage::initialize_tables(std::string classname) {
    char *errMsg;
    std::string tablename;
    int rc;

    // make table for string values
    tablename = classname + "_s";
    if(!table_is_initialized(tablename)) {
      debug("initializing table " + tablename);
      std::string query = "CREATE TABLE IF NOT EXISTS " + tablename + " (id INT, keyname VARCHAR(255), "
	"value VARCHAR(" + VALUE_MAX_SIZE_S + "))";
      execute(query);
      initialized_tables.push_back(tablename);
    }

    // make table for string values
    tablename = classname + "_i";
    if(!table_is_initialized(tablename)) {
      debug("initializing table " + tablename);
      execute("CREATE TABLE IF NOT EXISTS " + tablename + " (id INT, keyname VARCHAR(255), value INT)");
      initialized_tables.push_back(tablename);
    }

    debug("Finished initializing tables for class " + classname);
  };

  void SQLiteStorage::execute(std::string query) {
    debug("SQLite executing: " + query);
    // this var doesn't matter cause it's the same as what will be printed by test_result
    char *errMsg; 
    int rc = sqlite3_exec(db, query.c_str(), NULL, 0, &errMsg);
    test_result(rc, query);
  };

  void SQLiteStorage::get(int id, std::string classname, SarMap< SarVector<int> >& sm) {
    sqlite3_stmt *pSelect;
    std::string s_id, query, key;
    int_to_string(id, s_id);
    debug("Getting all related objects to a " + classname);

    query = "SELECT class_one, class_one_id FROM relationships WHERE class_two=\"" + classname + "\" AND class_two_id=" + s_id;
    debug(query);
    int rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      char c_key[255];
      snprintf(c_key, 255, "%s", sqlite3_column_text(pSelect, 0));
      key = std::string(c_key);
      if(!sm.has_key(key))
	sm[key] = SarVector<int>();
      sm[key] << sqlite3_column_int(pSelect, 1);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);    

    query = "SELECT class_two, class_two_id FROM relationships WHERE class_one=\"" + classname + "\" AND class_one_id=" + s_id;
    debug(query);
    rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      char c_key[255];
      snprintf(c_key, 255, "%s", sqlite3_column_text(pSelect, 0));
      key = std::string(c_key);
      if(!sm.has_key(key))
	sm[key] = SarVector<int>();
      sm[key] << sqlite3_column_int(pSelect, 1);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);    
  };

  void SQLiteStorage::del(int id, std::string classname, SarVector<int> related, std::string related_classname) {
    if(related.size() == 0)
      return;
    std::string s_id, related_id;
    int_to_string(id, s_id);
    debug("Deleting some related " + related_classname + "s to a " + classname);
    bool swap = (strcmp(classname.c_str(), related_classname.c_str()) > 0) ? true : false;

    std::string idlist = "(";
    for(SarVector<int>::size_type i=0; i<related.size(); i++) {
      int_to_string(related[i], related_id);
      idlist += related_id;
      if(i!=(related.size() - 1))
	idlist += ",";
    }
    idlist += ")";

    if(swap)
      execute("DELETE FROM relationships WHERE class_two=\"" + classname + "\" AND class_two_id=" + s_id \
	      + " AND class_one=\"" + related_classname + "\" and class_one_id IN " + idlist);
    else
      execute("DELETE FROM relationships WHERE class_one=\"" + classname + "\" AND class_one_id=" + s_id \
	      + " AND class_two=\"" + related_classname + "\" and class_two_id IN " + idlist);
  };
  
  // some searching stuff
  void SQLiteStorage::get(std::string classname, SarVector<int>& results) {
    sqlite3_stmt *pSelect;
    SarVector<int> others;
    std::string tablename = classname + "_s";
    debug("Getting all objects of type " + classname);
    std::string query = "SELECT DISTINCT id FROM " + tablename;
    debug(query);
    int rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      results << sqlite3_column_int(pSelect, 0);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);
   
    tablename = classname + "_i";
    query = "SELECT DISTINCT id FROM " + tablename;
    debug(query);
    rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      others << sqlite3_column_int(pSelect, 0);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);
    results.unionize(others);
    others.clear();

    query = "SELECT DISTINCT class_one_id FROM relationships WHERE class_one = \"" + classname + "\"";
    debug(query);
    rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      others << sqlite3_column_int(pSelect, 0);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);
    results.unionize(others);
    others.clear();

    query = "SELECT DISTINCT class_two_id FROM relationships WHERE class_two = \"" + classname + "\"";
    debug(query);
    rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      others << sqlite3_column_int(pSelect, 0);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);
    results.unionize(others);
    others.clear();
  };

  void SQLiteStorage::where_to_string(Where * where, std::string& swhere) {
    bool isnot = where->isnot;
    if(where->ct == INTEGER) {
      std::string sint, second_sint;
      int_to_string(where->ivalue, sint);
      switch(where->type) {
      case GREATERTHAN:
	swhere = ((isnot) ? "<= " : "> ") + sint ;
	break;
      case LESSTHAN:
	swhere = ((isnot) ? ">= " : "< ") + sint;
	break;
      case EQUALS:
	swhere = ((isnot) ? "!= " : "= ") + sint;	
	break;
      case BETWEEN:
	int_to_string(where->ivaluetwo, second_sint);
	swhere = ((isnot) ? "NOT BETWEEN " : "BETWEEN " ) + sint + " AND " + second_sint;
	break;
      }
    } else if(where->ct == STRING) {
      switch(where->type) {
      case STARTSWITH:
	swhere = ((isnot) ? "NOT LIKE \"" : "LIKE \"") + where->svalue + "%\"";	
	break;
      case ENDSWITH:
	swhere = ((isnot) ? "NOT LIKE \"%" : "LIKE \"%") + where->svalue + "\"";	
	break;
      case EQUALS:
	swhere = ((isnot) ? "!= \"" : "= \"") + where->svalue + "\"";	
	break;
      case CONTAINS:
	swhere = ((isnot) ? "NOT LIKE \"%" : "LIKE \"%") + where->svalue + "%\"";	
      }
    }
  };
  
  void SQLiteStorage::insert(std::string table, SarVector<KVT> cols) {
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

  void SQLiteStorage::remove(std::string table, std::string key, Where * where) {
    std::string swhere;
    where_to_string(where, swhere);
    remove(table, key + " " + swhere);
  };

  void SQLiteStorage::remove(std::string table, std::string where) {
    execute("DELETE FROM " + ((where=="") ? table : table + " WHERE " + where));
  };

  void SQLiteStorage::update(std::string table, SarVector<KVT> cols, std::string key, Where * where) {
    std::string swhere;
    where_to_string(where, swhere);
    update(table, cols, key + " " + swhere);
  };

  void SQLiteStorage::update(std::string table, SarVector<KVT> cols, Q qwhere) {
    std::string where;
    qwhere.to_string(where);
    update(table, cols, where);
  }

  void SQLiteStorage::update(std::string table, SarVector<KVT> cols, std::string where) {
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

  SarVector<Row> SQLiteStorage::select(std::string table, SarVector<KVT> cols, std::string key, Where * where) {
    std::string swhere;
    where_to_string(where, swhere);
    return select(table, cols, key + " " + swhere);
  };

  SarVector<Row> SQLiteStorage::select(std::string table, SarVector<KVT> cols, Q qwhere) {
    std::string where;
    qwhere.to_string(where);
    return select(table, cols, where);
  };

  SarVector<Row> SQLiteStorage::select(std::string table, SarVector<KVT> cols, std::string where) {
    std::string columns;
    int result_iterator = 0;
    SarVector<std::string> s_cols;
    for(unsigned int i = 0; i < cols.size(); i++)
      s_cols << cols[i].key;
    sqlite3_stmt *pSelect;
    SarVector<Row> result;
    join(s_cols, ",", columns);
    std::string query = "SELECT " + columns + " FROM " + ((where=="") ? table : table + " WHERE " + where);
    debug(query);
    int rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      Row r;
      for(unsigned int i=0; i<cols.size(); i++) {
	if(cols[i].type == INTEGER) {
	  r << sqlite3_column_int(pSelect, i);
	} else if(cols[i].type == STRING) {
	  char c_key[255];
	  snprintf(c_key, 255, "%s", sqlite3_column_text(pSelect, i));
	  r << std::string(c_key);
	}
      }
      result << r;
      result_iterator++;
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);
    return result;
  };

  void SQLiteStorage::get_where(std::string classname, std::string key, Where * where, SarVector<int>& results) {
    bool isnot = where->isnot;
    std::string swhere, table, query;
    sqlite3_stmt *pSelect;
    if(where->ct == INTEGER) {
      table = classname + "_i";
      where_to_string(where, swhere);
      query = "SELECT id FROM " + table + " WHERE keyname=\"" + key + "\" AND value " + swhere;
    } else if(where->ct == STRING) {
      table = classname + "_s";
      where_to_string(where, swhere);
      query = "SELECT id FROM " + table + " WHERE keyname=\"" + key + "\" AND value " + swhere;
    } else { // RECORD
      bool swap = (strcmp(classname.c_str(), where->svalue.c_str()) > 0) ? true : false;
      std::string sint;
      int_to_string(where->ivalue, sint);
      if(swap)
	query = "SELECT class_two_id FROM relationships WHERE class_two = \"" + classname + "\" AND class_one_id = "
	  + sint + " AND class_one = \"" + where->svalue + "\"";
      else
	query = "SELECT class_one_id FROM relationships WHERE class_one = \"" + classname + "\" AND class_two_id = "
	  + sint + " AND class_two = \"" + where->svalue + "\"";
    }
    debug(query);
    int rc = sqlite3_prepare(db, query.c_str(), -1, &pSelect, 0);
    if( rc!=SQLITE_OK || !pSelect ){
      throw Sar_DBException("error preparing sql query: " + query);
    }
    rc = sqlite3_step(pSelect);
    while(rc == SQLITE_ROW){
      results << sqlite3_column_int(pSelect, 0);
      rc = sqlite3_step(pSelect);
    }
    rc = sqlite3_finalize(pSelect);
  };

};
