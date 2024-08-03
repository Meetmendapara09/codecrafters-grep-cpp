#include <iostream>
#include <string>
#include <stack>
#include <unordered_set>
#include <cctype>
#include <stdexcept>

// Check if the input line contains at least one digit
bool matchdigit(const std::string& input_line) {
    for (char ch : input_line) {
        if (std::isdigit(ch)) return true;
    }
    return false;
}

// Check if the input line contains at least one alphanumeric character or underscore
bool matchalphanumeric(const std::string& input_line) {
    for (char ch : input_line) {
        if (std::isalnum(ch) || ch == '_') return true;
    }
    return false;
}

// Handle positive character classes (e.g., [abc], [a-z])
bool positiveMatchGroup(const std::string& input_line, const std::string& pattern, int start, int end) {
    std::unordered_set<char> char_set;
    bool range_mode = false;
    char last_char = '\0';

    for (int i = start; i < end; ++i) {
        char current = pattern[i];
        if (current == '-') {
            range_mode = true;
            continue;
        }
        if (range_mode) {
            // Handle range like a-z
            for (char ch = last_char; ch <= current; ++ch) {
                if (input_line.find(ch) != std::string::npos) return true;
            }
            range_mode = false;
        } else {
            char_set.insert(current);
        }
        last_char = current;
    }
    for (char ch : char_set) {
        if (input_line.find(ch) != std::string::npos) return true;
    }
    return false;
}

// Handle negative character classes (e.g., [^abc], [^a-z])
bool negativeMatchGroup(const std::string& input_line, const std::string& pattern, int start, int end) {
    std::unordered_set<char> char_set;
    bool range_mode = false;
    char last_char = '\0';

    for (int i = start; i < end; ++i) {
        char current = pattern[i];
        if (current == '-') {
            range_mode = true;
            continue;
        }
        if (range_mode) {
            // Handle range like a-z
            for (char ch = last_char; ch <= current; ++ch) {
                if (input_line.find(ch) != std::string::npos) return false;
            }
            range_mode = false;
        } else {
            char_set.insert(current);
        }
        last_char = current;
    }
    for (char ch : char_set) {
        if (input_line.find(ch) != std::string::npos) return false;
    }
    return true;
}

// Match pattern in the input line
bool match(const std::string& input_line, const std::string& pattern) {
    size_t i = 0;
    while (i < input_line.size()) {
        size_t j = 0;
        size_t temp = i;
        while (j < pattern.size() && temp < input_line.size()) {
            if (pattern[j] == '\\') {
                if (j + 1 < pattern.size()) {
                    char next_char = pattern[j + 1];
                    if (next_char == 'd') {
                        if (!std::isdigit(input_line[temp])) break;
                        ++temp;
                    } else if (next_char == 'w') {
                        if (!std::isalnum(input_line[temp]) && input_line[temp] != '_') break;
                        ++temp;
                    } else if (next_char == '[') {
                        size_t end_idx = pattern.find(']', j);
                        if (end_idx == std::string::npos) return false;
                        if (pattern[j + 1] == '^') {
                            return negativeMatchGroup(input_line, pattern, j + 2, end_idx);
                        } else {
                            return positiveMatchGroup(input_line, pattern, j + 1, end_idx);
                        }
                    }
                    j += 2;
                } else break;
            } else {
                if (input_line[temp] != pattern[j]) break;
                ++temp;
                ++j;
            }
        }
        if (j == pattern.size()) return true;
        ++i;
    }
    return false;
}

// Match pattern considering the start of line
bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if (pattern.empty()) return false;

    // Check for Start of Line anchor
    bool starts_with_anchor = (pattern[0] == '^');
    std::string pattern_to_match = starts_with_anchor ? pattern.substr(1) : pattern;

    if (starts_with_anchor) {
        return input_line.find(pattern_to_match) == 0;
    }

    if (pattern.length() == 1) {
        return input_line.find(pattern) != std::string::npos;
    } else if (pattern[0] == '\\') {
        if (pattern.length() == 2) {
            if (pattern[1] == 'd') return matchdigit(input_line);
            if (pattern[1] == 'w') return matchalphanumeric(input_line);
        }
        throw std::runtime_error("Unhandled pattern " + pattern);
    } else if (pattern[0] == '[' && pattern[pattern.length() - 1] == ']') {
        if (pattern[1] == '^') return negativeMatchGroup(input_line, pattern, 0, pattern.size());
        else return positiveMatchGroup(input_line, pattern, 0, pattern.size());
    } else {
        return match(input_line, pattern);
    }
}

int main(int argc, char* argv[]) {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc != 3) {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string flag = argv[1];
    std::string pattern = argv[2];

    if (flag != "-E") {
        std::cerr << "Expected first argument to be '-E'" << std::endl;
        return 1;
    }

    std::string input_line;
    std::getline(std::cin, input_line);

    try {
        if (match_pattern(input_line, pattern)) {
            return 0;
        } else {
            return 1;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
