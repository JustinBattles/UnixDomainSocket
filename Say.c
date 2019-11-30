#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/un.h>


#define BUFSIZE 4096


/**
 * This program should create a socket, then:
 * - Read lines of input from the user and send them to the listen program (via
 *   sendto(2)) using the socket file path, which is by default /tmp/listen, but
 *   may be modified by a path given on the command line.
 * - Print the message "Type Ctrl-D to quit:\n" before accepting input.
 * - The program should stop when the end of file on stdin is reached.
 */

int main(int argc, char *argv[])
{


  struct sockaddr_un addr;
  char buf[BUFSIZE], *path = "/tmp/listen";
  int r,w,t;

  if (argc > 1) path = argv[1];

  if (access(path, F_OK) == -1) {
    printf("No one is listening\n");
    exit(1);
  }
  int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, path);

  printf("Write your message, type Ctrl-D to quit\n");
  while ((r = read(STDIN_FILENO, buf, BUFSIZE)) > 0) {
    t = 0;

    do {
     w = sendto(sock, buf, r, MSG_CONFIRM, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
     t += w;
  } while (t < r);

 }


  return 0;
}
