
#include "Function.hpp"

#include <cstdio>

struct Function::Impl {
    
    struct Statement {
        virtual ~Statement() {}
        virtual void evaluate(Impl & context) const = 0;
    };

    struct Expression {
        virtual ~Expression() {}
        virtual Value evaluate(Impl & context) const = 0;
    };
    
    uint32_t count;
    Function * parent;
    std::map<std::string, Value> variables;
    std::vector<std::string> arguments;
    Statement * statement;
    
    Impl() : count(1), parent(nullptr), statement(nullptr) {}
    
    ~Impl() {
        // TODO find a way to avoid cyclic refs causing memory leak
        delete parent;
        delete statement;
    }
    
    Value get(std::string const & name) {
        auto it = variables.find(name);
        if (it != variables.end())
            return it->second;
        if (parent)
            return parent->impl->get(name);
        return Value();
    }
    
    void set(std::string const & name, Value const & value) {
        // TODO check if exist in parent
        variables[name] = value;
    }

    struct ExpressionStatement : Statement {
        Expression * value = nullptr;
        ~ExpressionStatement() {
            delete value;
        }
        void evaluate(Impl & context) const {
            value->evaluate(context);
        }
    };

    struct AssignStatement : Statement {
        std::string key;
        Expression * value = nullptr;
        ~AssignStatement() {
            delete value;
        }
        void evaluate(Impl & context) const {
            context.set(key, value->evaluate(context));
        }
    };

    struct ReturnStatement : Statement {
        Expression * result = nullptr;
        ~ReturnStatement() {
            delete result;
        }
        void evaluate(Impl & context) const {
            throw result->evaluate(context);
        }
    };

    struct BlockStatement : Statement {
        std::vector<Statement *> body;
        ~BlockStatement() {
            for (Statement * b : body)
                delete b;
        }
        void evaluate(Impl & context) const {
            for (Statement * b : body)
                b->evaluate(context);
        }
    };

    struct IfStatement : Statement {
        Expression * condition = nullptr;
        Statement * true_body = nullptr;
        Statement * false_body = nullptr;
        ~IfStatement() {
            delete condition;
            delete true_body;
            delete false_body;
        }
        void evaluate(Impl & context) const {
            if (condition->evaluate(context).getBool())
                true_body->evaluate(context);
            else if (false_body)
                false_body->evaluate(context);
        }
    };

    struct WhileStatement : Statement {
        Expression * condition = nullptr;
        Statement * body = nullptr;
        ~WhileStatement() {
            delete condition;
            delete body;
        }
        void evaluate(Impl & context) const {
            while (condition->evaluate(context).getBool())
                body->evaluate(context);
        }
    };

    struct ValueExpression : Expression {
        Value value;
        Value evaluate(Impl & context) const {
            return value;
        }
    };
    
    struct QueryExpression : Expression {
        std::string key;
        Value evaluate(Impl & context) const {
            return context.get(key);
        }
    };
    
    struct CallExpression : Expression {
        Expression * function = nullptr;
        Expression * argument = nullptr;
        ~CallExpression() {
            delete function;
            delete argument;
        }
        Value evaluate(Impl & context) const {
            Value f(function->evaluate(context));
            Value a(argument->evaluate(context));
            return f.evaluate(a);
        }
    };
    
    struct ArrayExpression : Expression {
        std::vector<Expression *> expressions;
        ~ArrayExpression() {
            for (Expression * expression : expressions)
                delete expression;
        }
        Value evaluate(Impl & context) const {
            std::vector<Value> array;
            for (Expression * expression : expressions)
                array.push_back(expression->evaluate(context));
            Value result;
            result.setVector(array);
            return result;
        }
    };
    
    struct BinaryExpression : Expression {
        Expression * left = nullptr;
        Expression * right = nullptr;
        ~BinaryExpression() {
            delete left;
            delete right;
        }
    };
    
    // TODO boolean operators
    
    // TODO comparison operators
    
    struct PlusExpression : BinaryExpression {
        Value evaluate(Impl & context) const {
            float l = left->evaluate(context).getFloat();
            float r = right->evaluate(context).getFloat();
            Value result;
            result.setFloat(l + r);
            return result;
        }
    };
    
    struct MinusExpression : BinaryExpression {
        Value evaluate(Impl & context) const {
            float l = left->evaluate(context).getFloat();
            float r = right->evaluate(context).getFloat();
            Value result;
            result.setFloat(l - r);
            return result;
        }
    };
    
