#include <cstddef>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
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

std::vector<char*> ft_split(char* input, char delimiter) {
    char* inputCopy = new char[strlen(input) + 1];
    strcpy(inputCopy, input);

    std::vector<char*> tokens;
    char* token = strtok(inputCopy, &delimiter);

    while (token != NULL) {
        tokens.push_back(token);
        token = strtok(NULL, &delimiter);
    }

    delete[] inputCopy; 

    return tokens;
}

void removeSpaces(std::string &str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
}

bool nicknameIsValid(std::string nick)
{
    std::string letter = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string special = "[]\\`_^{|}";
    std::string digit = "0123456789";
    std::string minus = "-";
    std::string firstChar = letter + special;
    std::string all = letter + special + digit + minus;
    if (nick.size() > 9)
        return (false);
    if (firstChar.find(nick[0]) == std::string::npos)
        return (false);
    for (int i = 1; nick[i] && i < 9; i++)
    {
        if (all.find(nick[i]) == std::string::npos)
            return (false);
    }
    return (true);
}


bool checkInvalidCharacter(char c)
{
    return (c == '\0' || c == '\a' || c == '\r' || c == '\n' || c == ' ' || c == ',' || c == ':');
}

bool channelNameInvalid(std::string name)
{
    if (name.size() > 50)
        return false;
    std::string::iterator it = name.begin();
    std::string channelId = "#&+";
    if (!name.empty() && channelId.find(name[0]) == std::string::npos)
        return false;
    for (; it != name.end() && !checkInvalidCharacter(*it); it++);
    return (it == name.end());
}
