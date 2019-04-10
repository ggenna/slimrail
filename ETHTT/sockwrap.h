#ifndef SOCKWRAP_H
#define SOCKWRAP_H

#define SA struct sockaddr



ssize_t writen (int fd, const void *vptr, size_t n);


int readn (int fd, void *vptr, size_t n);
int Select (int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);
int Socket (int family, int type, int protocol);
void Bind(int sockfd,const SA *myaddr,  socklen_t myaddrlen);
ssize_t Recvfrom (int fd, void *bufptr, size_t nbytes, int flags, SA *sa, socklen_t *salenptr);
void Sendto (int fd, void *bufptr, size_t nbytes, int flags, const SA *sa, socklen_t salen);
void Close (int fd);
#endif
