CC=gcc

server:
	gcc dhcp_server.c -o dserver

client:
	gcc dhcp_client.c -o dclient
clean:
	rm dserver dclient
