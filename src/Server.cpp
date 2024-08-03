#include <iostream>
#include <string>
#include <stack>
#include <algorithm>

// Helper functions to match digits and alphanumeric characters
bool matchdigit(const std::string& input_line) {
    for (char c : input_line) {
        if (std::isdigit(c)) return true;
    }
    return false;
}

bool matchalphanumeric(const std::string& input_line) {
    for (char c : input_line) {
        if (std::isalnum(c)) return true;
    }
    return false;
}

// Helper functions for character groups in patterns
bool positiveMatchGroup(const std::string& input_line, const std::string& pattern, int start, int end) {
    std::stack<char> s;
    std::stack<std::pair<char, char>> s_pair;
    if (end >= pattern.size()) end = pattern.size();
    int idx = start, patternSize = end;
    while (idx < patternSize) {
        if (idx != patternSize - 1 && pattern[idx] == '-') {
            idx++;
            char temp = s.top();
            s.pop();
            s_pair.push({temp, pattern[idx]});
        } else {
            s.push(pattern[idx]);
        }
        idx++;
    }
    while (!s.empty()) {
        char temp = s.top();
        s.pop();
        if (input_line.find(temp) != std::string::npos) return true;
    }
    while (!s_pair.empty()) {
        std::pair<char, char> temp = s_pair.top();
        s_pair.pop();
        char temp_1 = temp.first, temp_2 = temp.second;
        for (char ch = temp_1; ch <= temp_2; ch++) {
            if (input_line.find(ch) != std::string::npos) return true;
        }
    }
    return false;
}

bool negativeMatchGroup(const std::string& input_line, const std::string& pattern, int start, int end) {
    std::stack<char> s;
    std::stack<std::pair<char, char>> s_pair;
    if (end >= pattern.length()) end = pattern.length();
    int idx = start + 1, patternSize = end;
    while (idx < patternSize - 1) {
        if (idx != patternSize - 1 && pattern[idx] == '-') {
            idx++;
            char temp = s.top();
            s.pop();
            s_pair.push({temp, pattern[idx]});
        } else {
            s.push(pattern[idx]);
        }
        idx++;
    }
    while (!s.empty()) {
        char temp = s.top();
        s.pop();
        if (input_line.find(temp) != std::string::npos) return false;
    }
    while (!s_pair.empty()) {
        std::pair<char, char> temp = s_pair.top();
        s_pair.pop();
        char temp_1 = temp.first, temp_2 = temp.second;
        for (char ch = temp_1; ch <= temp_2; ch++) {
            if (input_line.find(ch) != std::string::npos) return false;
        }
    }
    return true;
}

// Match function for patterns
bool match(const std::string& input_line, const std::string& pattern) {
    int i = 0;
    bool startAnchor = (pattern[0] == '^');
    bool endAnchor = (pattern[pattern.size() - 1] == '$');
    if (startAnchor) pattern = pattern.substr(1);
    if (endAnchor) pattern = pattern.substr(0, pattern.size() - 1);

    while (i < input_line.size()) {
        int j = 0;
        int temp = i;

        while (j < pattern.size() && temp < input_line.size()) {
            if (pattern[j] == '\\') {
                j++;
                if (j < pattern.size()) {
                    if (pattern[j] == 'd') {
                        if (!isdigit(input_line[temp])) break;
                        temp++;
                    } else if (pattern[j] == 'w') {
                        if (!isalnum(input_line[temp])) break;
                        temp++;
                    } else if (pattern[j] == '[') {
                        int start = j;
                        while (j < pattern.size() && pattern[j] != ']') j++;
                        if (j < pattern.size() && pattern[j] == ']') {
                            if (pattern[start + 1] == '^') {
                                return negativeMatchGroup(input_line, pattern, start, j + 1);
                            } else {
                                return positiveMatchGroup(input_line, pattern, start, j + 1);
                            }
                        }
                    }
                } else {
                    break;
                }
            } else if (pattern[j] == '+') {
                j++;
                while (j < pattern.size() && temp < input_line.size() && pattern[j] != input_line[temp]) {
                    temp++;
                }
            } else if (pattern[j] == '?') {
                if (j > 0 && temp < input_line.size()) {
                    char prevChar = pattern[j - 1];
                    if (input_line[temp] == prevChar || prevChar == '.') {
                        if (match(input_line.substr(temp + 1), pattern.substr(j + 1))) return true;
                    }
                    if (match(input_line.substr(temp), pattern.substr(j + 1))) return true;
                }
                j++;
            } else {
                if (input_line[temp] != pattern[j]) break;
                temp++;
            }
            j++;
        }
        if (j == pattern.size()) return true;
        if (startAnchor && j != pattern.size()) return false;
        if (endAnchor && j != pattern.size() - 1) return false;
        i++;
    }

    return false;
}

// Function to match patterns with different special cases
bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if (pattern.length() == 1) {
        return input_line.find(pattern) != std::string::npos;
    } else if (pattern[0] == '\\' && pattern.length() == 2) {
        switch (pattern[1]) {
            case 'd':
                return matchdigit(input_line);
            case 'w':
                return matchalphanumeric(input_line);
            default:
                throw std::runtime_error("Unhandled pattern " + pattern);
        }
    } else if (pattern[0] == '[' && pattern[pattern.length() - 1] == ']') {
        if (pattern[1] == '^') {
            return negativeMatchGroup(input_line, pattern, 0, pattern.size());
        } else {
            return positiveMatchGroup(input_line, pattern, 0, pattern.size());
        }
    } else {
        return match(input_line, pattern);
    }
}

int main(int argc, char* argv[]) {
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
