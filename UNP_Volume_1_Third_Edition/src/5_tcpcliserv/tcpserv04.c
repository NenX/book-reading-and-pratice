#include "unp.h"
#include "wrapper.h"
#include <time.h>
#include "readn.h"
#include "writen.h"

void sig_chld(int signo)
{
   int status;
   int pid;
   while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
   {
      printf("pid %d terminated.\n", pid);
   }
}

void str_echo(int connfd)
{
   char buf[MAXLINE];
   char buf2[MAXLINE];
   int n;
   int pid = getpid();
   while (1)
   {
   again:
      n = read(connfd, buf, MAXLINE);
      if (n < 0)
      {
         if (errno == EINTR)
            goto again;
         err_sys("str_echo read");
      }
      else if (n == 0)
      {
         break;
      }
      buf[n] = 0;
      snprintf(buf2, MAXLINE, "pid %d:%s",pid, buf);
      writen(connfd, buf2, strlen(buf2));
      writen(STDOUT_FILENO, buf2, strlen(buf2));

   }
}

int main(int argc, char **argv)
{
   int listenfd, connfd, pid;
   struct sockaddr_in servaddr, cliaddr;
   socklen_t len;

   bzero(&servaddr, sizeof(servaddr));

   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(SERV_PORT);
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

   listenfd = Socket(AF_INET, SOCK_STREAM, 0);

   Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

   Listen(listenfd, LISTENQ);
   Signal(SIGCHLD, sig_chld);
   while (1)
   {
      len = sizeof(cliaddr);
      connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);

      if (connfd < 0)
      {
         if (errno == EINTR)
            continue;
         err_sys("accept failed!");
      }
      if ((pid = Fork()) == 0)
      {
         Close(listenfd);
         str_echo(connfd);
         exit(0);
      }
      close(connfd);
      printf("pid: %d\n", pid);
   }
}