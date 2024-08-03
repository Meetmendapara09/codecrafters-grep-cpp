#include <iostream>
#include <string>
#include <stack>
#include <algorithm>

// Utility functions for checking digits and alphanumeric characters
bool matchdigit(const std::string& input_line) {
    for (char ch : input_line) {
        if (std::isdigit(ch)) return true;
    }
    return false;
}

bool matchalphanumeric(const std::string& input_line) {
    for (char ch : input_line) {
        if (std::isalnum(ch)) return true;
    }
    return false;
}

// Function to match a positive character group in the pattern
bool positiveMatchGroup(const std::string& input_line, const std::string& pattern, int start, int end) {
    std::stack<char> s;
    std::stack<std::pair<char, char>> s_pair;

    if (end >= pattern.size()) end = pattern.size();
    int idx = start + 1;  // Start after '['
    while (idx < end) {
        if (idx < end - 1 && pattern[idx] == '-') {
            idx++;
            char temp = s.top();
            s.pop();
            s_pair.push({ temp, pattern[idx] });
        } else {
            s.push(pattern[idx]);
        }
        idx++;
    }

    // Check for individual characters
    while (!s.empty()) {
        char temp = s.top();
        s.pop();
        if (input_line.find(temp) != std::string::npos) return true;
    }

    // Check for ranges
    while (!s_pair.empty()) {
        auto temp = s_pair.top();
        s_pair.pop();
        for (char ch = temp.first; ch <= temp.second; ++ch) {
            if (input_line.find(ch) != std::string::npos) return true;
        }
    }

    return false;
}

// Function to match a negative character group in the pattern
bool negativeMatchGroup(const std::string& input_line, const std::string& pattern, int start, int end) {
    std::stack<char> s;
    std::stack<std::pair<char, char>> s_pair;

    if (end >= pattern.length()) end = pattern.length();
    int idx = start + 1;  // Start after '[^'
    while (idx < end) {
        if (idx < end - 1 && pattern[idx] == '-') {
            idx++;
            char temp = s.top();
            s.pop();
            s_pair.push({ temp, pattern[idx] });
        } else {
            s.push(pattern[idx]);
        }
        idx++;
    }

    // Check for individual characters
    while (!s.empty()) {
        char temp = s.top();
        s.pop();
        if (input_line.find(temp) != std::string::npos) return false;
    }

    // Check for ranges
    while (!s_pair.empty()) {
        auto temp = s_pair.top();
        s_pair.pop();
        for (char ch = temp.first; ch <= temp.second; ++ch) {
            if (input_line.find(ch) != std::string::npos) return false;
        }
    }

    return true;
}

// Function to match the pattern against the input line
bool match(const std::string& input_line, const std::string& pattern) {
    int i = 0;
    bool startAnchor = (pattern[0] == '^');
    bool endAnchor = (pattern.back() == '$');
    
    // Adjust pattern and input for anchors
    if (startAnchor) {
        i = 1;  // Skip '^'
    }
    if (endAnchor) {
        if (input_line.size() < pattern.size() - 1) return false;
        // Match from end of input
        i = input_line.size() - (pattern.size() - 1);
    }
    
    int temp = i;

    while (temp <= input_line.size()) {
        int j = (startAnchor ? 1 : 0);  // Skip '^' if it's a start anchor
        int pattern_length = (endAnchor ? pattern.size() - 1 : pattern.size());  // Exclude '$' if it's an end anchor
        
        while (j < pattern_length && temp < input_line.size()) {
            if (pattern[j] == '\\') {
                j++;
                if (j < pattern.size()) {
                    if (pattern[j] == 'd') {
                        if (!std::isdigit(input_line[temp])) break;
                        temp++;
                    } else if (pattern[j] == 'w') {
                        if (!std::isalnum(input_line[temp])) break;
                        temp++;
                    } else if (pattern[j] == '[') {
                        int start = j;
                        while (j < pattern.size() && pattern[j] != ']') j++;
                        if (j < pattern.size()) {
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
            } else {
                if (input_line[temp] != pattern[j]) break;
                temp++;
            }
            j++;
        }
        
        if (j == pattern_length) return true;
        if (startAnchor && j != pattern_length) return false;
        if (endAnchor) {
            if (j == pattern_length - 1) return true;
            return false;
        }
        temp++;
    }

    return false;
}

// Function to handle different types of patterns
bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if (pattern.length() == 1) {
        return input_line.find(pattern) != std::string::npos;
    } else if (pattern[0] == '\\' && pattern.length() == 2) {
        switch (pattern[1]) {
            case 'd': return matchdigit(input_line);
            case 'w': return matchalphanumeric(input_line);
            default: throw std::runtime_error("Unhandled pattern " + pattern);
        }
    } else if (pattern[0] == '[' && pattern[pattern.length() - 1] == ']') {
        if (pattern[1] == '^') {
            return negativeMatchGroup(input_line, pattern, 0, pattern.size());
        } else {
            return positiveMatchGroup(input_line, pattern, 0, pattern.size());
        }
    } else if (pattern.length() > 1) {
        return match(input_line, pattern);
    } else {
        throw std::runtime_error("Unhandled pattern " + pattern);
    }
}

// Main function
int main(int argc, char* argv[]) {
    std::cout << "Logs from your program will appear here" << std::endl;
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
