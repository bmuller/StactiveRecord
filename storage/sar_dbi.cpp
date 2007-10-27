#include "../stactive_record.h"

namespace stactiverecord {

  Sar_Dbi * Sar_Dbi::makeStorage(string config) {
    if (config == "sqlite")
	  return new SQLiteStorage("./somefile.db");
    else if (config=="mysql")
	  return new MySQLStorage("");
  };

  bool Sar_Dbi::table_is_initialized(string tablename) { 
    return initialized_tables.includes(tablename);
  };

};
