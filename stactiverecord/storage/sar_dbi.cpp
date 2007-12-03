#include "../stactive_record.h"

namespace stactiverecord {

  Sar_Dbi * Sar_Dbi::makeStorage(std::string config, std::string prefix) {
    std::vector<std::string> configparts = explode(config, "://");
    if(configparts.size() != 2)
      throw Sar_InvalidConfigurationException("Invalid database configuration string: " + config);

#ifdef HAVE_MYSQL 
    if(configparts[0] == "mysql")
      return new MySQLStorage(configparts[1], prefix);
#endif

#ifdef HAVE_SQLITE3
    if(configparts[0] == "sqlite")
      return new SQLiteStorage(configparts[1], prefix);
#endif    

#ifdef HAVE_POSTGRESQL
    if(configparts[0] == "postgres")
      return new PostgreSQLStorage(configparts[1], prefix);
#endif    

    throw Sar_InvalidConfigurationException("DB type of \"" + configparts[0] + "\" not recognized.");
  };

  /** parse scheme://[user[:password]@host[:port]/]database into a hashmap that can contain the following
      keys: user, password, host, port, database
  */
  SarMap<std::string> Sar_Dbi::parseconfig(std::string config) {
    SarMap<std::string> result;
    std::vector<std::string> parts = explode(config, "/");
    if(parts.size() == 1) { 
      result["database"] = parts[0];
      return result;
    }
    result["database"] = parts[1];
    parts = explode(parts[0], "@");
    if(parts.size() == 1) 
      throw Sar_InvalidConfigurationException("If you specify a user you must also specify a host");
    std::string user = parts[0];
    std::string host = parts[1];
    parts = explode(user, ":");
    if(parts.size() == 1) {
      result["user"] = user;
    } else {
      result["user"] = parts[0];
      result["password"] = parts[1];
    }

    parts = explode(host, ":");
    if(parts.size() == 1) {
      result["host"] = host;
    } else {
      result["host"] = parts[0];
      result["port"] = parts[1];
    }
    return result;
  };

  bool Sar_Dbi::exists(std::string classname, int id) {
    std::string tablename = table_prefix + classname + "_e";
    SarVector<KVT> cols;
    cols << KVT("id", INTEGER);
    return select(tablename, cols, Q("id", id)).size() != 0;
  };

  void Sar_Dbi::make_existing(std::string classname, int id) {
    std::string tablename = table_prefix + classname + "_e";
    SarVector<KVT> cols;
    cols << KVT("id", id);
    insert(tablename, cols);
  };

  void Sar_Dbi::update(std::string table, SarVector<KVT> cols, Q qwhere) {
    std::string where;
    qwhere.to_string(where);
    update(table, cols, where);
  };

  void Sar_Dbi::remove(std::string table, Q qwhere) {
    std::string where = "";
    qwhere.to_string(where);
    remove(table, where);
  };

  SarVector<Row> Sar_Dbi::select(std::string table, SarVector<KVT> cols, Q qwhere, bool distinct) {
    std::string where;
    qwhere.to_string(where);
    return select(table, cols, where, distinct);
  };

  bool Sar_Dbi::table_is_initialized(std::string tablename) { 
    return initialized_tables.includes(tablename);
  };

  int Sar_Dbi::next_id(std::string classname) {
    SarVector<KVT> cols;
    cols << KVT("id", INTEGER);
    SarVector<Row> row = select(table_prefix + "id_maximums", cols, Q("classname", classname));
    SarVector<KVT> values;
    if(row.size() == 0) {
      debug("could not find max id int for " + classname + " - starting at 0");
      values << KVT("id", 0);
      values << KVT("classname", classname);
      insert(table_prefix + "id_maximums", values);
      return 0;
    }
    int maxid = row[0].get_int(0) + 1;
    values << KVT("id", maxid);
    update(table_prefix + "id_maximums", values, Q("classname", classname));
    return maxid;
  };

  int Sar_Dbi::current_id(std::string classname) {
    SarVector<KVT> values;
    values << KVT("id", INTEGER);
    SarVector<Row> rows = select(table_prefix + "id_maximums", values, Q("classname", classname));
    if(rows.size() == 0)
      return -1;
    return rows[0].get_int(0);
  };

  void Sar_Dbi::get(int id, std::string classname, SarMap<std::string>& values) {
    std::string tablename = table_prefix + classname + "_s";
    SarVector<KVT> cols;
    cols << KVT("keyname", STRING);
    cols << KVT("value", STRING);
    SarVector<Row> rows = select(tablename, cols, Q("id", id));
    std::string key, value;
    for (unsigned int i = 0; i < rows.size(); i++) {
      rows[i].get_string(0, key);
      rows[i].get_string(1, value);
      values[key] = value;
    }
  };

