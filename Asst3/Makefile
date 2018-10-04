libnetfiles: libnetfiles.c
	gcc -g -Wall -Werror -fsanitize=address libnetfiles.c -o client.out
netfileserver: netfileserver.c
	gcc -g -Wall -Werror -fsanitize=address -pthread netfileserver.c -o server.out
clean:
	rm -rf *.out
