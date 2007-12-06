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
  // config is in form  scheme://[user[:password]@host[:port]/]database
  PostgreSQLStorage::PostgreSQLStorage(std::string sconfig, std::string prefix) : Sar_Dbi(prefix) {
    debug("Attempting to open PostgreSQL DB with config " + sconfig);
    SarMap<std::string> config = parseconfig(sconfig);
    std::string pconfig = "";
    if(config.has_key("host"))
      pconfig += "hostaddr = '" + config["host"] + "' ";
    if(config.has_key("database"))
      pconfig += "dbname = '" + config["database"] + "' ";
    if(config.has_key("user"))
      pconfig += "user = '" + config["user"] + "' ";
    if(config.has_key("password"))
      pconfig += "password = '" + config["password"] + "' ";
    if(config.has_key("port"))
      pconfig += "port = '" + config["port"] + "' ";
    
    is_closed = false;
    db = PQconnectdb(pconfig.c_str());
    if (!db) {
      throw Sar_DBException("PostgreSQL connector error: Check hostname/port");
    }
    if (PQstatus(db) != CONNECTION_OK) {
      throw Sar_DBException("PostgreSQL connection error.  Check username/password/db name");
    }
    try { execute("CREATE TABLE " + table_prefix + "id_maximums (id INT, classname VARCHAR(255))"); } catch(...) {};
    try { execute("CREATE TABLE " + table_prefix + "relationships (class_one VARCHAR(255), "
		  "class_one_id INT, class_two VARCHAR(255), class_two_id INT)");
    } catch(...) {}
    debug("PostgreSQL database opened successfully");
  };

  void PostgreSQLStorage::close() {
    if(is_closed)
      return;
    is_closed = true;
    PQfinish(db);    
  };

  void PostgreSQLStorage::execute(std::string query) {
    debug("PostgreSQL executing: " + query);
    // this var doesn't matter cause it's the same as what will be printed by test_result
    PGresult *result = PQexec(db, query.c_str());
    if(!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK)) {
      std::string msg = std::string(PQresultErrorMessage(result));
      PQclear(result);
      throw Sar_DBException("PostgreSQL exception when executing: " + query + "\n" + msg);
    }
    PQclear(result);
  };

  void PostgreSQLStorage::initialize_tables(std::string classname) {
    std::string tablename;

    // make table for string values
    tablename = table_prefix + classname + "_s";
    if(!table_is_initialized(tablename)) {
      debug("initializing table " + tablename);
      std::string query = "CREATE TABLE " + tablename + " (id INT, keyname VARCHAR(255), "
	"value VARCHAR(" + VALUE_MAX_SIZE_S + "))";
      try { execute(query); } catch(...) {};
      initialized_tables.push_back(tablename);
    }

    // make table for int values
    tablename = table_prefix + classname + "_i";
    if(!table_is_initialized(tablename)) {
      debug("initializing table " + tablename);
      try { execute("CREATE TABLE " + tablename + " (id INT, keyname VARCHAR(255), value INT)"); } catch(...) {};
      initialized_tables.push_back(tablename);
    }

    // make table for datetime values
    tablename = table_prefix + classname + "_dt";
    if(!table_is_initialized(tablename)) {
      debug("initializing table " + tablename);
      try { execute("CREATE TABLE " + tablename + " (id INT, keyname VARCHAR(255), value INT)"); } catch(...) {};
      initialized_tables.push_back(tablename);
    }

    // make table for exiting objects
    tablename = table_prefix + classname + "_e";
    if(!table_is_initialized(tablename)) {
      debug("initializing table " + tablename);
      try { execute("CREATE TABLE " + tablename + " (id INT)"); } catch(...) {};
      initialized_tables.push_back(tablename);
    }

    debug("Finished initializing tables for class " + classname);
  };

  void PostgreSQLStorage::where_to_string(Where * where, std::string& swhere) {
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
	swhere = ((isnot) ? "NOT LIKE '" : "LIKE '") + where->svalue + "%'";	
	break;
      case ENDSWITH:
	swhere = ((isnot) ? "NOT LIKE '%" : "LIKE '%") + where->svalue + "'";	
	break;
      case EQUALS:
	swhere = ((isnot) ? "!= '" : "= '") + where->svalue + "'";	
	break;
      case CONTAINS:
	swhere = ((isnot) ? "NOT LIKE '%" : "LIKE '%") + where->svalue + "%'";	
      }
    }
  };
  
  void PostgreSQLStorage::insert(std::string table, SarVector<KVT> cols) {
    std::string columns, sint;
    std::string values = "";
    SarVector<std::string> s_cols;
    for(unsigned int i = 0; i < cols.size(); i++)
      s_cols << cols[i].key;
    join(s_cols, ",", columns);
    for(unsigned int i = 0; i < cols.size(); i++) {
      if(cols[i].type == STRING) {
	values += "'" + cols[i].svalue + "'";
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

  void PostgreSQLStorage::remove(std::string table, std::string where) {
    execute("DELETE FROM " + ((where=="") ? table : table + " WHERE " + where));
  };

  void PostgreSQLStorage::update(std::string table, SarVector<KVT> cols, std::string where) {
    std::string setstring = "";
    std::string sint;
    for(unsigned int i = 0; i < cols.size(); i++) {
      if(cols[i].type == STRING) {
	setstring += cols[i].key + "='" + cols[i].svalue + "'";
      } else if(cols[i].type == INTEGER) {
	int_to_string(cols[i].ivalue, sint);
	setstring += cols[i].key + "=" + sint;
      }
      if(i != cols.size() - 1)
	setstring += ",";
    }
    execute("UPDATE " + table + " SET " + ((where=="") ? setstring : setstring + " WHERE " + where));
  };

  SarVector<Row> PostgreSQLStorage::select(std::string table, SarVector<KVT> cols, std::string where, bool distinct) {
    std::string columns;
    SarVector<std::string> s_cols;
    for(unsigned int i = 0; i < cols.size(); i++)
      s_cols << cols[i].key;
    SarVector<Row> result;
    join(s_cols, ",", columns);
    std::string query = (distinct ? "SELECT DISTINCT " : "SELECT ") + columns + " FROM " + ((where=="") ? table : table + " WHERE " + where);
    debug(query);
    PGresult *qresult = PQexec(db, query.c_str());
    if(!qresult || (PQresultStatus(qresult) != PGRES_COMMAND_OK && PQresultStatus(qresult) != PGRES_TUPLES_OK)) {
      std::string msg = std::string(PQresultErrorMessage(qresult));
      PQclear(qresult);
      throw Sar_DBException("PostgreSQL exception when executing: " + query + "\n" + msg);
    }
    int rows = PQntuples(qresult);
    for(int row = 0; row < rows; row++) {
      Row r;
      for(unsigned int i=0; i<cols.size(); i++) {
	if(cols[i].type == INTEGER) {
	  r << string_to_int(PQgetvalue(qresult, row, i));
	} else if(cols[i].type == STRING) {
	  char c_key[255];
	  snprintf(c_key, 255, "%s", PQgetvalue(qresult, row, i));
	  r << std::string(c_key);
	}
      }
      result << r;
    }
    PQclear(qresult);
    return result;
  };

};
