#include "SimpleServer.hpp"

#include <fcntl.h>
#include <sys/socket.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
SimpleServer::SimpleServer(): mFd(0) {
}

SimpleServer::~SimpleServer() {
  if (mFd > 0) {
    ::close(mFd);
  }
}

int
SimpleServer::start(const char* sockname) {
  int sfd = socket(PF_INET,SOCK_STREAM,0);;//socket(AF_UNIX, SOCK_STREAM, 0);

  if (sfd < 0) {
    return sfd;
  }

  //struct sockaddr_un addr;
  //memset(&addr, 0, sizeof(addr));    
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));   
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htons(INADDR_ANY);
  addr.sin_port = htons(9999);

  //addr.sun_family = AF_UNIX;
  //strncpy(&addr.sun_path[1], sockname, strlen(sockname));
  int flag=1,len=sizeof(int); 
  if( setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &flag, len) == -1) {
    return -1;
  } 
  //if (::bind(sfd, (struct sockaddr*) &addr,
  //    sizeof(sa_family_t) + strlen(sockname) + 1) < 0) {
  if (::bind(sfd, (struct sockaddr*) &addr, sizeof(sockaddr_in)) < 0) {
    ::close(sfd);
    return -1;
  }

  ::listen(sfd, 1);

  mFd = sfd;

  return mFd;
}

int
SimpleServer::accept() {
  struct sockaddr_un addr;
  socklen_t addr_len = sizeof(addr);
  return ::accept(mFd, (struct sockaddr *) &addr, &addr_len);
}
