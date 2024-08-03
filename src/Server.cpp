#include <iostream>
#include <string>
#include <cctype>
#include <sstream>

// Function to match a single character against a class pattern
bool match_char_class(char ch, const std::string& char_class) {
    if (char_class == "\\d") {
        return std::isdigit(ch);
    } else if (char_class == "\\w") {
        return std::isalnum(ch) || ch == '_';
    } else if (char_class.length() > 2 && char_class[0] == '[' && char_class.back() == ']') {
        std::string chars_to_match = char_class.substr(1, char_class.length() - 2);
        if (!chars_to_match.empty() && chars_to_match[0] == '^') {
            chars_to_match.erase(0, 1);
            return chars_to_match.find(ch) == std::string::npos;
        } else {
            return chars_to_match.find(ch) != std::string::npos;
        }
    }
    return false;
}

// Function to match the input line against a pattern
bool match_pattern(const std::string& input_line, const std::string& pattern) {
    std::istringstream pattern_stream(pattern);
    std::string component;
    size_t pos = 0;
    
    while (pattern_stream >> component) {
        if (component == "\\d" || component == "\\w" ||
            (component.length() > 2 && component[0] == '[' && component.back() == ']')) {
            // Match character class
            if (pos >= input_line.size() || !match_char_class(input_line[pos], component)) {
                return false;
            }
            ++pos;
        } else {
            // Match literal string
            size_t match_length = component.length();
            if (input_line.substr(pos, match_length) != component) {
                return false;
            }
            pos += match_length;
        }
    }
    
    // Ensure the whole input line was matched
    return pos == input_line.size();
}

int main(int argc, char* argv[]) {
    // Flush after every std::cout / std::cerr
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
