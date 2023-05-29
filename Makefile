
all: servidor cliente obs

servidor: clean server.cpp
	g++ server.cpp -o servidor

cliente: clean cliente.cpp
	g++ cliente.cpp -o cliente

obs:
	@echo "Para ejecutar el servidor usa: ./servidor <puerto>"
	@echo "Para ejecutar el cliente usa: ./cliente <127.0.0.1> <puerto>"

clean:
	rm -f servidor cliente

	

