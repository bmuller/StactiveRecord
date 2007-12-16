#!/bin/bash
(./db_test $1 && ./object_test $1 && echo "Tests completed successfully") || echo "Tests failed"