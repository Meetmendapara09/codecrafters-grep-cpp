#include <iostream>
#include <string>
#include <regex>
#include <cctype>
#include <stdexcept>

// Helper functions to match digits and alphanumeric characters
bool matchdigit(const std::string& input_line) {
    return std::regex_search(input_line, std::regex("\\d"));
}

bool matchalphanumeric(const std::string& input_line) {
    return std::regex_search(input_line, std::regex("\\w"));
}

// Helper functions for character groups in patterns
bool negativeMatchGroup(const std::string& input_line, const std::string& pattern, int start, int end) {
    std::string chars = pattern.substr(start + 1, end - start - 2);
    for (char ch : chars) {
        if (input_line.find(ch) != std::string::npos) {
            return false;
        }
    }
    return true;
}

bool positiveMatchGroup(const std::string& input_line, const std::string& pattern, int start, int end) {
    std::string chars = pattern.substr(start + 1, end - start - 2);
    for (char ch : chars) {
        if (input_line.find(ch) != std::string::npos) {
            return true;
        }
    }
    return false;
}

// Match function for patterns
bool match(const std::string& input_line, const std::string& pattern) {
    int i = 0;
    bool startAnchor = (pattern[0] == '^');
    bool endAnchor = (pattern[pattern.size() - 1] == '$');
    std::string adjustedPattern = pattern;
    
    if (startAnchor) adjustedPattern = pattern.substr(1);
    if (endAnchor) adjustedPattern = pattern.substr(0, pattern.size() - 1);

    while (i < input_line.size()) {
        int j = 0;
        int temp = i;

        while (j < adjustedPattern.size() && temp < input_line.size()) {
            if (adjustedPattern[j] == '\\') {
                j++;
                if (j < adjustedPattern.size()) {
                    if (adjustedPattern[j] == 'd') {
                        if (!std::isdigit(input_line[temp])) break;
                        temp++;
                    } else if (adjustedPattern[j] == 'w') {
                        if (!std::isalnum(input_line[temp])) break;
                        temp++;
                    } else if (adjustedPattern[j] == '[') {
                        int start = j;
                        while (j < adjustedPattern.size() && adjustedPattern[j] != ']') j++;
                        if (j < adjustedPattern.size() && adjustedPattern[j] == ']') {
                            if (adjustedPattern[start + 1] == '^') {
                                return negativeMatchGroup(input_line, adjustedPattern, start, j + 1);
                            } else {
                                return positiveMatchGroup(input_line, adjustedPattern, start, j + 1);
                            }
                        }
                    }
                } else {
                    break;
                }
            } else if (adjustedPattern[j] == '+') {
                j++;
                while (j < adjustedPattern.size() && temp < input_line.size() && adjustedPattern[j] != input_line[temp]) {
                    temp++;
                }
            } else if (adjustedPattern[j] == '?') {
                if (j > 0 && temp < input_line.size()) {
                    char prevChar = adjustedPattern[j - 1];
                    if (match(input_line.substr(temp + 1), adjustedPattern.substr(j + 1))) return true;
                    if (input_line[temp] == prevChar || prevChar == '.') {
                        if (match(input_line.substr(temp + 1), adjustedPattern.substr(j + 1))) return true;
                    }
                    if (match(input_line.substr(temp), adjustedPattern.substr(j + 1))) return true;
                }
                j++;
            } else {
                if (input_line[temp] != adjustedPattern[j]) break;
                temp++;
            }
            j++;
        }
        if (j == adjustedPattern.size()) return true;
        if (startAnchor && j != adjustedPattern.size()) return false;
        if (endAnchor && j != adjustedPattern.size() - 1) return false;
        i++;
    }

    return false;
}

// Function to match patterns with different special cases
bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if (pattern.length() == 1) {
        return input_line.find(pattern) != std::string::npos;
    } else if (pattern == "\\d") {
        return matchdigit(input_line);
    } else if (pattern == "\\w") {
        return matchalphanumeric(input_line);
    } else if (pattern[0] == '[' && pattern[pattern.length() - 1] == ']') {
        if (pattern[1] == '^') {
            return negativeMatchGroup(input_line, pattern, 0, pattern.size());
        } else {
            return positiveMatchGroup(input_line, pattern, 0, pattern.size());
        }
    } else {
        try {
            std::regex regex(pattern);
            return std::regex_search(input_line, regex);
        } catch (const std::regex_error&) {
            return false;
        }
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

    if (match_pattern(input_line, pattern)) {
        return 0;
    } else {
        return 1;
    }
}
