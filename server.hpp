#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <stdexcept>
#include <poll.h>
#include <fcntl.h>

#define MAX_CLIENTS 100

void ft_bzero(void* s, std::size_t n);
void ft_bcopy(const void* src, void* dest, std::size_t n);
char	**ft_split(char const *s, char c);