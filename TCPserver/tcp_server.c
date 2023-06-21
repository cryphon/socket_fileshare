#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#define SIZE 1024

void send_file(FILE *file, int socket) {
  char data[SIZE] = {0};

  while (fgets(data, SIZE, file) != NULL) {
    if (send(socket, data, sizeof(data), 0) == -1) {
      perror("[-]Error sending file.");
      exit(1);
    }
    bzero(data, SIZE);
  }
}

int check_file_input(const char *filepath) {
  struct stat st;

  if (stat(filepath, &st) < 0) {
    printf("No file at location:\t%s\n", filepath);
    exit(1);
  }
  return S_ISREG(st.st_mode);
}

int init_server_socket() {

  // create server_socket
  int server_socket;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  if (server_socket < 0) {
    perror("[-]Error in socket\n");
    exit(1);
  }
  printf("[+]Server socket created succesfully\n");

  // def server_address
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(9002);
  server_address.sin_addr.s_addr = INADDR_ANY;

  // bind sock to specified IP and port
  int bind_response = bind(server_socket, (struct sockaddr *)&server_address,
                           sizeof(server_address));

  if (bind_response == -1) {
    perror("[-]Error in bind\n");
    exit(1);
  }
  printf("[+]Binding succesful\n");

  // 3 = num of connections before getting refused
  if (listen(server_socket, 3) == 0) {
    printf("[+]Listening...\n");
  } else {
    perror("[-]Error while listening\n");
    exit(1);
  }
  return server_socket;
}

int main(int argc, char **argv) {

  if (argc == 1) {
    printf("Error: No flags provided\n");
    exit(-1);
  }

  int client_socket;
  int server_socket = init_server_socket();

  FILE *file;
  char *file_path = (char *)malloc(SIZE);
  int i;
  for (i = 1; i < argc; i++) {
    /* Check for a switch (leading "-"). */
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'f':
        if (!argv[i + 1]) {
          printf("No file provided\n");
          exit(-1);
        }
        char *temp_file_path = argv[i + 1];
        strcpy(file_path, "server_message.txt");
        break;
      case 'b':
        printf("flag b was called\n");
        break;
      }
    }
  }

  for (;;) {
    struct sockaddr_in addr;
    socklen_t addr_len;
    char client_address[1024];

    client_socket = accept(server_socket, (struct sockaddr *)&addr, &addr_len);

    inet_ntop(AF_INET, &addr, client_address, 1024);
    printf("Client connection: %s\n", client_address);

    file = fopen(file_path, "r");
    if (file == NULL) {
      printf("[-]Error in reading file: %s", file_path);
      exit(1);
    }

    send_file(file, client_socket);
  }
  return 0;
}
