CFLAGS=-fPIC -shared
LFLAGS=-L/home/philk/src/libcli
LIBS=-ldl -lpthread -lcli -lglib-2.0
INCLUDE=-I/home/philk/src/libcli -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include

.cc.o:
	$(CXX) -fPIC -o $@ -c $< $(INCLUDE)

all: testwrite libdelay.so libpreload.so 

testwrite: testwrite.c
	$(CXX) -o $@ $^

libdelay.so: delay.o
	$(CXX) $(CFLAGS) -o $@ $^ $(INCLUDE) $(LFLAGS) $(LIBS)

libpreload.so: main.o adminserver.o
	$(CXX) $(CFLAGS) -o $@ $^ $(INCLUDE) $(LFLAGS) $(LIBS)

#main.o: main.c
#	$(CC) -c -fPIC -o $@ $^ $(INCLUDE)

#adminserver.o: adminserver.c
#	$(CC) -c -fPIC -o $@ $^ $(INCLUDE)

#read.o: read.c
#	$(CC) -c -fPIC -o $@ $^

#write.o: write.c
#	$(CC) -c -fPIC -o $@ $^
.PHONY: clean
clean:
	rm *.o testwrite libdelay.so libpreload.so
