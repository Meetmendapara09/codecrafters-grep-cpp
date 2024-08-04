#include <iostream>
#include <string>
#include <regex>
#include <cctype>
#include <stdexcept>

using namespace std;

// Helper function to check for digits in the input line using regex
bool matchdigit(const string& input_line) {
    return regex_search(input_line, regex("\\d"));
}

// Helper function to check for alphanumeric characters in the input line using regex
bool matchalphanumeric(const string& input_line) {
    return regex_search(input_line, regex("\\w"));
}

// Helper function to match negative character groups (e.g., [^abc])
bool negativeMatchGroup(const string& input_line, const string& pattern, int start, int end) {
    // Extract characters from the pattern
    string chars = pattern.substr(start + 1, end - start - 2);
    // Check if any of these characters are present in the input line
    for (char ch : chars) {
        if (input_line.find(ch) != string::npos) {
            return false; // Found a character that should be excluded
        }   
    }
    return true; // None of the characters were found in the input line
}

// Helper function to match positive character groups (e.g., [abc])
bool positiveMatchGroup(const string& input_line, const string& pattern, int start, int end) {
    // Extract characters from the pattern
    string chars = pattern.substr(start + 1, end - start - 2);
    // Check if any of these characters are present in the input line
    for (char ch : chars) {
        if (input_line.find(ch) != string::npos) {
            return true; // Found a character that is included
        }
    }
    return false; // None of the characters were found in the input line
}

// Match function to handle various patterns including anchors, quantifiers, and escape sequences
bool match(const string& input_line, const string& pattern) {
    int i = 0;
    bool startAnchor = (pattern[0] == '^'); // Check if pattern starts with '^' for start of string anchor
    bool endAnchor = (pattern[pattern.size() - 1] == '$'); // Check if pattern ends with '$' for end of string anchor
    string adjustedPattern = pattern;
    
    // Remove anchors from pattern for easier processing
    if (startAnchor) adjustedPattern = pattern.substr(1);
    if (endAnchor) adjustedPattern = pattern.substr(0, pattern.size() - 1);

    // Iterate through the input line
    while (i < input_line.size()) {
        int j = 0;
        int temp = i;

        // Process the pattern against the current segment of the input line
        while (j < adjustedPattern.size() && temp < input_line.size()) {
            if (adjustedPattern[j] == '\\') {
                // Handle escape sequences
                j++;
                if (j < adjustedPattern.size()) {
                    if (adjustedPattern[j] == 'd') {
                        if (!isdigit(input_line[temp])) break;
                        temp++;
                    } else if (adjustedPattern[j] == 'w') {
                        if (!isalnum(input_line[temp])) break;
                        temp++;
                    } else if (adjustedPattern[j] == '[') {
                        // Handle character groups
                        int start = j;
                        while (j < adjustedPattern.size() && adjustedPattern[j] != ']') j++;
                        if (j < adjustedPattern.size() && adjustedPattern[j] == ']') {
                            // Determine if it's a negative or positive character group
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
                // Handle one or more occurrences
                j++;
                while (j < adjustedPattern.size() && temp < input_line.size() && adjustedPattern[j] != input_line[temp]) {
                    temp++;
                }
            } else if (adjustedPattern[j] == '?') {
                // Handle zero or one occurrence
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
                // Handle literal characters
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

// Function to match patterns with special cases and regex support
bool match_pattern(const string& input_line, const string& pattern) {
    if (pattern.length() == 1) {
        // Handle single character pattern
        return input_line.find(pattern) != string::npos;
    } else if (pattern == "\\d") {
        // Handle digit pattern
        return matchdigit(input_line);
    } else if (pattern == "\\w") {
        // Handle word character pattern
        return matchalphanumeric(input_line);
    } else if (pattern[0] == '[' && pattern[pattern.length() - 1] == ']') {
        // Handle character class patterns
        if (pattern[1] == '^') {
            return negativeMatchGroup(input_line, pattern, 0, pattern.size());
        } else {
            return positiveMatchGroup(input_line, pattern, 0, pattern.size());
        }
    } else {
        // Handle complex patterns using regex
        try {
            regex regex_pattern(pattern);
            return regex_search(input_line, regex_pattern);
        } catch (const regex_error&) {
            return false; // Invalid regex pattern
        }
    }
}

// Main function to handle command-line arguments and pattern matching
int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Expected two arguments" << endl;
        return 1;
    }
    
    string flag = argv[1];
    string pattern = argv[2];
    
    // Check if the first argument is '-E'
    if (flag != "-E") {
        cerr << "Expected first argument to be '-E'" << endl;
        return 1;
    }

    // Read input line from standard input
    string input_line;
    getline(cin, input_line);

    // Match the pattern against the input line and return appropriate exit code
    if (match_pattern(input_line, pattern)) {
        return 0;
    } else {
        return 1;
    }
}
