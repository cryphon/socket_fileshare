#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define SIZE 2048

void write_file(int socket) {
  int i;
  FILE *file;
  char *filename = "received_file.txt";
  char buff[SIZE];

  file = fopen(filename, "w");
  i = recv(socket, buff, SIZE, 0);
  printf("i:     %d", i);
  if (i <= 0) {
    perror("error reading file from buffer");
    return;
  }
  fprintf(file, "%s", buff);
  bzero(buff, SIZE);
}

int main(void) {

  char buff[SIZE];

  // create network socket
  int network_socket;
  network_socket = socket(AF_INET, SOCK_STREAM, 0); // std TCP socket

  // spec addr for receiving socket
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;         // type of address
  server_address.sin_port = htons(9002);       // port to be conn to remotely
  server_address.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY == 0.0.0.0 local

  int conn_stat = connect(network_socket, (struct sockaddr *)&server_address,
                          sizeof(server_address));

  if (conn_stat == -1) {
    printf("unexpected error making connection");
  }

  write_file(network_socket);
  return 0;
}
