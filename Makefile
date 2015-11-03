all: server client
server:
	mkdir -p bin
	g++ -o bin/server src/MemLeaseServer.cpp src/MemProtocol.cpp src/TcpServer.cpp src/LeasedMemoryContainer.cpp
client:
	g++ -o bin/client src/MemLeaseClient.cpp src/MemProtocol.cpp src/TcpClient.cpp 
clean:
	rm  bin/*

