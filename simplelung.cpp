#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
  
using namespace std;
// Token Types
enum TokenType {
    IDENTIFIER,
    NUMBER,
    ASSIGN,
    PLUS,
    MINUS,
    MULT,
    DIV,
    SEMICOLON,
    END_OF_FILE,
    UNKNOWN
};

// Token Structure
struct Token {
    TokenType type;
    string value;
};

// Lexer: Tokenizes input
class Lexer {
public:
    Lexer(const string &input) : input(input), position(0) {}

    Token getNextToken() {
        skipWhitespace();
        if (position >= input.size()) return {END_OF_FILE, ""};

        char current = input[position];
        if (isalpha(current)) return tokenizeIdentifier();
        if (isdigit(current)) return tokenizeNumber();

        position++; // Move forward for single-character tokens
        switch (current) {
            case '=': return {ASSIGN, "="};
            case '+': return {PLUS, "+"};
            case '-': return {MINUS, "-"};
            case '*': return {MULT, "*"};
            case '/': return {DIV, "/"};
            case ';': return {SEMICOLON, ";"};
            default:  return {UNKNOWN, string(1, current)};
        }
    }

private:
    string input;
    size_t position;

    void skipWhitespace() {
        while (position < input.size() && isspace(input[position])) position++;
    }

    Token tokenizeIdentifier() {
        string result;
        while (position < input.size() && isalnum(input[position]))
            result += input[position++];
        return {IDENTIFIER, result};
    }

    Token tokenizeNumber() {
        string result;
        while (position < input.size() && isdigit(input[position]))
            result += input[position++];
        return {NUMBER, result};
    }
};

// AST Node
struct ASTNode {
    string value;
    vector<ASTNode *> children;

    ASTNode(const string &val) : value(val) {}
};

// Parser: Builds an AST
class Parser {
public:
    Parser(Lexer &lexer) : lexer(lexer) { currentToken = lexer.getNextToken(); }

    ASTNode *parse() { return parseStatement(); }

private:
    Lexer &lexer;
    Token currentToken;

    void advance() { currentToken = lexer.getNextToken(); }

    ASTNode *parseStatement() {
        ASTNode *node = parseExpression();
        if (currentToken.type != SEMICOLON)
            throw runtime_error("Expected ';' at the end of the statement");
        advance();
        return node;
    }

    ASTNode *parseExpression() {
        ASTNode *node = parseTerm();
        while (currentToken.type == PLUS || currentToken.type == MINUS) {
            string op = currentToken.value;
            advance();
            ASTNode *opNode = new ASTNode(op);
            opNode->children.push_back(node);
            opNode->children.push_back(parseTerm());
            node = opNode;
        }
        return node;
    }

    ASTNode *parseTerm() {
        ASTNode *node = parseFactor();
        while (currentToken.type == MULT || currentToken.type == DIV) {
            string op = currentToken.value;
            advance();
            ASTNode *opNode = new ASTNode(op);
            opNode->children.push_back(node);
            opNode->children.push_back(parseFactor());
            node = opNode;
        }
        return node;
    }

    ASTNode *parseFactor() {
        if (currentToken.type == NUMBER || currentToken.type == IDENTIFIER) {
            ASTNode *node = new ASTNode(currentToken.value);
            advance();
            return node;
        } else {
            throw runtime_error("Unexpected token: " + currentToken.value);
        }
    }
};

// Code Generator
class CodeGenerator {
public:
    void generate(ASTNode *node) {
        if (node->children.empty()) {
            emit("LOAD " + node->value);
        } else {
            generate(node->children[0]);
            generate(node->children[1]);
            if (node->value == "+") emit("ADD");
            else if (node->value == "-") emit("SUB");
            else if (node->value == "*") emit("MUL");
            else if (node->value == "/") emit("DIV");
            else throw runtime_error("Unknown operator: " + node->value);
        }
    }

private:
    void emit(const string &instruction) { cout << instruction << endl; }
};

// Main Program
int main() {
    string input = " 3 + 4 * 5;";
    Lexer lexer(input);
    Parser parser(lexer);
    CodeGenerator generator;

    try {
        ASTNode *ast = parser.parse();
        generator.generate(ast);
    } catch (const exception &e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
