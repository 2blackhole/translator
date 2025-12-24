#ifndef TRANSLATOR
#define TRANSLATOR


#include <iostream>
#include <string>
#include <cmath>
#include <stdexcept>
#include "TStack.h"
#include "VectorBananov.h"

enum class TYPE {
    OPERATOR,
    NUMBER,
    OPEN_BRACKET,
    CLOSE_BRACKET,
};

class Token {
public:
    virtual TYPE get_type() const = 0;
    virtual ~Token() = default;
};

class Number : public Token {
    double value = 0;
public:
    explicit Number(double val) : value(val) {}
    TYPE get_type() const override { return TYPE::NUMBER; }
    double get_value() const { return value; }
};

class Operator : public Token {
    char op = '+';
    int prior = 0;
public:
    explicit Operator(char c, int p) : op(c), prior(p) {}
    TYPE get_type() const override { return TYPE::OPERATOR; }
    char get_op() const { return op; }
    int get_priority() const { return prior; }
};

class OpenBracket : public Token {
public:
    TYPE get_type() const override { return TYPE::OPEN_BRACKET; }
};

class CloseBracket : public Token {
public:
    TYPE get_type() const override { return TYPE::CLOSE_BRACKET; }
};

enum class num_state { START, EXP_N_OR_D, D_EXP_N, Z_EXP_N, EXP_N, EXP_D, END, ERROR };

size_t binpow(size_t a, size_t b) {
    if (b == 0) return 1;
    if (b == 1) return a;
    if (b % 2 == 1) return a * binpow(a, b - 1);
    size_t v = binpow(a, b / 2);
    return v * v;
}

Number* parse_number(const std::string& s, size_t& pos) {
    std::string val;
    num_state st = num_state::START;
    while (pos < s.size() && st != num_state::ERROR && st != num_state::END) {
        char c = s[pos];
        switch (st) {
            case num_state::START:
                if (c == '0') { st = num_state::EXP_D; val.push_back(c); }
                else if (c >= '1' && c <= '9') { st = num_state::EXP_N_OR_D; val.push_back(c); }
                else st = num_state::ERROR;
                break;
            case num_state::EXP_D:
                if (c == '.') { st = num_state::D_EXP_N; val.push_back(c); }
                else st = num_state::END;
                break;
            case num_state::EXP_N_OR_D:
                if (isdigit(c)) val.push_back(c);
                else if (c == '.') { st = num_state::D_EXP_N; val.push_back(c); }
                else st = num_state::END;
                break;
            case num_state::D_EXP_N:
                if (c == '0') { st = num_state::Z_EXP_N; val.push_back(c); }
                else if (c >= '1' && c <= '9') { st = num_state::EXP_N; val.push_back(c); }
                else st = num_state::ERROR;
                break;
            case num_state::Z_EXP_N:
            case num_state::EXP_N:
                if (isdigit(c)) {
                    st = (c == '0' ? num_state::Z_EXP_N : num_state::EXP_N);
                    val.push_back(c);
                } else st = num_state::END;
                break;
            default: st = num_state::ERROR; break;
        }
        if (st != num_state::END && st != num_state::ERROR) ++pos;
    }

    if (val.empty() || st == num_state::ERROR) throw std::invalid_argument("Invalid number format");

    double number_value = 0;
    size_t dot_index = val.find('.');
    if (dot_index == std::string::npos) {
        for (size_t i = 0; i < val.size(); i++)
            number_value += static_cast<double>((val[i] - '0') * binpow(10, val.size() - i - 1));
    } else {
        for (size_t i = 0; i < val.size(); i++) {
            if (i == dot_index) continue;
            if (i < dot_index)
                number_value += static_cast<double>((val[i] - '0') * binpow(10, dot_index - i - 1));
            else
                number_value += static_cast<double>(val[i] - '0') / static_cast<double>(binpow(10, i - dot_index));
        }
    }
    return new Number(number_value);
}

std::string extract_bracket_content(const std::string& s, size_t& pos) {
    if (s[pos] != '(') throw std::invalid_argument("Expected '(' after function");
    pos++;
    int balance = 1;
    std::string res;
    while (pos < s.size() && balance > 0) {
        if (s[pos] == '(') balance++;
        if (s[pos] == ')') balance--;
        if (balance > 0) res.push_back(s[pos++]);
    }
    if (balance != 0) throw std::invalid_argument("Unbalanced brackets in function");
    pos++;
    return res;
}

double solve(const VectorBananov<Token*>& tokens);
VectorBananov<Token*> parse_into_tokens(std::string s);

enum class state { EXP_N_OR_O, EXP_OP_OR_C, ERROR };

