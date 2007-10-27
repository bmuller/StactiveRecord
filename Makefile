
mysql_lib_options = -L/usr/local/mysql/lib/mysql -lmysqlclient
sqlite_lib_options = -lsqlite3

main: sar_dbi.o utils.o record.o sqlite_storage.o mysql_storage.o
	g++ test.cpp utils.o storage/sar_dbi.o record.o storage/sqlite_storage.o -o test -lsqlite3
mysql: sar_dbi.o utils.o record.o sqlite_storage.o mysql_storage.o
	g++ mysql_test.cpp utils.o storage/sar_dbi.o record.o storage/mysql_storage.o storage/sqlite_storage.o \
	    -o mysql_test $(mysql_lib_options) $(sqlite_lib_options)
utils.o:
	g++ utils.cpp -o utils.o -c
sar_dbi.o:
	g++ storage/sar_dbi.cpp -o storage/sar_dbi.o -c
record.o:
	g++ record.cpp -o record.o -c 
sqlite_storage.o:
	g++ storage/sqlite_storage.cpp -o storage/sqlite_storage.o -c
mysql_storage.o:
	g++ storage/mysql_storage.cpp -o storage/mysql_storage.o -c
clean:
	rm -rf *.o ./storage/*.o
