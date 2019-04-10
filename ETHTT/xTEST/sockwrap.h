#ifndef SOCKWRAP_H
#define SOCKWRAP_H

#define SA struct sockaddr

int Socket (int family, int type, int protocol);

void Bind (int sockfd, const SA *myaddr, socklen_t myaddrlen);

void Listen (int sockfd, int backlog);

int Accept (int listen_sockfd, SA *cliaddr, socklen_t *addrlenp);

void Connect (int sockfd, const SA *srvaddr, socklen_t addrlen);

void Close (int fd);

void Shutdown (int fd, int howto);

ssize_t Read (int fd, void *bufptr, size_t nbytes);

void Write (int fd, void *bufptr, size_t nbytes);

ssize_t Recvfrom (int fd, void *bufptr, size_t nbytes, int flags, SA *sa, socklen_t *salenptr);

ssize_t Recvfrom_timeout (int fd, void *bufptr, size_t nbytes, int flags, SA *sa, socklen_t *salenptr, int timeout);

void Sendto (int fd, void *bufptr, size_t nbytes, int flags, const SA *sa, socklen_t salen);

void Inet_aton (const char *strptr, struct in_addr *addrptr);

ssize_t readn (int fd, void *vptr, size_t n);

ssize_t Readn (int fd, void *ptr, size_t nbytes);

ssize_t readline (int fd, void *vptr, size_t maxlen);

ssize_t Readline (int fd, void *ptr, size_t maxlen);

ssize_t writen(int fd, const void *vptr, size_t n);

void Writen (int fd, void *ptr, size_t nbytes);

//int Select (int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);

pid_t Fork (void);

struct hostent *Gethostbyname (const char *hostname);

void Getsockname (int sockfd, struct sockaddr *localaddr, socklen_t *addrp);
#endif
