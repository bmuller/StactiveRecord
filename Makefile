LIBS = cud_property_register.o utils.o storage/sar_dbi.o record.o storage/mysql_storage.o storage/sqlite_storage.o -lsqlite3 -lmysqlpp
PREREQ = utils.o cud_property_register.o sar_dbi.o record.o mysql_storage.o sqlite_storage.o

main: ${PREREQ}
	g++ test.cpp ${LIBS} -o test 
db_test: ${PREREQ}
	g++ tests/db_test.cpp ${LIBS} -o tests/db_test
cud_property_register.o:
	g++ cud_property_register.cpp -o cud_property_register.o -c
utils.o:
	g++ utils.cpp -o utils.o -c
sar_dbi.o:
	g++ storage/sar_dbi.cpp -o storage/sar_dbi.o -c
record.o:
	g++ record.cpp -o record.o -c 
mysql_storage.o:
	g++ storage/mysql_storage.cpp -o storage/mysql_storage.o -c
sqlite_storage.o:
	g++ storage/sqlite_storage.cpp -o storage/sqlite_storage.o -c
clean:
	rm -rf *.o ./storage/*.o ./types/*.o test *.db