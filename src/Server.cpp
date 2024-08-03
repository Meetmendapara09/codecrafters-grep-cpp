#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>

// Function to check if a character matches a character class
bool match_char_class(char ch, const std::string& char_class) {
    if (char_class == "\\d") {
        return std::isdigit(ch);
    } else if (char_class == "\\w") {
        return std::isalnum(ch) || ch == '_';
    } else if (char_class.length() > 2 && char_class[0] == '[' && char_class.back() == ']') {
        std::string chars_to_match = char_class.substr(1, char_class.length() - 2);
        bool negation = !chars_to_match.empty() && chars_to_match[0] == '^';
        if (negation) {
            chars_to_match.erase(0, 1);
            return chars_to_match.find(ch) == std::string::npos;
        } else {
            return chars_to_match.find(ch) != std::string::npos;
        }
    }
    return false;
}

// Function to match the input line against the pattern
bool match_pattern(const std::string& input_line, const std::string& pattern) {
    size_t input_len = input_line.size();
    size_t pattern_len = pattern.size();
    size_t input_pos = 0;
    size_t pattern_pos = 0;

    while (pattern_pos < pattern_len) {
        if (pattern[pattern_pos] == '\\') {
            // Handle escaped characters
            if (pattern_pos + 1 < pattern_len) {
                std::string char_class;
                char next_char = pattern[pattern_pos + 1];
                
                if (next_char == 'd') {
                    char_class = "\\d";
                } else if (next_char == 'w') {
                    char_class = "\\w";
                } else {
                    // Unsupported escape sequence
                    return false;
                }

                if (input_pos >= input_len || !match_char_class(input_line[input_pos], char_class)) {
                    return false;
                }
                ++input_pos;
                pattern_pos += 2;
            } else {
                // Invalid pattern
                return false;
            }
        } else if (pattern[pattern_pos] == '[') {
            // Handle character groups
            size_t end_pos = pattern.find(']', pattern_pos);
            if (end_pos == std::string::npos) {
                // Error: unmatched '['
                return false;
            }
            std::string char_class = pattern.substr(pattern_pos, end_pos - pattern_pos + 1);
            if (input_pos >= input_len || !match_char_class(input_line[input_pos], char_class)) {
                return false;
            }
            ++input_pos;
            pattern_pos = end_pos + 1;
        } else {
            // Match literal character
            if (input_pos >= input_len || input_line[input_pos] != pattern[pattern_pos]) {
                return false;
            }
            ++input_pos;
            ++pattern_pos;
        }
    }

    // Ensure the entire input line was matched
    return input_pos == input_len;
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
