#include <iostream>
#include <string>
#include <cctype>

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

// Basic pattern matching function
bool simpleMatch(const std::string& input_line, const std::string& pattern) {
    size_t pos = input_line.find(pattern);
    return pos != std::string::npos;
}

// Main pattern matching function
bool matchPattern(const std::string& input_line, const std::string& pattern) {
    if (pattern == "\\d") {
        return containsDigit(input_line);
    } else if (pattern == "\\w") {
        return containsAlphanumeric(input_line);
    } else if (pattern.length() == 1 || (pattern[0] != '[' && pattern[pattern.length() - 1] != ']')) {
        return simpleMatch(input_line, pattern);
    } else {
        // Handle simple character class patterns
        return simpleMatch(input_line, pattern.substr(1, pattern.length() - 2));  // Remove brackets
    }
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

    if (matchPattern(input_line, pattern)) {
        return 0;
    } else {
        return 1;
    }
}
