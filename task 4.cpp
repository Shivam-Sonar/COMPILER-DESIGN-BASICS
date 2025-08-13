#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>

enum class TokenType {
    Number,
    Plus,
    Minus,
    Multiply,
    Divide,
    LParen,
    RParen,
    End
};

struct Token {
    TokenType type;
    double value;  // Only used for numbers

    Token(TokenType t, double v = 0) : type(t), value(v) {}
};

class Lexer {
public:
    Lexer(const std::string& input) : input(input), pos(0) {}

    Token getNextToken() {
        while (pos < input.length() && std::isspace(input[pos])) {
            ++pos;
        }

        if (pos >= input.length()) return Token(TokenType::End);

        char current = input[pos];

        if (std::isdigit(current)) {
            double num = 0;
            while (pos < input.length() && std::isdigit(input[pos])) {
                num = num * 10 + (input[pos++] - '0');
            }
            return Token(TokenType::Number, num);
        }

        ++pos;
        switch (current) {
            case '+': return Token(TokenType::Plus);
            case '-': return Token(TokenType::Minus);
            case '*': return Token(TokenType::Multiply);
            case '/': return Token(TokenType::Divide);
            case '(': return Token(TokenType::LParen);
            case ')': return Token(TokenType::RParen);
            default:
                throw std::runtime_error(std::string("Invalid character: ") + current);
        }
    }

private:
    std::string input;
    size_t pos;
};

class Parser {
public:
    Parser(Lexer& lexer) : lexer(lexer), currentToken(lexer.getNextToken()) {}

    double parseExpression() {
        return parseTerm();  // Starting point
    }

private:
    Lexer& lexer;
    Token currentToken;

    void eat(TokenType type) {
        if (currentToken.type == type)
            currentToken = lexer.getNextToken();
        else
            throw std::runtime_error("Unexpected token");
    }

    double parseFactor() {
        if (currentToken.type == TokenType::Number) {
            double value = currentToken.value;
            eat(TokenType::Number);
            return value;
        } else if (currentToken.type == TokenType::LParen) {
            eat(TokenType::LParen);
            double result = parseTerm();
            eat(TokenType::RParen);
            return result;
        } else if (currentToken.type == TokenType::Minus) {
            eat(TokenType::Minus);
            return -parseFactor();
        } else {
            throw std::runtime_error("Invalid factor");
        }
    }

    double parseTerm() {
        double result = parseAddSub();
        return result;
    }

    double parseAddSub() {
        double result = parseMulDiv();

        while (currentToken.type == TokenType::Plus || currentToken.type == TokenType::Minus) {
            if (currentToken.type == TokenType::Plus) {
                eat(TokenType::Plus);
                result += parseMulDiv();
            } else if (currentToken.type == TokenType::Minus) {
                eat(TokenType::Minus);
                result -= parseMulDiv();
            }
        }
        return result;
    }

    double parseMulDiv() {
        double result = parseFactor();

        while (currentToken.type == TokenType::Multiply || currentToken.type == TokenType::Divide) {
            if (currentToken.type == TokenType::Multiply) {
                eat(TokenType::Multiply);
                result *= parseFactor();
            } else if (currentToken.type == TokenType::Divide) {
                eat(TokenType::Divide);
                double divisor = parseFactor();
                if (divisor == 0)
                    throw std::runtime_error("Division by zero!");
                result /= divisor;
            }
        }

        return result;
    }
};

int main() {
    std::string input;
    std::cout << "Simple Arithmetic Expression Evaluator\n";
    std::cout << "Enter an expression (e.g., 2 + 3 * (4 - 1)) or 'exit' to quit:\n";

    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, input);

        if (input == "exit") break;

        try {
            Lexer lexer(input);
            Parser parser(lexer);
            double result = parser.parseExpression();
            std::cout << "Result: " << result << std::endl;
        } catch (const std::exception& ex) {
            std::cerr << "Error: " << ex.what() << std::endl;
        }
    }

    return 0;
}
