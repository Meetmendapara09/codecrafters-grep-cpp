#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>

// Function to check if the input line contains digits
bool matchdigit(const std::string& input_line) {
    for (char c : input_line) {
        if (std::isdigit(c)) return true;
    }
    return false;
}

// Function to check if the input line contains alphanumeric characters
bool matchalphanumeric(const std::string& input_line) {
    for (char c : input_line) {
        if (std::isalnum(c)) return true;
    }
    return false;
}

// Function to match character classes in patterns
bool matchGroup(const std::string& input_line, const std::string& pattern, int start, int end, bool negate) {
    std::string group_pattern = pattern.substr(start + 1, end - start - 2);
    if (negate) {
        for (char c : group_pattern) {
            if (input_line.find(c) != std::string::npos) return false;
        }
        return true;
    } else {
        for (char c : group_pattern) {
            if (input_line.find(c) != std::string::npos) return true;
        }
        return false;
    }
}

// Function to match a pattern against the input line
bool match(const std::string& input_line, const std::string& pattern) {
    bool hasDollar = (pattern.back() == '$');
    int patternSize = hasDollar ? pattern.size() - 1 : pattern.size();
    bool startAnchor = (pattern[0] == '^');

    for (int i = 0; i <= input_line.size() - patternSize; ++i) {
        int j = startAnchor ? 1 : 0; // Skip '^' if present
        int temp = i;
        bool matched = true;

        while (j < patternSize && temp < input_line.size()) {
            if (pattern[j] == '\\') {
                ++j;
                if (j < patternSize) {
                    if (pattern[j] == 'd') {
                        int count = 0;
                        while (j < patternSize && pattern[j] == 'd') {
                            ++count;
                            ++j;
                        }
                        int match_count = 0;
                        while (temp < input_line.size() && std::isdigit(input_line[temp])) {
                            ++match_count;
                            ++temp;
                        }
                        if (match_count != count) {
                            matched = false;
                            break;
                        }
                    } else if (pattern[j] == 'w') {
                        int count = 0;
                        while (j < patternSize && pattern[j] == 'w') {
                            ++count;
                            ++j;
                        }
                        int match_count = 0;
                        while (temp < input_line.size() && std::isalnum(input_line[temp])) {
                            ++match_count;
                            ++temp;
                        }
                        if (match_count != count) {
                            matched = false;
                            break;
                        }
                    } else if (pattern[j] == '[') {
                        int start_idx = j;
                        while (j < patternSize && pattern[j] != ']') ++j;
                        if (j < patternSize) {
                            bool negate = (pattern[start_idx + 1] == '^');
                            matched = matchGroup(input_line, pattern, start_idx, j + 1, negate);
                        }
                        if (!matched) break;
                    } else {
                        matched = false;
                        break;
                    }
                } else {
                    matched = false;
                    break;
                }
            } else {
                if (input_line[temp] != pattern[j]) {
                    matched = false;
                    break;
                } else {
                    ++temp;
                }
            }
            ++j;
        }

        if (matched && (hasDollar ? temp == input_line.size() : true)) return true;
        if (startAnchor && j != patternSize) return false;
    }

    return false;
}

// Function to match a pattern against the input line
bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if (pattern.length() == 1) {
        return input_line.find(pattern) != std::string::npos;
    } else if (pattern[0] == '\\' && pattern.length() > 1) {
        switch (pattern[1]) {
        case 'd':
            return matchdigit(input_line);
        case 'w':
            return matchalphanumeric(input_line);
        default:
            throw std::runtime_error("Unhandled pattern: " + pattern);
        }
    } else if (pattern[0] == '[' && pattern[pattern.length() - 1] == ']') {
        bool negate = (pattern[1] == '^');
        return matchGroup(input_line, pattern, 0, pattern.size(), negate);
    } else if (pattern.length() > 1) {
        return match(input_line, pattern);
    } else {
        throw std::runtime_error("Unhandled pattern: " + pattern);
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