    // TODO modulo
    
    struct TimesExpression : BinaryExpression {
        Value evaluate(Impl & context) const {
            float l = left->evaluate(context).getFloat();
            float r = right->evaluate(context).getFloat();
            Value result;
            result.setFloat(l * r);
            return result;
        }
    };
    
    struct DivideExpression : BinaryExpression {
        Value evaluate(Impl & context) const {
            float l = left->evaluate(context).getFloat();
            float r = right->evaluate(context).getFloat();
            Value result;
            result.setFloat(l / r);
            return result;
        }
    };
    
    struct Token {
        enum Type {
            END,
            BREAK,
            OPEN,
            CLOSE,
            SYMBOL,
            NUMBER,
            NAME,
            STRING
        };
        Type type;
        uint32_t row;
        uint32_t column;
        std::string text;
    };
    
    static bool tokenize(std::string const & code, std::vector<Token> & tokens) {
        char const * pointer = code.c_str();
        uint32_t row = 1;
        uint32_t column = 1;
        std::vector<uint32_t> indents;
        tokens.push_back({Token::OPEN, row, column});
        indents.push_back(0);
        while (pointer[0] != '\0') {
            
            // Skip whitespaces until relevant data
            uint32_t indent = 0;
            while (pointer[0] == ' ' || pointer[0] == '\t') {
                indent += pointer[0] == ' ' ? 1 : 4;
                column += pointer[0] == ' ' ? 1 : 4;
                ++pointer;
            }
            
            // Check if line is empty
            if (pointer[0] == '\0' || pointer[0] == '\n' || (pointer[0] == '\r' && pointer[1] == '\n') || pointer[0] == '#') {
                if (pointer[0] == '#')
                    while (pointer[0] != '\n')
                        ++pointer;
                if (pointer[0] == '\r')
                    ++pointer;
                if (pointer[0] == '\n') {
                    ++pointer;
                    column = 1;
                    ++row;
                }
                continue;
            }
            
            // Check if indentation has changed
            if (indent > indents.back()) {
                tokens.push_back({Token::OPEN, row, column});
                indents.push_back(indent);
            } else if (indent < indents.back()) {
                uint32_t up;
                while (indent < indents.back()) {
                    up = indents.back();
                    tokens.push_back({Token::CLOSE, row, column});
                    indents.pop_back();
                }
                if (indent != indents.back()) {
                    std::cout << row << ':' << column << ": incoherent indentation (expected " << indents.back() << " or >=" << up << ")" << std::endl;
                    return false;
                }
            }
            
            // Parse line content
            while (true) {
                
                // End of line
                if (pointer[0] == '\0' || pointer[0] == '\n' || (pointer[0] == '\r' && pointer[1] == '\n') || pointer[0] == '#') {
                    tokens.push_back({Token::BREAK, row, column});
                    if (pointer[0] == '#')
                        while (pointer[0] != '\n')
                            ++pointer;
                    if (pointer[0] == '\r')
                        ++pointer;
                    if (pointer[0] == '\n') {
                        ++pointer;
                        column = 1;
                        ++row;
                    }
                    break;
                }
                
                // Skip whitespaces
                if (pointer[0] == ' ' || pointer[0] == '\t') {
                    ++column;
                    ++pointer;
                    continue;
                }
                
                // Symbol
                if (strchr("+-*/%.,()<>=", pointer[0])) {
                    Token token = {Token::SYMBOL, row, column};
                    if ((pointer[0] == '<' && (pointer[1] == '>' || pointer[1] == '=')) || ((pointer[0] == '>' || pointer[0] == '=') && pointer[1] == '=')) {
                        token.text += pointer[0];
                        ++column;
                        ++pointer;
                    }
                    token.text += pointer[0];
                    ++column;
                    ++pointer;
                    tokens.push_back(token);
                    continue;
                }
                
                // Number
                if (pointer[0] >= '0' && pointer[0] <= '9') {
                    // TODO handle all number formats
                    Token token = {Token::NUMBER, row, column};
                    while (pointer[0] >= '0' && pointer[0] <= '9') {
                        token.text += pointer[0];
                        ++column;
                        ++pointer;
                    }
                    tokens.push_back(token);
                    continue;
                }
                // TODO handle single quote as char (i.e. a number)... unicode?
                
                // Name
                if ((pointer[0] >= 'a' && pointer[0] <= 'z') || (pointer[0] >= 'A' && pointer[0] <= 'Z') || pointer[0] == '_') {
                    Token token = {Token::NAME, row, column};
                    while ((pointer[0] >= 'a' && pointer[0] <= 'z') || (pointer[0] >= 'A' && pointer[0] <= 'Z') || pointer[0] == '_' || (pointer[0] >= '0' && pointer[0] <= '9')) {
                        token.text += pointer[0];
                        ++column;
                        ++pointer;
                    }
                    // TODO do not forget to give NaN, Inf, PI... special meaning
                    tokens.push_back(token);
                    continue;
                }
                
                // String
                if (pointer[0] == '"') {
                    Token token = {Token::STRING, row, column};
                    ++column;
                    ++pointer;
                    while (pointer[0] != '\0' && pointer[0] != '"') {
                        // TODO handle escape
                        // TODO handle line break
                        token.text += pointer[0];
                        ++column;
                        ++pointer;
                    }
                    if (pointer[0] == '"') {
                        ++column;
                        ++pointer;
                    }
                    tokens.push_back(token);
                    continue;
                }
                
                // Hence, this is an invalid character
                std::cout << row << ':' << column << ": bad character 0x" << (pointer[0] & 0xff);
                if (pointer[0] >= 32 && pointer[0] <= 127)
                    std::cout << ' ' << pointer[0];
                std::cout << std::endl;
                return false;
            }
        }
        
        // Close remaining groups
        while (!indents.empty()) {
            tokens.push_back({Token::CLOSE, row, column});
            indents.pop_back();
        }
        
        // Emit final token
        tokens.push_back({Token::END, row, column});
        return true;
    }
    
