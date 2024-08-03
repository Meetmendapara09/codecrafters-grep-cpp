#include <iostream>
#include <string>
#include <stack>

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

bool match(const std::string& input_line, const std::string& pattern) {
    int i = 0;
    bool hasDollar = (pattern.back() == '$');
    int patternSize = hasDollar ? pattern.size() - 1 : pattern.size();

    while (i <= input_line.size() - patternSize) {
        int j = 0;
        bool start = false;
        if (pattern[0] == '^') {
            j++;
            start = true;
        }
        int temp = i;
        while (j < patternSize && temp < input_line.size()) {
            if (pattern[j] == '\\') {
                j++;
                if (j < patternSize) {
                    if (pattern[j] == 'd') {
                        int digit_count = 0;
                        while (j < patternSize && pattern[j] == 'd') {
                            digit_count++;
                            j++;
                        }
                        if (digit_count > 0) {
                            int match_count = 0;
                            while (temp < input_line.size() && std::isdigit(input_line[temp])) {
                                match_count++;
                                temp++;
                            }
                            if (match_count != digit_count) break;
                        }
                    } else if (pattern[j] == 'w') {
                        int word_count = 0;
                        while (j < patternSize && pattern[j] == 'w') {
                            word_count++;
                            j++;
                        }
                        if (word_count > 0) {
                            int match_count = 0;
                            while (temp < input_line.size() && std::isalnum(input_line[temp])) {
                                match_count++;
                                temp++;
                            }
                            if (match_count != word_count) break;
                        }
                    } else if (pattern[j] == '[') {
                        int start = j;
                        while (j < patternSize && pattern[j] != ']') j++;
                        if (pattern[j] == '^') {
                            return negativeMatchGroup(input_line, pattern, start, j + 1);
                        } else {
                            return positiveMatchGroup(input_line, pattern, start, j + 1);
                        }
                    }
                } else {
                    break;
                }
            } else {
                if (input_line[temp] != pattern[j]) break;
                else temp++;
            }
            j++;
        }
        if (j == patternSize && (!hasDollar || temp == input_line.size())) return true;
        if (start && j != patternSize) return false;
        i++;
    }
    return false;
}

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if (pattern.length() == 1) {
        return input_line.find(pattern) != std::string::npos;
    }
    else if (pattern[0] == '\\' && pattern.length() > 1) {
        switch (pattern[1]) {
        case 'd':
            return matchdigit(input_line);
        case 'w':
            return matchalphanumeric(input_line);
        default:
            throw std::runtime_error("Unhandled pattern " + pattern);
        }
    }
    else if (pattern[0] == '[' && pattern[pattern.length() - 1] == ']') {
        if (pattern[1] == '^') {
            return negativeMatchGroup(input_line, pattern, 0, pattern.size());
        } else {
            return positiveMatchGroup(input_line, pattern, 0, pattern.size());
        }
    }
    else if (pattern.length() > 1) {
        return match(input_line, pattern);
    }
    else {
        throw std::runtime_error("Unhandled pattern " + pattern);
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
