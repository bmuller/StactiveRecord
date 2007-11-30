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

/* example code
  PGconn *psql = PQconnectdb("hostaddr = '127.0.0.1' dbname = 'stactiverecord' user = 'stactiverecord' password = 'stactiverecord'");
  if (!psql) {
    fprintf(stderr, "libpq error: PQconnectdb returned NULL.\n\n");
  }
  if (PQstatus(psql) != CONNECTION_OK) {
    fprintf(stderr, "libpq error: PQstatus(psql) != CONNECTION_OK\n\n");
  }

  PGresult *result = PQexec(psql, "SELECT now()");
  int rowcount;
  if (!result || !(rowcount = PQntuples(result))) {
    fprintf(stderr, "libpq error: no rows returned or bad result set\n\n");
    PQfinish(psql);
    return 0;
  }
  for (int i = 0; i < rowcount; i++) {
    printf("Time: %s\n", PQgetvalue(result, i, 0));
  }
  PQclear(result);

  PQfinish(psql);
*/





#include "../stactive_record.h"

namespace stactiverecord {
  // config is in form scheme://[user[:password]@]host[:port]/database
  PostgreSQLStorage::PostgreSQLStorage(std::string config, std::string prefix) : Sar_Dbi(prefix) {
    debug("Attempting to open PostgreSQL DB at " + config);
    /*
    SarVector<std::string> parts = explode(config, '/');
    std::string database;
    if(parts.size() == 1) {
      
    }
    parts = explode(parts
    */
    /*
    is_closed = false;
    int rc = sqlite3_open(location.c_str(), &db);
    test_result(rc, "problem opening database");
    execute("CREATE TABLE IF NOT EXISTS " + table_prefix + "id_maximums (id INT, classname VARCHAR(255))");
    execute("CREATE TABLE IF NOT EXISTS " + table_prefix + "relationships (class_one VARCHAR(255), "
	    "class_one_id INT, class_two VARCHAR(255), class_two_id INT)");
    debug("sqlite database opened successfully");
    */
  };

  void PostgreSQLStorage::close() {
    /*
    if(is_closed)
      return;
    is_closed = true;
    test_result(sqlite3_close(db), "problem closing database");
    */
  };

  void PostgreSQLStorage::execute(std::string query) {
    /*
      debug("SQLite executing: " + query);
    */
    // this var doesn't matter cause it's the same as what will be printed by test_result
    /*
    char *errMsg; 
    int rc = sqlite3_exec(db, query.c_str(), NULL, 0, &errMsg);
    test_result(rc, query);
    */
  };

  void PostgreSQLStorage::test_result(int result, const std::string& context) {
    /*
    if(result != SQLITE_OK){
      std::string msg = "SQLite Error - " + context + ": " + std::string(sqlite3_errmsg(db)) + "\n";
      sqlite3_close(db);
      is_closed = true;
      throw Sar_DBException(msg);
    }
    */
  };

  void PostgreSQLStorage::initialize_tables(std::string classname) {
    /*
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

    // make table for string values
    tablename = table_prefix + classname + "_i";
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
    */
  };

  void PostgreSQLStorage::where_to_string(Where * where, std::string& swhere) {
    /*
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
      case IN:
	std::string idlist = "(";
	for(std::vector<int>::size_type i=0; i<where->ivalues.size(); i++) {
	  int_to_string(where->ivalues[i], sint);
	  idlist += sint;
	  if(i!=(where->ivalues.size() - 1))
	    idlist += ",";
	}
	idlist += ")";
	swhere = ((isnot) ? "NOT IN " : "IN " ) + idlist;
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
    */
  };
  
  void PostgreSQLStorage::insert(std::string table, SarVector<KVT> cols) {
    /*
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
    */
  };

  void PostgreSQLStorage::remove(std::string table, std::string where) {
    execute("DELETE FROM " + ((where=="") ? table : table + " WHERE " + where));
  };

  void PostgreSQLStorage::update(std::string table, SarVector<KVT> cols, std::string where) {
    /*
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
    */
  };

  SarVector<Row> PostgreSQLStorage::select(std::string table, SarVector<KVT> cols, std::string where, bool distinct) {
        SarVector<Row> result;
	return result;
    /*
    std::string columns;
    int result_iterator = 0;
    SarVector<std::string> s_cols;
    for(unsigned int i = 0; i < cols.size(); i++)
      s_cols << cols[i].key;
    sqlite3_stmt *pSelect;
    SarVector<Row> result;
    join(s_cols, ",", columns);
    std::string query = (distinct ? "SELECT DISTINCT " : "SELECT ") + columns + " FROM " + ((where=="") ? table : table + " WHERE " + where);
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
    */
  };

};
