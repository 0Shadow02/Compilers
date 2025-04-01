#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include <cctype>

enum class TokenType {
    _return,
    int_lit,
    semi
};

struct Token {
    TokenType type;
    std::optional<std::string> value{};
};

std::vector<Token> tokenize(const std::string& str) {
    std::vector<Token> tokens;
    std::string buf;

    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];

        if (std::isalpha(c)) {
            buf.clear();
            while (i < str.length() && std::isalnum(str[i])) {
                buf.push_back(str[i++]);
            }
            i--; 

            if (buf == "return") {
                tokens.push_back({TokenType::_return});
            } else {
                std::cerr << "Do it right nerd! You messed up (skill issue) " << buf << std::endl;
                return {}; 
            }
        } else if (std::isdigit(c)) {
            buf.clear();
            do {
                buf.push_back(c);
                c = str[++i];
            } while (i < str.length() && std::isdigit(c));
            i--; 
            tokens.push_back({TokenType::int_lit, buf});
        } else if (c == ';') {
            tokens.push_back({TokenType::semi});
        } else if (!std::isspace(c)) {
            std::cerr << "Do it right nerd! You messed up (skill issue) " << c << std::endl;
            return {};
        }
    }

    return tokens; 
}

std::string tokens_to_asm(const std::vector<Token>& tokens) {
    std::stringstream output;
    output << "global _start\n"; 
    output << "_start:\n";

    for (size_t i = 0; i < tokens.size(); i++) {
        const Token& token = tokens[i];

        if (token.type == TokenType::_return) {
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::int_lit) {
                if (i + 2 < tokens.size() && tokens[i + 2].type == TokenType::semi) {
                    output << "    mov rax, 60\n"; // syscall number for exit
                    output << "    mov rdi, " << tokens[i + 1].value.value() << "\n"; // return value
                    output << "    syscall\n";   
                }
            }
        }
    }
    return output.str();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is ..." << std::endl;
        std::cerr << "aether <input.aeth>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream input(argv[1]);
    if (!input) {
        std::cerr << "Error: Could not open file '" << argv[1] << "'." << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    std::vector<Token> tokens = tokenize(contents);

    if (tokens.empty()) {
        return EXIT_FAILURE; // Tokenization failed
    }

    std::cout << tokens_to_asm(tokens) << std::endl;

    {
        std::fstream file("out.asm", std::ios::out);
        if (!file) {
            std::cerr << "Error: Could not open output file 'out.asm'." << std::endl;
            return EXIT_FAILURE;
        }
        file << tokens_to_asm(tokens);
    }

    system("yasm -f elf64 -o out.o out.asm && gcc -nostartfiles -o out out.o -lc");
    return EXIT_SUCCESS;
}
