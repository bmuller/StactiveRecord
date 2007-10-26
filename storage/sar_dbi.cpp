#include "../stactive_record.h"

namespace stactiverecord {

  Sar_Dbi * Sar_Dbi::makeStorage(string config) {
    return new SQLiteStorage("./somefile.db");
  };

  bool Sar_Dbi::table_is_initialized(string tablename) { 
    return initialized_tables.includes(tablename);
  };

};