    static Expression * parseExpression(Token * & pointer, int priority = 0);
    
    static Expression * parseExpressionAndBreak(Token * & pointer) {
        Expression * expression = parseExpression(pointer);
        if (!expression)
            return nullptr;
        if (pointer[0].type != Token::BREAK) {
            delete expression;
            std::cout << pointer->row << ':' << pointer->column << ": expected line break after expression" << std::endl;
            return nullptr;
        }
        ++pointer;
        return expression;
    }
    
    static Statement * parseStatement(Token * & pointer);
    
    static Statement * parseBlock(Token * & pointer) {
        if (pointer[0].type != Token::OPEN) {
            std::cout << pointer->row << ':' << pointer->column << ": expected block open" << std::endl;
            return nullptr;
        }
        BlockStatement * block = new BlockStatement();
        ++pointer;
        while (pointer[0].type != Token::CLOSE) {
            Statement * sub = parseStatement(pointer);
            if (!sub) {
                delete block;
                return nullptr;
            }
            block->body.push_back(sub);
        }
        ++pointer;
        Statement * statement = block;
        if (block->body.size() == 1) {
            statement = block->body[0];
            block->body.clear();
            delete block;
        }
        return statement;
    }
    
    static Function * parse(Token * & pointer, std::vector<std::string> const & arguments) {
        Statement * statement = parseBlock(pointer);
        if (!statement)
            return nullptr;
        Function * function = new Function();
        function->impl->statement = statement;
        function->impl->arguments = arguments;
        return function;
    }
    
    static Function * parse(std::string const & code) {
        std::vector<Token> tokens;
        if (!tokenize(code, tokens))
            return nullptr;
        Token * pointer = &tokens[0];
        return parse(pointer, {});
    }
    
};

