#ifndef HELPER_UTILS
#define HELPER_UTILS

#include <algorithm>
#include <string>

namespace helpers {
    std::string str_tolower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return std::tolower(c);}
        );
        return s;
    }
}
#endif // !HELPER_UTILS

