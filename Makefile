CFLAGS=-fPIC -shared
LFLAGS=-L/home/philk/src/libcli
LIBS=-ldl -lpthread -lcli
INCLUDE=-I/home/philk/src/libcli

.c.o:
	$(CC) -fPIC -o $@ -c $< $(INCLUDE)

all: testwrite libpreload.so

testwrite: testwrite.c
	$(CC) -o $@ $^

libpreload.so: main.o adminserver.o read.o write.o
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDE) $(LFLAGS) $(LIBS)

#main.o: main.c
#	$(CC) -c -fPIC -o $@ $^ $(INCLUDE)

#adminserver.o: adminserver.c
#	$(CC) -c -fPIC -o $@ $^ $(INCLUDE)

#read.o: read.c
#	$(CC) -c -fPIC -o $@ $^

#write.o: write.c
#	$(CC) -c -fPIC -o $@ $^
