#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// Decide what to print for ONE line (already cut at CRLF, CRLF not included).
// Return "OK <method> <target> <version>", "400", "501" or "505".
static std::string judge(const std::string &line) {
    // TODO 1: a CR or LF left inside the line means it was not terminated by CRLF -> "400".
    // TODO 2: exactly three non-empty fields separated by single spaces -> else "400".
    // TODO 3: method only A-Z; target starts with '/' (or is "*" for OPTIONS) and every byte
    //         is 0x21..0x7e; version is HTTP/<digit>.<digit>  -> any failure is "400".
    // TODO 4: method not in GET HEAD POST PUT DELETE OPTIONS PATCH -> "501".
    // TODO 5: version not HTTP/1.0 or HTTP/1.1 -> "505".
    return "OK " + line;
}

int main() {
    // Read every byte of stdin; a char iterator never rewrites \r\n.
    std::istreambuf_iterator<char> begin(std::cin), end;
    std::string data(begin, end);
    size_t start = 0;
    while (true) {
        size_t p = data.find("\r\n", start);
        if (p == std::string::npos) break;
        std::cout << judge(data.substr(start, p - start)) << "\n";
        start = p + 2;
    }
    if (start < data.size()) std::cout << "400\n"; // an unterminated fragment is a 400
    return 0;
}
