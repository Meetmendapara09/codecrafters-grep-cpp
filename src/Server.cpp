#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>

// Function to check if a character is a digit
bool match_digit(char c) {
    return std::isdigit(c);
}

// Function to check if a character is alphanumeric
bool match_alphanumeric(char c) {
    return std::isalnum(c);
}

// Function to check if a character matches a set of characters
bool match_set(char c, const std::string& set, bool negate) {
    bool found = set.find(c) != std::string::npos;
    return negate ? !found : found;
}

// Function to match character classes in patterns
bool match_group(const std::string& regex, const std::string& text) {
    if (text.empty()) return false;

    bool negate = regex[0] == '^';
    size_t close_bracket_pos = regex.find(']');
    if (close_bracket_pos == std::string::npos) {
        throw std::runtime_error("Invalid regex: missing closing bracket");
    }

    size_t start = negate ? 1 : 0;
    if (match_set(text[0], regex.substr(start, close_bracket_pos - start), negate)) {
        return match_here(regex.substr(close_bracket_pos + 1), text.substr(1));
    }
    return false;
}

// Function to match one or more occurrences of a pattern
bool match_one_or_more(const std::string& regex, const std::string& text) {
    // Match the pattern one or more times
    for (size_t i = 1; i <= text.size(); ++i) {
        if (match_here(regex, text.substr(i))) {
            return true;
        }
    }
    return false;
}

// Recursive function to match regex against text
bool match_here(const std::string& regex, const std::string& text) {
    if (regex.empty()) return true;

    if (regex[0] == '^') {
        return match_here(regex.substr(1), text);
    }

    if (regex[regex.size() - 1] == '$') {
        return match_here(regex.substr(0, regex.size() - 1), text.substr(text.size() - regex.size() + 1));
    }

    // Check for quantifier '+'
    if (regex.size() > 1 && regex[1] == '+') {
        return match_one_or_more(regex.substr(0, 1), text);
    }

    for (size_t i = 0; i < text.size(); ++i) {
        if (regex[0] == text[i] && match_here(regex.substr(1), text.substr(i + 1))) {
            return true;
        }

        if (regex[0] == '\\') {
            if (regex.size() < 2) {
                throw std::runtime_error("Invalid regex: incomplete escape sequence");
            }
            if (regex[1] == 'd' && match_digit(text[i]) && match_here(regex.substr(2), text.substr(i + 1))) {
                return true;
            }
            if (regex[1] == 'w' && match_alphanumeric(text[i]) && match_here(regex.substr(2), text.substr(i + 1))) {
                return true;
            }
        }

        if (regex[0] == '[' && match_group(regex.substr(1), std::string(1, text[i]))) {
            return true;
        }
    }
    return false;
}

// Function to match a pattern against the input line
bool match_pattern(const std::string& input_line, const std::string& pattern) {
    return match_here(pattern, input_line);
}

// Main function to handle input and invoke pattern matching
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
