#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <vector>


std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> fields;
    std::size_t start = 0;

    while (true) {
        std::size_t pos = s.find(delim, start);

        if (pos == std::string::npos) {
            fields.push_back(s.substr(start));
            break;
        }

        fields.push_back(s.substr(start, pos - start));
        start = pos + 1;
    }

    return fields;
}

static bool is_valid_method_syntax(const std::string& method) {
    return !method.empty() &&
        std::all_of(method.begin(), method.end(),
            [](unsigned char c) {
                return c >= 'A' && c <= 'Z';
            });
}

static bool is_valid_target(const std::string& method,
                            const std::string& target) {
    if (target == "*") {
        return method == "OPTIONS";
    }

    return !target.empty() &&
        target[0] == '/' &&
        std::all_of(target.begin(), target.end(),
            [](unsigned char c) {
                return c >= 0x21 && c <= 0x7e;
            });
}

static bool is_valid_version_syntax(const std::string& version) {
    return version.size() == 8 &&
        version.compare(0, 5, "HTTP/") == 0 &&
        version[5] >= '0' && version[5] <= '9' &&
        version[6] == '.' &&
        version[7] >= '0' && version[7] <= '9';
}

static std::string judge(const std::string& line) {
    // 1. Reject leftover CR or LF.
    if (line.find_first_of("\r\n") != std::string::npos) {
        return "400";
    }

    // 2. Require exactly three non-empty fields.
    const auto fields = split(line, ' ');

    if (fields.size() != 3 ||
        fields[0].empty() ||
        fields[1].empty() ||
        fields[2].empty()) {
        return "400";
    }

    const auto& method = fields[0];
    const auto& target = fields[1];
    const auto& version = fields[2];

    // 3. Validate syntax.
    if (!is_valid_method_syntax(method) ||
        !is_valid_target(method, target) ||
        !is_valid_version_syntax(version)) {
        return "400";
    }

    // 4. Check supported methods.
    static const std::vector<std::string> valid_methods = {
        "GET", "HEAD", "POST", "PUT",
        "DELETE", "OPTIONS", "PATCH"
    };

    if (std::find(valid_methods.begin(), valid_methods.end(), method)
        == valid_methods.end()) {
        return "501";
    }

    // 5. Check supported versions.
    if (version != "HTTP/1.0" && version != "HTTP/1.1") {
        return "505";
    }

    return "OK " + method + " " + target + " " + version;
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
