#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define PUBLIC_G 15
#define MOD_P 97

#define HOSTNAME "172.26.37.44"
#define PORT 7800

#define USERNAME "voa1"

int modExp(int x, unsigned int y, int p);

int main(int argc, char ** argv) {
  int sockfd, n, secret_b;
  struct sockaddr_in serv_addr;
  struct hostent * server;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <secret b>\n", argv[0]);
    exit(0);
  }

  secret_b = atoi(argv[1]);

  char buffer[256];

  server = gethostbyname(HOSTNAME);

  if (server == NULL) {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }

  /* Building data structures for socket */
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy(server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr,
    server->h_length);
  serv_addr.sin_port = htons(PORT);

  // create TCP socket -- active open
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(0);
  }

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR connecting");
    exit(0);
  }

  // send username
  printf("Sending username: %s\n", USERNAME);
  bzero(buffer, 256);
  sprintf(buffer, "%s\n", USERNAME);
  n = write(sockfd, buffer, strlen(buffer));
  if (n < 0) {
    perror("ERROR writing to socket");
    exit(0);
  }

  // send g^b mod p as text
  int gbp = modExp(PUBLIC_G, secret_b, MOD_P);
  printf("Sending g^b mod p: %d\n", gbp);
  bzero(buffer, 256);
  sprintf(buffer, "%d\n", gbp);
  n = write(sockfd, buffer, strlen(buffer));
  if (n < 0) {
    perror("ERROR writing to socket");
    exit(0);
  }

  // wait for reply
  bzero(buffer, 256);
  n = read(sockfd, buffer, 255);
  if (n < 0) {
    perror("ERROR reading from socket");
    exit(0);
  }
  printf("g^a mod p received: %s", buffer);

  // calculate g^(ab) mod p and send it back as text
  int gbap = modExp(atoi(buffer), secret_b, MOD_P);
  printf("Sending g^(ab) mod p: %d\n", gbap);
  bzero(buffer, 256);
  sprintf(buffer, "%d\n", gbap);
  n = write(sockfd, buffer, strlen(buffer));

  // wait for reply and print status report
  bzero(buffer, 256);
  n = read(sockfd, buffer, 255);
  if (n < 0) {
    perror("ERROR reading from socket");
    exit(0);
  }
  printf("Status report: %s", buffer);

  return 0;
}

// from https://www.geeksforgeeks.org/modular-exponentiation-power-in-modular-arithmetic/
// calculates x^y mod p with modular exponentiation
int modExp(int x, unsigned int y, int p) {
  int res = 1;
  x = x % p;
  while (y > 0) {
    if (y & 1) {
      res = (res*x) % p;
    }

    y = y>>1;
    x = (x*x) % p;
  }
  return res;
}