VectorBananov<Token*> parse_into_tokens(std::string s) {
    if (s.find_first_not_of(" \t\n\r") == std::string::npos)
        throw std::invalid_argument("Empty expression");

    size_t pos = 0;
    VectorBananov<Token*> tokens;
    state st = state::EXP_N_OR_O;

    while (pos < s.size()) {
        char c = s[pos];
        if (isspace(c)) { pos++; continue; }

        if (st == state::EXP_N_OR_O) {
            if (isdigit(c)) {
                tokens.push_back(parse_number(s, pos));
                st = state::EXP_OP_OR_C;
            }
            else if (c == '(') {
                tokens.push_back(new OpenBracket());
                pos++;
            }
            else if (c == '-' || c == '+') {
                if (!tokens.empty() && tokens[tokens.size() - 1]->get_type() == TYPE::OPERATOR) {
                    throw std::invalid_argument("Double operator");
                }

                tokens.push_back(new Number(0));
                tokens.push_back(new Operator(c, 0));
                pos++;
            }
            else if (s.substr(pos, 3) == "sin" || s.substr(pos, 3) == "cos") {
                std::string func = s.substr(pos, 3);
                pos += 3;
                std::string content = extract_bracket_content(s, pos);
                VectorBananov<Token*> inner_tokens = parse_into_tokens(content);
                tokens.push_back(new Number(func == "sin" ? std::sin(solve(inner_tokens)) : std::cos(solve(inner_tokens))));
                st = state::EXP_OP_OR_C;
            } else throw std::invalid_argument("Syntax error");
        }
        else if (st == state::EXP_OP_OR_C) {
            if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
                int p = (c == '+' || c == '-') ? 0 : (c == '^' ? 2 : 1);
                tokens.push_back(new Operator(c, p));
                st = state::EXP_N_OR_O;
                pos++;
            } else if (c == ')') {
                tokens.push_back(new CloseBracket());
                pos++;
            } else throw std::invalid_argument("Expected operator");
        }
    }

    if (!tokens.empty() && tokens[tokens.size() - 1]->get_type() == TYPE::OPERATOR)
        throw std::invalid_argument("Trailing operator");

    return tokens;
}

void do_op(TStack<double>& num_stack, Token* oper) {
    Operator* op = dynamic_cast<Operator*>(oper);
    if (num_stack.size() < 2) return;
    auto r = num_stack.top(); num_stack.pop();
    auto l = num_stack.top(); num_stack.pop();
    switch (op->get_op()) {
        case '+': num_stack.push(l + r); break;
        case '-': num_stack.push(l - r); break;
        case '*': num_stack.push(l * r); break;
        case '/': 
            if (r == 0) throw std::runtime_error("Division by zero");
            num_stack.push(l / r); break;
        case '^': num_stack.push(std::pow(l, r)); break;
        default: ;
    }
}

double solve(const VectorBananov<Token*>& tokens) {
    if (tokens.empty()) throw std::invalid_argument("Empty tokens");

    TStack<double> num_stack;
    TStack<Token*> op_stack;

    for (size_t i = 0; i < tokens.size(); i++) {
        TYPE type = tokens[i]->get_type();
        if (type == TYPE::NUMBER) {
            num_stack.push(static_cast<Number*>(tokens[i])->get_value());
        }
        else if (type == TYPE::OPEN_BRACKET) {
            op_stack.push(tokens[i]);
        }
        else if (type == TYPE::CLOSE_BRACKET) {
            bool found_open = false;
            while (!op_stack.empty()) {
                if (op_stack.top()->get_type() == TYPE::OPEN_BRACKET) {
                    found_open = true;
                    break;
                }
                do_op(num_stack, op_stack.top());
                op_stack.pop();
            }
            if (!found_open) throw std::invalid_argument("Unbalanced brackets: missing '('");
            op_stack.pop();
        }
        else if (type == TYPE::OPERATOR) {
            Operator* cur_op = static_cast<Operator*>(tokens[i]);
            while (!op_stack.empty() && op_stack.top()->get_type() == TYPE::OPERATOR) {
                Operator* last_op = static_cast<Operator*>(op_stack.top());
                if (last_op->get_priority() >= cur_op->get_priority()) { // СКОРЕЕ ВСЕГО УПАЛО ТУТ!!!
                    do_op(num_stack, op_stack.top());
                    op_stack.pop();
                } else break;
            }
            op_stack.push(tokens[i]);
        }
    }

    while (!op_stack.empty()) {
        if (op_stack.top()->get_type() == TYPE::OPEN_BRACKET)
            throw std::invalid_argument("Unbalanced brackets: missing ')'");
        do_op(num_stack, op_stack.top());
        op_stack.pop();
    }

    if (num_stack.empty()) throw std::runtime_error("Stack is empty after evaluation");
    double result = num_stack.top();

    for (size_t i = 0; i < tokens.size(); i++) delete tokens[i];

    return result;
}

// int main() {
//     try {
//         std::string s1 = "sin(0) + 2^3 * -2";
//         auto v1 = parse_into_tokens(s1);
//         std::cout << s1 << " = " << solve(v1) << std::endl;
//
//         std::string s2 = "cos(sin(0) * 10^2)";
//         std::cout << s2 << " = " << solve(parse_into_tokens(s2)) << std::endl;
//
//         std::string s3 = "1.005 + 2.0";
//         std::cout << s3 << " = " << solve(parse_into_tokens(s3)) << std::endl;
//
//     } catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }
//     return 0;
// }

#endif
