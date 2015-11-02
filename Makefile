
all: server client

server:
	g++ -o bin/server src/MemLeaseServer.cpp src/MemProtocol.cpp src/TcpServer.cpp src/LeasedMemoryContainer.cpp

client:
	g++ -o bin/cient src/MemLeaseClient.cpp src/MemProtocol.cpp src/TcpClient.cpp 
