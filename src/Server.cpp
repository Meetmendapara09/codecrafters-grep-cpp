#include <iostream>
#include <string>
#include <cctype>  // For std::isalnum

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if (pattern == "\\w") {
        // Check for any alphanumeric character or underscore
        for (char ch : input_line) {
            if (std::isalnum(ch) || ch == '_') {
                return true;
            }
        }
        return false;
    } else if (pattern == "\\d") {
        // Handle digit pattern
        return input_line.find_first_of("0123456789") != std::string::npos;
    } else if (pattern.length() > 2 && pattern[0] == '[' && pattern.back() == ']') {
        // Handle positive and negative character groups
        std::string chars_to_match = pattern.substr(1, pattern.length() - 2); // Remove the square brackets
        
        if (!chars_to_match.empty() && chars_to_match[0] == '^') {
            // Negative character group
            chars_to_match.erase(0, 1); // Remove the '^' from the beginning
            for (char ch : input_line) {
                if (chars_to_match.find(ch) == std::string::npos) {
                    return true; // Found a character not in the negative group
                }
            }
            return false;
        } else {
            // Positive character group
            for (char ch : input_line) {
                if (chars_to_match.find(ch) != std::string::npos) {
                    return true;
                }
            }
            return false;
        }
    } else if (pattern.length() == 1) {
        // Handle single-character patterns
        return input_line.find(pattern) != std::string::npos;
    } else {
        throw std::runtime_error("Unhandled pattern " + pattern);
    }
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
