#include <cstddef>
#include <iostream>
#include <cstring>
#include <vector>
void ft_bzero(void* s, std::size_t n) {
    char* p = static_cast<char*>(s);
    while (n--) {
        *p++ = '\0';
    }
}

void ft_bcopy(const void* src, void* dest, std::size_t n) {
    const char* src_ptr = static_cast<const char*>(src);
    char* dest_ptr = static_cast<char*>(dest);

    while (n--) {
        *dest_ptr++ = *src_ptr++;
    }
}

/*std::vector<std::string> split(const char* input, char delimiter) {
    std::vector<std::string> tokens;
    char* token = std::strtok(const_cast<char*>(input), &delimiter);
    
    while (token != NULL) {
        tokens.push_back(std::string(token));
        token = std::strtok(NULL, &delimiter);
    }
    
    return tokens;
}*/