Function::Impl::Expression * Function::Impl::parseExpression(Token * & pointer, int priority) {
    
    // Check for preoperators
    // TODO preoperators (not, -)
    
    // Parse primary expression
    Expression * left;
    if (pointer[0].type == Token::NUMBER) {
        ValueExpression * expression = new ValueExpression();
        expression->value.setFloat(strtof(pointer[0].text.c_str(), nullptr));
        left = expression;
        ++pointer;
    } else if (pointer[0].type == Token::STRING) {
        ValueExpression * expression = new ValueExpression();
        expression->value.setString(pointer[0].text);
        left = expression;
        ++pointer;
    } else if (pointer[0].type == Token::NAME) {
        // TODO check if this is not a reserved word
        QueryExpression * expression = new QueryExpression();
        expression->key = pointer[0].text;
        left = expression;
        ++pointer;
    } else if (pointer[0].type == Token::SYMBOL && pointer[0].text == "(") {
        ++pointer;
        left = parseExpression(pointer);
        if (!left)
            return nullptr;
        if (pointer[0].type != Token::SYMBOL || pointer[0].text != "(") {
            std::cout << pointer->row << ':' << pointer->column << ": expected ')'" << std::endl;
            return nullptr;
        }
        ++pointer;
    } else {
        std::cout << pointer->row << ':' << pointer->column << ": unexpected token" << std::endl;
        return nullptr;
    }
    
    // Check for postoperators
    while (true) {
        if (pointer[0].type == Token::SYMBOL && pointer[0].text == "(") {
            ++pointer;
            ArrayExpression * arguments = new ArrayExpression();
            if (pointer[0].type != Token::SYMBOL || pointer[0].text != ")")
                while (true) {
                    Expression * argument = parseExpression(pointer);
                    if (!argument) {
                        delete left;
                        delete arguments;
                        return nullptr;
                    }
                    arguments->expressions.push_back(argument);
                    if (pointer[0].type == Token::SYMBOL && pointer[0].text == ")")
                        break;
                    if (pointer[0].type != Token::SYMBOL || pointer[0].text != ",") {
                        std::cout << pointer->row << ':' << pointer->column << ": unexpected ',' or ')'" << std::endl;
                        delete left;
                        delete arguments;
                        return nullptr;
                    }
                    ++pointer;
                }
            ++pointer;
            CallExpression * call = new CallExpression();
            call->function = left;
            call->argument = arguments;
            left = call;
            continue;
        }
        // TODO postoperators (dot)
        break;
    }
    
    // Check for binary operators
    while (true) {
        
        // Get operator
        BinaryExpression * binary = nullptr;
        int new_priority = 0;
        if (pointer[0].type == Token::SYMBOL && pointer[0].text == "+") {
            binary = new PlusExpression();
            new_priority = 5;
        } else if (pointer[0].type == Token::SYMBOL && pointer[0].text == "-") {
            binary = new MinusExpression();
            new_priority = 6;
        } else if (pointer[0].type == Token::SYMBOL && pointer[0].text == "*") {
            binary = new TimesExpression();
            new_priority = 8;
        } else if (pointer[0].type == Token::SYMBOL && pointer[0].text == "/") {
            binary = new DivideExpression();
            new_priority = 9;
        }
        
        // Check priority
        if (binary == nullptr || new_priority <= priority)
            return left;
        
        // Get right part
        ++pointer;
        binary->left = left;
        binary->right = parseExpression(pointer, new_priority);
        if (!binary->right) {
            delete binary;
            return nullptr;
        }
        left = binary;
    }
}

