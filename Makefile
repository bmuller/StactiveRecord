main: utils.o sar_dbi.o record.o sqlite_storage.o
	g++ test.cpp utils.o storage/sar_dbi.o record.o storage/sqlite_storage.o -o test -lsqlite3
utils.o:
	g++ utils.cpp -o utils.o -c
sar_dbi.o:
	g++ storage/sar_dbi.cpp -o storage/sar_dbi.o -c
record.o:
	g++ record.cpp -o record.o -c 
sqlite_storage.o:
	g++ storage/sqlite_storage.cpp -o storage/sqlite_storage.o -c
clean:
	rm -rf *.o ./storage/*.o ./types/*.o test