  void Sar_Dbi::get(int id, std::string classname, SarMap<int>& values) {
    std::string tablename = table_prefix + classname + "_i";
    SarVector<KVT> cols;
    cols << KVT("keyname", STRING);
    cols << KVT("value", INTEGER);
    SarVector<Row> rows = select(tablename, cols, Q("id", id));
    std::string key;
    int value;
    for (unsigned int i = 0; i < rows.size(); i++) {
      rows[i].get_string(0, key);
      value = rows[i].get_int(0);
      values[key] = value;
    }
  };

  void Sar_Dbi::set(int id, std::string classname, SarMap<std::string> values, bool isinsert) {
    std::string tablename = table_prefix + classname + "_s";
    for(std::map<std::string,std::string>::iterator i=values.begin(); i!=values.end(); ++i) {
      SarVector<KVT> values;
      values << KVT("value", std::string((*i).second));
      if(isinsert) {
        values << KVT("id", id);
	values << KVT("keyname", std::string((*i).first));
        insert(tablename, values);
      } else {
	update(tablename, values, Q("id", id) && Q("keyname", std::string((*i).first)));
      }
    }
  };

  void Sar_Dbi::set(int id, std::string classname, SarMap<int> values, bool isinsert) {
    std::string tablename = table_prefix + classname + "_i";
    for(std::map<std::string,int>::iterator i=values.begin(); i!=values.end(); ++i) {
      SarVector<KVT> values;
      values << KVT("value", (*i).second);
      if(isinsert) {
        values << KVT("id", id);
	values << KVT("keyname", std::string((*i).first));
        insert(tablename, values);
      } else {
	update(tablename, values, Q("id", id) && Q("keyname", std::string((*i).first)));
      }
    }
  };

  void Sar_Dbi::del(int id, std::string classname, SarVector<std::string> keys, coltype ct) {
    std::string tablename = table_prefix + ((ct == STRING) ? classname+"_s" : classname+"_i");
    for(unsigned int i=0; i < keys.size(); i++) 
      remove(tablename, Q("id", id) && Q("keyname", keys[i]));
  };
  
  void Sar_Dbi::delete_record(int id, std::string classname) {
    std::string tablename = table_prefix + classname + "_s";
    remove(tablename, Q("id", id));

    tablename = table_prefix + classname + "_i";
    remove(tablename, Q("id", id));

    tablename = table_prefix + classname + "_e";
    remove(tablename, Q("id", id));

    tablename = table_prefix + "relationships";
    remove(tablename, Q("class_one", classname) && Q("class_one_id", id));
    remove(tablename, Q("class_two", classname) && Q("class_two_id ", id));
  };

  void Sar_Dbi::delete_records(std::string classname) {
    // delete string values table
    std::string tablename = table_prefix + classname + "_s";
    remove(tablename);

    // delete int values table
    tablename = table_prefix + classname + "_i";
    remove(tablename);

    // delete existing values table
    tablename = table_prefix + classname + "_e";
    remove(tablename);

    std::string sclassname_where;
    tablename = table_prefix + "relationships";
    Where * classname_where = equals(classname);
    where_to_string(classname_where, sclassname_where);

    // delete entries from relationships
    remove(tablename, Q("class_one", classname) || Q("class_two", classname));

    // delete max id
    tablename = table_prefix + "id_maximums";
    remove(tablename, Q("classname", classname));
  };

  void Sar_Dbi::set(int id, std::string classname, SarVector<int> related, std::string related_classname) {
    std::string tablename = table_prefix + "relationships";
    bool swap = (strcmp(classname.c_str(), related_classname.c_str()) > 0) ? true : false;
    debug("Adding related " + related_classname + "s to a " + classname);
    for(SarVector<int>::size_type i=0; i<related.size(); i++) {
      SarVector<KVT> values;
      if(swap) {
        values << KVT("class_one", related_classname);
        values << KVT("class_one_id", related[i]);
        values << KVT("class_two", classname);
        values << KVT("class_two_id", id);
      } else {
        values << KVT("class_one", classname);
        values << KVT("class_one_id", id);
        values << KVT("class_two", related_classname);
        values << KVT("class_two_id", related[i]);
      }
      insert(tablename, values);
    }
  };

  void Sar_Dbi::get(int id, std::string classname, std::string related_classname, SarVector<int>& related) {
    std::string tablename = table_prefix + "relationships";
    debug("Getting related " + related_classname + "s to a " + classname);
    bool swap = (strcmp(classname.c_str(), related_classname.c_str()) > 0) ? true : false;
    SarVector<KVT> cols;
    SarVector<Row> rows;

    if(swap) {
      cols << KVT("class_one_id", INTEGER);
      rows = select(tablename, cols, Q("class_one", related_classname) && Q("class_two_id", id) && Q("class_two", classname));
    } else {
      cols << KVT("class_two_id", INTEGER);
      rows = select(tablename, cols, Q("class_one", classname) && Q("class_one_id", id) && Q("class_two", related_classname));
    }

    for(unsigned int i = 0; i < rows.size(); i++)
      related << rows[i].get_int(0);
  };

