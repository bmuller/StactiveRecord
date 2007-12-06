/** \mainpage StactiveRecord :: (Static) ActiveRecord for C++
    \section desc Description
    The goal of the project is to create an implementation of the Active Record pattern for C++

    \section Resources
    \li The source, docs, and more info can be found at http://butterfat.net/wiki/Projects/StactiveRecord
    \li Bug reporting can be found at https://coop.butterfat.net/tracker/butterfat/
    \section Author
    Brian Muller <bmuller@butterfat.net>
*/
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <exception>
#include <time.h>

/// \namespace stactiverecord The project's namespace
namespace stactiverecord {
  /** 
   * \enum coltype The possible valid column types. Note that a change here means 
   * editing coltype_to_name in utils too.
   * \enum wheretype The possible specific types/relationships a Where class can represent. 
   */
  enum coltype { NONE, INTEGER, STRING, RECORD, DATETIME, ALL };
  enum wheretype { STARTSWITH, ENDSWITH, CONTAINS, GREATERTHAN, LESSTHAN, BETWEEN, EQUALS, OBJECTRELATION, IN, ISNULL };
};

#include "config.h"
#include "datetime.h"
#include "where.h"
#include "utils.h"
#include "types.h"
#include "cud_property_register.h"
#include "exception.h"
#include "query.h"
#include "storage.h"
#include "record.h"

namespace stactiverecord {
  // Due to the fracking limitations of C++, this definition can't go in where.h
  // or a circular dependency is introduced.  Gack!
  Where * hasobject(Record r);
};

#define VALUE_MAX_SIZE 255
#define VALUE_MAX_SIZE_S "255"

