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

std::vector<char*> ft_split(const char* input, char delimiter) {
    // Copie de la chaîne d'entrée dans une mémoire tampon
    char* inputCopy = new char[strlen(input) + 1];
    strcpy(inputCopy, input);

    std::vector<char*> tokens;
    char* token = strtok(inputCopy, &delimiter);

    while (token != NULL) {
        tokens.push_back(token);
        token = strtok(NULL, &delimiter);
    }

    delete[] inputCopy; // Libérer la mémoire de la copie

    return tokens;
}