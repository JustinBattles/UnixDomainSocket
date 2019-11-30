#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define BUFSIZE 4096
/**
 * This function should fork(), and execlp() the clear Unix command in the child
 * and wait for the child process to complete in the parent.
 */


struct sockaddr_un addr;


void clear(void)
{
  pid_t pid = fork();

  if (pid < 0) {
    perror("fork");
    exit(1);
  }

  if (pid > 0) waitpid(pid, NULL, 0);
  else execlp("/usr/bin/clear", "clear", NULL);
}

/**
 * This should be a signal handler to clean up the socket file and exit when
 * at a minimum Ctl-C is pressed.
 */
void cleanup(int sig)
{
  unlink(addr.sun_path);
  printf("\n");
  exit(1);
}


/**
 * Make a program that works just like listen.c, done in class, except that
 * it uses a Unix Domain socket to listen on. The default socket path should be
 * /tmp/listen, but may be altered by providing a path given by the command line.
 * After setting up the socket it should clear the screen and print:
 * Login to <hostname> and say at <socket_path>
 * 
 * It should then read messages from the socket continuously and print them
 * to the stdout.
 */

int main(int argc, char *argv[])
{
  char buf[BUFSIZE], *path = "/tmp/listen";
  int r;

  signal(SIGINT, &cleanup);

  if (argc > 1) path = argv[1];

  if (access(path, F_OK) != -1) {
    printf("Listen is already running, exiting.\n");
    exit(1);
  }

  chmod(path, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);

  int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, path);

  unlink(path);

  if (bind(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) < 0) {
    perror("bind");
    exit(1);
  }

  clear();

     printf("Log in to %s and say at %s\n", getenv("HOSTNAME"), path);

  while (1) {
  while ((r = read(sock, buf, BUFSIZE)) > 0) {
    write(STDOUT_FILENO, buf, r);
  }
  }

  return 0;
}
