#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <algorithm>
#include <string>
#include <vector>

using std::string;

inline void remove_char(string &s, char c)
{
    s.erase((std::remove(s.begin(), s.end(), c)), s.end());
}

inline std::vector<string> split(const string &s, char c)
{
    std::vector<string> v;
    string::size_type i = 0;
    string::size_type j = s.find(c);

    while (j != string::npos) {
        v.push_back(s.substr(i, j-i));
        i = ++j;
        j = s.find(c, j);

        if (j == string::npos)
            v.push_back(s.substr(i, s.length()));
    }
    return v;
}

inline void tolowercase(string &s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}
#endif // STRINGUTILS_H
