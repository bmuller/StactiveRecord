#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <exception>

namespace stactiverecord {
  // Note that a change here means editing coltype_to_name in utils too
  enum coltype { NONE, INTEGER, STRING, RECORD };
  enum wheretype { STARTSWITH, ENDSWITH, CONTAINS, GREATERTHAN, LESSTHAN, BETWEEN };
};

#include "config.h"
#include "where.h"
#include "utils.h"
#include "types.h"
#include "cud_property_register.h"
#include "exception.h"
#include "storage.h"
#include "query.h"
#include "record.h"

#define VALUE_MAX_SIZE 255
#define VALUE_MAX_SIZE_S "255"

