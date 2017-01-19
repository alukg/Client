CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lboost_locale -lboost_thread

all: Client
	g++ -o bin/TFTPclient bin/ConnectionHandler.o bin/Client.o bin/ACK.o bin/BCAST.o bin/DATA.o bin/DELRQ.o bin/ERROR.o bin/LOGRQ.o bin/Packet.o bin/RRQ.o bin/WRQ.o $(LDFLAGS)

Client: bin/ConnectionHandler.o bin/Client.o bin/ACK.o bin/BCAST.o bin/DATA.o bin/DELRQ.o bin/ERROR.o bin/LOGRQ.o bin/Packet.o bin/RRQ.o bin/WRQ.o
	
bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/Client.o: src/Client.cpp
	g++ $(CFLAGS) -o bin/Client.o src/Client.cpp

bin/ACK.o: src/packets/ACK.cpp
	g++ $(CFLAGS) -o bin/ACK.o src/packets/ACK.cpp

bin/BCAST.o: src/packets/BCAST.cpp
	g++ $(CFLAGS) -o bin/BCAST.o src/packets/BCAST.cpp

bin/DATA.o: src/packets/DATA.cpp
	g++ $(CFLAGS) -o bin/DATA.o src/packets/DATA.cpp

bin/DELRQ.o: src/packets/DELRQ.cpp
	g++ $(CFLAGS) -o bin/DELRQ.o src/packets/DELRQ.cpp

bin/ERROR.o: src/packets/ERROR.cpp
	g++ $(CFLAGS) -o bin/ERROR.o src/packets/ERROR.cpp

bin/LOGRQ.o: src/packets/LOGRQ.cpp
	g++ $(CFLAGS) -o bin/LOGRQ.o src/packets/LOGRQ.cpp

bin/Packet.o: src/packets/Packet.cpp
	g++ $(CFLAGS) -o bin/Packet.o src/packets/Packet.cpp

bin/RRQ.o: src/packets/RRQ.cpp
	g++ $(CFLAGS) -o bin/RRQ.o src/packets/RRQ.cpp

bin/WRQ.o: src/packets/WRQ.cpp
	g++ $(CFLAGS) -o bin/WRQ.o src/packets/WRQ.cpp

.PHONY: clean
clean:
	rm -f bin/*
