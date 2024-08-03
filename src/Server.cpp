#include <iostream>
#include <string>
#include <cctype>
#include <stack>
#include <utility>

// Helper function to match if input contains a digit
bool containsDigit(const std::string& input_line) {
    for (char c : input_line) {
        if (std::isdigit(c)) return true;
    }
    return false;
}

// Helper function to match if input contains alphanumeric characters
bool containsAlphanumeric(const std::string& input_line) {
    for (char c : input_line) {
        if (std::isalnum(c)) return true;
    }
    return false;
}

// Match function for character groups in patterns
bool matchGroup(const std::string& input_line, const std::string& pattern, int start, int end, bool isNegative) {
    std::stack<char> s;
    std::stack<std::pair<char, char>> s_pair;
    if (end >= pattern.size()) end = pattern.size();
    int idx = start + 1;

    while (idx < end - 1) {
        if (pattern[idx] == '-') {
            idx++;
            if (!s.empty()) {
                char temp = s.top();
                s.pop();
                s_pair.push({temp, pattern[idx]});
            }
        } else {
            s.push(pattern[idx]);
        }
        idx++;
    }

    while (!s.empty()) {
        char temp = s.top();
        s.pop();
        if (input_line.find(temp) != std::string::npos) return !isNegative;
    }

    while (!s_pair.empty()) {
        auto [temp_1, temp_2] = s_pair.top();
        s_pair.pop();
        for (char ch = temp_1; ch <= temp_2; ++ch) {
            if (input_line.find(ch) != std::string::npos) return !isNegative;
        }
    }

    return isNegative;
}

// Main pattern matching function
bool matchPattern(const std::string& input_line, const std::string& pattern) {
    if (pattern == "\\d") return containsDigit(input_line);
    if (pattern == "\\w") return containsAlphanumeric(input_line);

    size_t pos = pattern.find('[');
    if (pos != std::string::npos && pattern.back() == ']') {
        bool isNegative = pattern[pos + 1] == '^';
        return matchGroup(input_line, pattern, pos, pattern.size(), isNegative);
    }

    size_t pattern_size = pattern.size();
    size_t input_size = input_line.size();
    
    for (size_t i = 0; i <= input_size - pattern_size; ++i) {
        if (input_line.substr(i, pattern_size) == pattern) return true;
    }

    return false;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " -E <pattern>" << std::endl;
        return 1;
    }

    std::string flag = argv[1];
    std::string pattern = argv[2];
    if (flag != "-E") {
        std::cerr << "First argument must be '-E'" << std::endl;
        return 1;
    }

    std::string input_line;
    std::getline(std::cin, input_line);

    try {
        if (matchPattern(input_line, pattern)) {
            return 0;
        } else {
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