  void Sar_Dbi::get(int id, std::string classname, SarMap< SarVector<int> >& sm) {
    debug("Getting all related objects to a " + classname);
    std::string tablename = table_prefix + "relationships";
    SarVector<KVT> cols;
    cols << KVT("class_one", STRING);
    cols << KVT("class_one_id", INTEGER);
    SarVector<Row> rows = select(tablename, cols, Q("class_two", classname) && Q("class_two_id", id));
    for(unsigned int i = 0; i<rows.size(); i++) {
      std::string key;
      rows[i].get_string(0, key);
      if(!sm.has_key(key))
        sm[key] = SarVector<int>();
      sm[key] << rows[i].get_int(0);
    }

    cols.clear();
    cols << KVT("class_two", STRING);
    cols << KVT("class_two_id", INTEGER);
    rows = select(tablename, cols, Q("class_one", classname) && Q("class_one_id", id));
    for(unsigned int i = 0; i<rows.size(); i++) {
      std::string key;
      rows[i].get_string(0, key);
      if(!sm.has_key(key))
        sm[key] = SarVector<int>();
      sm[key] << rows[i].get_int(0);
    }
  };

  void Sar_Dbi::del(int id, std::string classname, SarVector<int> related, std::string related_classname) {
    if(related.size() == 0)
      return;
    std::string tablename = table_prefix + "relationships";
    debug("Deleting some related " + related_classname + "s to a " + classname);
    bool swap = (strcmp(classname.c_str(), related_classname.c_str()) > 0) ? true : false;
    if(swap)
      remove(tablename, Q("class_two", classname) && Q("class_two_id", id) && Q("class_one", related_classname) && Q("class_one_id", in(related)));
    else
      remove(tablename, Q("class_one", classname) && Q("class_one_id", id) && Q("class_two", related_classname) && Q("class_two_id", in(related)));
  };

  void Sar_Dbi::get(std::string classname, SarVector<int>& results) {
    std::string tablename = table_prefix + classname + "_s";
    debug("Getting all objects of type " + classname);
    SarVector<KVT> cols;
    cols << KVT("id", INTEGER);
    SarVector<Row> rows = select(tablename, cols, "", true);

    tablename = table_prefix + classname + "_i";
    rows.unionize(select(tablename, cols, "", true));

    tablename = table_prefix + "relationships";
    cols.clear();
    cols << KVT("class_one_id", INTEGER);
    rows.unionize(select(tablename, cols, Q("class_one", classname), true));

    cols.clear();
    cols << KVT("class_two_id", INTEGER);
    rows.unionize(select(tablename, cols, Q("class_two", classname), true));

    for(unsigned int i=0; i < rows.size(); i++)
      results << rows[i].get_int(0);
  };

  void Sar_Dbi::get_where(std::string classname, std::string key, Where * where, SarVector<int>& results) {
    bool isnot = where->isnot;
    SarVector<Row> rows;
    SarVector<KVT> cols;
    std::string tablename;
    if(where->ct == INTEGER) {
      tablename = table_prefix + classname + "_i";
      cols << KVT("id", INTEGER);
      rows = select(tablename, cols, Q("keyname", key) && Q("value", where));
    } else if(where->ct == STRING) {
      tablename = table_prefix + classname + "_s";
      cols << KVT("id", INTEGER);
      rows = select(tablename, cols, Q("keyname", key) && Q("value", where));
    } else { //RECORD
      tablename = table_prefix + "relationships";
      bool swap = (strcmp(classname.c_str(), where->svalue.c_str()) > 0) ? true : false;
      if(swap) {
        cols << KVT("class_two_id", INTEGER);
        rows = select(tablename, cols, Q("class_two", classname) && Q("class_one_id", where->ivalue) && Q("class_one", where->svalue));
      } else {
        cols << KVT("class_one_id", INTEGER);
        rows = select(tablename, cols, Q("class_one", classname) && Q("class_two_id", where->ivalue) && Q("class_two", where->svalue));
      }
    }

    for(unsigned int i = 0; i < rows.size(); i++)
      results << rows[i].get_int(0);
  };

  /** Most SQL db's will use the following - kids can overwrite if necessary (like postgres) **/
  void Sar_Dbi::where_to_string(Where * where, std::string& swhere) {
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
    } else if(where->ct == ALL) {
      switch(where->type) {
      case ISNULL:
        swhere = (isnot) ? "IS NOT NULL" : "IS NULL";
	break;
      }
    }
  };

};