Function::Impl::Statement * Function::Impl::parseStatement(Token * & pointer) {
        
    // End
    if (pointer[0].type == Token::END) {
        std::cout << pointer->row << ':' << pointer->column << ": unexpected end of document" << std::endl;
        return nullptr;
    }

    // Block
    if (pointer[0].type == Token::OPEN)
        return parseBlock(pointer);

    // Return
    if (pointer[0].type == Token::NAME && pointer[0].text == "return") {
        ++pointer;
        Expression * expression = parseExpressionAndBreak(pointer);
        if (!expression)
            return nullptr;
        ReturnStatement * statement = new ReturnStatement();
        statement->result = expression;
        return statement;
    }

    // If
    if (pointer[0].type == Token::NAME && pointer[0].text == "if") {
        ++pointer;
        Expression * condition = parseExpressionAndBreak(pointer);
        if (!condition)
            return nullptr;
        IfStatement * statement = new IfStatement();
        statement->condition = condition;
        statement->true_body = parseBlock(pointer);
        if (!statement->true_body) {
            delete statement;
            return nullptr;
        }
        if (pointer[0].type == Token::NAME && pointer[0].text == "else") {
            statement->false_body = parseBlock(pointer);
            if (!statement->false_body) {
                delete statement;
                return nullptr;
            }
        }
        return statement;
    }

    // While
    if (pointer[0].type == Token::NAME && pointer[0].text == "while") {
        ++pointer;
        Expression * condition = parseExpressionAndBreak(pointer);
        if (!condition)
            return nullptr;
        WhileStatement * statement = new WhileStatement();
        statement->condition = condition;
        statement->body = parseBlock(pointer);
        if (!statement->body) {
            delete statement;
            return nullptr;
        }
        return statement;
    }

    // TODO other statements?

    // Function declaration (represented as an assignment)
    if (pointer[0].type == Token::NAME && pointer[0].text == "def") {
        ++pointer;
        
        // Get function name
        if (pointer[0].type != Token::NAME) {
            std::cout << pointer->row << ':' << pointer->column << ": expected identifier after 'def'" << std::endl;
            return nullptr;
        }
        std::string key = pointer[0].text;
        ++pointer;
        
        // Get function arguments
        std::vector<std::string> arguments;
        if (pointer[0].type == Token::SYMBOL && pointer[0].text == "(") {
            ++pointer;
            if (pointer[0].type != Token::SYMBOL || pointer[0].text != ")")
                while (true) {
                    if (pointer[0].type != Token::NAME) {
                        std::cout << pointer->row << ':' << pointer->column << ": expected identifier" << std::endl;
                        return nullptr;
                    }
                    arguments.push_back(pointer[0].text);
                    // TODO check if reserved word
                    ++pointer;
                    if (pointer[0].type == Token::SYMBOL && pointer[0].text == ")")
                        break;
                    if (pointer[0].type != Token::SYMBOL || pointer[0].text != ",") {
                        std::cout << pointer->row << ':' << pointer->column << ": expected ',' or ')' after identifier" << std::endl;
                        return nullptr;
                    }
                    ++pointer;
                }
            ++pointer;
        }
        if (pointer[0].type != Token::BREAK) {
            std::cout << pointer->row << ':' << pointer->column << ": expected line break after function header" << std::endl;
            return nullptr;
        }
        ++pointer;
        
        // Parse function body
        Function * function = parse(pointer, arguments);
        if (!function)
            return nullptr;
        
        // Define function as an assignment
        ValueExpression * expression = new ValueExpression();
        expression->value.setFunction(*function);
        AssignStatement * statement = new AssignStatement();
        statement->key = key;
        statement->value = expression;
        return statement;
    }

    // Assignment
    if (pointer[0].type == Token::NAME && pointer[1].type == Token::SYMBOL && pointer[1].text == "=") {
        AssignStatement * statement = new AssignStatement();
        statement->key = pointer[0].text;
        pointer += 2;
        statement->value = parseExpression(pointer);
        if (!statement->value) {
            delete statement;
            return nullptr;
        }
        if (pointer[0].type != Token::BREAK) {
            std::cout << pointer->row << ':' << pointer->column << ": expected line break after expression" << std::endl;
            delete statement;
            return nullptr;
        }
        ++pointer;
        return statement;
    }

    // Expression
    Expression * expression = parseExpressionAndBreak(pointer);
    if (!expression)
        return nullptr;
    ExpressionStatement * statement = new ExpressionStatement();
    statement->value = expression;
    return statement;
}

Function::Function() : impl(new Impl()) {}

Function::Function(Function const & function) : impl(function.impl) {
    ++impl->count;
}

Function & Function::operator=(Function const & function) {
    if (&function != this && function.impl != impl) {
        if (--impl->count == 0)
            delete impl;
        impl = function.impl;
        ++impl->count;
    }
    return *this;
}

Function::~Function() {
    if (--impl->count == 0)
        delete impl;
}

void Function::setValue(Value const & value) {
    Function function;
    Impl::ValueExpression * expression = new Impl::ValueExpression();
    expression->value = value;
    Impl::ExpressionStatement * statement = new Impl::ExpressionStatement();
    statement->value = expression;
    function.impl->statement = statement;
    *this = function;
}

bool Function::setCode(std::string const & code) {
    Function * function = Impl::parse(code);
    if (!function)
        return false;
    *this = *function;
    delete function;
    return true;
}

bool Function::setCodeFile(std::string const & path) {
    FILE * file = fopen(path.c_str(), "rb");
    if (!file)
        return false;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    std::string code(size, ' ');
    if (fread(&code[0], size, 1, file) != 1) {
        fclose(file);
        return false;
    }
    fclose(file);
    return setCode(code);
}

Value Function::evaluate(Value const & value) {
    if (impl->statement) {
        impl->variables["args"] = value;
        for (size_t i = 0; i < impl->arguments.size(); ++i)
            impl->variables[impl->arguments[i]] = value.get(i);
        try {
            impl->statement->evaluate(*impl);
        } catch (Value & result) {
            return result;
        }
    }
    return Value();
}
