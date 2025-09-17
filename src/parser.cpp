#include "parser.h"
#include <algorithm>

parser::parser():src(""){}

parser::parser(const std::string &code):src(code){}

void parser::appendProgram(astNode *node)
{
    node->type=astNodeType::PROGRAM;

    auto tk=src.peek();
    while (tk.type != tokenType::ILLEGAL)
    {
        if (tk.type != tokenType::KEYWORD)
            throw compileError();
        if (tk.value == "fn")
        {
            auto* newNode = new astNode;
            newNode->type = astNodeType::FUNCTION;
            node->children.push_back(newNode);
            appendFunction(newNode);
        }
        else if (tk.value == "const")
        {
            std::cerr<<"Oh, how can this be possible?"<<std::endl;
        }
        tk=src.peek();
    }
    if (!src.checkEnd())
        throw compileError();
}

void parser::appendFunction(astNode *node)
{
    node->type=astNodeType::FUNCTION;

    // identifier
    auto tk=src.consume();
    if (tk.type != tokenType::KEYWORD || tk.value != "fn")
        throw compileError();

    tk=src.consume();
    if (tk.type != tokenType::IDENTIFIER)
        throw compileError();
    node->value = tk.value;

    //parameters
    tk=src.consume();
    if (tk.type != tokenType::OPERATOR || tk.value != "(")
        throw compileError();
    auto* newNode=new astNode;
    appendParameters(newNode);
    node->children.push_back(newNode);

    tk=src.consume();
    if (tk.type != tokenType::OPERATOR || tk.value != ")")
        throw compileError();

    // -> type
    tk=src.peek();
    newNode=new astNode;
    if (tk.type == tokenType::OPERATOR && tk.value == "->")
        src.consume(),appendType(newNode);
    else
    {
        newNode->type=astNodeType::TYPE;
        newNode->value = "()";
    }
    node->children.push_back(newNode);

    // statement block
    tk=src.consume();
    if (tk.type != tokenType::OPERATOR || tk.value != "{")
        throw compileError();


    newNode=new astNode;
    appendStatementBlock(newNode);
    node->children.push_back(newNode);

    tk=src.consume();
    if (tk.type != tokenType::OPERATOR || tk.value != "}")
        throw compileError();
}

void parser::appendType(astNode *node)
{
    node->type=astNodeType::TYPE;

    auto tk=src.consume();
    if (tk.type == tokenType::OPERATOR && tk.value == "(")
    {
        tk=src.consume();
        if (tk.type == tokenType::OPERATOR && tk.value == ")")
            node->value="()";
        else
            throw compileError();
    }
    else if (tk.type == tokenType::IDENTIFIER && tk.value == "i32")
        node->value="i32";
    else if (tk.type == tokenType::IDENTIFIER && tk.value == "u32")
        node->value="u32";
    else if (tk.type == tokenType::IDENTIFIER && tk.value == "isize")
        node->value="isize";
    else if (tk.type == tokenType::IDENTIFIER && tk.value == "usize")
        node->value="usize";
    else
        throw compileError();
}

void parser::appendParameters(astNode *node)
{
    node->type=astNodeType::PARAMETERS;

    auto tk=src.peek();
    if (tk.type == tokenType::KEYWORD && tk.value == "self")
    {
        std::cerr<<"Oh, how can this be possible?"<<std::endl;
        src.consume();
        tk=src.peek();
        if (tk.type == tokenType::KEYWORD && tk.value == ",")
            src.consume(),tk=src.peek();
    }

    while (tk.type == tokenType::IDENTIFIER)
    {
        auto newNode=new astNode;
        appendTypedIdentifier(newNode);
        node->children.push_back(newNode);

        tk=src.peek();
        if (tk.type == tokenType::OPERATOR && tk.value == ",")
            src.consume(),tk=src.peek();
    }
}

void parser::appendTypedIdentifier(astNode *node)
{
    node->type=astNodeType::TYPED_IDENTIFIER;

    auto tk=src.consume();
    if (tk.type != tokenType::IDENTIFIER)
        throw compileError();
    node->value = tk.value;

    tk=src.consume();
    if (tk.type != tokenType::OPERATOR || tk.value != ":")
        throw compileError();

    auto newNode=new astNode;
    appendType(newNode);
    node->children.push_back(newNode);

}

void parser::appendStatementBlock(astNode* node)
{
    node->type=astNodeType::STATEMENT_BLOCK;

    while (true)
    {
        auto tk=src.peek();
        if (tk.type == tokenType::OPERATOR && tk.value == ";")
            src.consume(),tk=src.peek();
        else if (tk.type == tokenType::OPERATOR && tk.value == "}")
            break;
        else if (tk.type == tokenType::KEYWORD && tk.value == "let")
        {
            auto newNode=new astNode;
            appendLetStatement(newNode);
            node->children.push_back(newNode);
        }
        else
        {
            auto newNode=new astNode;
            appendExpressionStatement(newNode);
            node->children.push_back(newNode);
        }
    }
}

void parser::appendLetStatement(astNode* node)
{
    node->type=astNodeType::LET_STATEMENT;

    auto tk=src.consume();
    if (tk.type != tokenType::KEYWORD || tk.value != "let")
        throw compileError();

    tk=src.consume();
    if (tk.type != tokenType::IDENTIFIER)
        throw compileError();
    auto newNode=new astNode;
    newNode->type=astNodeType::IDENTIFIER;
    newNode->value=tk.value;
    node->children.push_back(newNode);

    tk=src.consume();
    if (tk.type != tokenType::OPERATOR || tk.value != ":")
        throw compileError();

    newNode=new astNode;
    appendType(newNode);
    node->children.push_back(newNode);


    tk=src.consume();
    if (tk.type == tokenType::OPERATOR && tk.value == "=")
    {
        newNode=new astNode;
        appendExpressionStatement(newNode);
        node->children.push_back(newNode);
    }
    else if (tk.type != tokenType::OPERATOR || tk.value != ";")
        throw compileError();
}

static bool isPrefixOperend(const token& t)
{
    return
        (t.type == tokenType::OPERATOR && t.value == "+") ||
        (t.type == tokenType::OPERATOR && t.value == "-") ||
        (t.type == tokenType::OPERATOR && t.value == "~");
}
static bool isItem(const token& t)
{
    return
        t.type == tokenType::CHAR_LITERAL ||
        t.type == tokenType::STRING_LITERAL ||
        t.type == tokenType::INTEGER_LITERAL ||
        t.type == tokenType::FLOAT_LITERAL ||
        t.type == tokenType::RAW_STRING_LITERAL ||
        (t.type == tokenType::KEYWORD && (t.value == "false" || t.value == "true")) ||
        t.type == tokenType::IDENTIFIER;
}
static bool isInfixOperand(const token &t)
{
    return
        (t.type == tokenType::OPERATOR && t.value == "+") ||
        (t.type == tokenType::OPERATOR && t.value == "-") ||
        (t.type == tokenType::OPERATOR && t.value == "*") ||
        (t.type == tokenType::OPERATOR && t.value == "/") ||
        (t.type == tokenType::OPERATOR && t.value == "%") ||
        (t.type == tokenType::OPERATOR && t.value == "<<") ||
        (t.type == tokenType::OPERATOR && t.value == ">>") ||
        (t.type == tokenType::OPERATOR && t.value == "&") ||
        (t.type == tokenType::OPERATOR && t.value == "^") ||
        (t.type == tokenType::OPERATOR && t.value == "|") ||
        (t.type == tokenType::OPERATOR && t.value == "==") ||
        (t.type == tokenType::OPERATOR && t.value == "!=") ||
        (t.type == tokenType::OPERATOR && t.value == "<") ||
        (t.type == tokenType::OPERATOR && t.value == ">") ||
        (t.type == tokenType::OPERATOR && t.value == "<=") ||
        (t.type == tokenType::OPERATOR && t.value == ">=") ||
        (t.type == tokenType::OPERATOR && t.value == "&&") ||
        (t.type == tokenType::OPERATOR && t.value == "||") ||
        (t.type == tokenType::OPERATOR && t.value == "=") ||
        (t.type == tokenType::OPERATOR && t.value == "+=") ||
        (t.type == tokenType::OPERATOR && t.value == "-=") ||
        (t.type == tokenType::OPERATOR && t.value == "*=") ||
        (t.type == tokenType::OPERATOR && t.value == "/=") ||
        (t.type == tokenType::OPERATOR && t.value == "%=") ||
        (t.type == tokenType::OPERATOR && t.value == "&=") ||
        (t.type == tokenType::OPERATOR && t.value == "|=") ||
        (t.type == tokenType::OPERATOR && t.value == "^=") ||
        (t.type == tokenType::OPERATOR && t.value == "<<=") ||
        (t.type == tokenType::OPERATOR && t.value == ">>=") ||
        (t.type == tokenType::KEYWORD && t.value == "as");
}
static int getPrecedence(const token& t)
{
    if (t.type == tokenType::KEYWORD && t.value == "as")
        return 1000;
    if (t.type == tokenType::OPERATOR && (t.value == "*" || t.value == "/" || t.value =="%"))
        return 990;
    if (t.type == tokenType::OPERATOR && (t.value == "+" || t.value == "-"))
        return 980;
    if (t.type == tokenType::OPERATOR && (t.value == "<<" || t.value == ">>"))
        return 970;
    if (t.type == tokenType::OPERATOR && t.value == "&")
        return 960;
    if (t.type == tokenType::OPERATOR && t.value == "^")
        return 950;
    if (t.type == tokenType::OPERATOR && t.value == "|")
        return 940;
    if (t.type == tokenType::OPERATOR && (t.value == "==" || t.value == "!=" || t.value =="<"||
        t.value == ">" || t.value == "<=" || t.value ==">="))
        return 930;
    if (t.type == tokenType::OPERATOR && t.value == "&&")
        return 920;
    if (t.type == tokenType::OPERATOR && t.value == "||")
        return 910;;
    if (t.type == tokenType::OPERATOR && (t.value == "=" || t.value == "+=" || t.value =="-="||
        t.value == "*=" || t.value == "/=" || t.value == "%=" || t.value == "&=" || t.value == "|=" ||
        t.value == "^=" || t.value == "<<=" || t.value == ">>="))
        return 900;
    return -1;
}
static astNode* createPrefix(const token &t)
{
    auto newNode = new astNode;
    if (t.type == tokenType::IDENTIFIER &&
        (t.value == "i32" || t.value == "u32" || t.value == "isize" || t.value == "usize"))
    {
        newNode -> type = astNodeType::TYPE;
        newNode -> value = t.value;
    }
    else if (t.type == tokenType::IDENTIFIER)
    {
        newNode -> type = astNodeType::IDENTIFIER;
        newNode -> value = t.value;
    }
    else if (t.type == tokenType::OPERATOR)
    {
        newNode -> type = astNodeType::UNARY_OPERATOR;
        newNode -> value = t.value;
    }
    else if (t.type == tokenType::CHAR_LITERAL)
    {
        newNode -> type = astNodeType::CHAR_LITERAL;
        newNode -> value = t.value;
    }
    else if (t.type == tokenType::STRING_LITERAL)
    {
        newNode -> type = astNodeType::STRING_LITERAL;
        newNode -> value = t.value;
    }
    else if (t.type == tokenType::INTEGER_LITERAL)
    {
        newNode -> type = astNodeType::INTEGER_LITERAL;
        newNode -> value = t.value;
    }
    else if (t.type == tokenType::FLOAT_LITERAL)
    {
        newNode -> type = astNodeType::FLOAT_LITERAL;
        newNode -> value = t.value;
    }
    else if (t.type == tokenType::RAW_STRING_LITERAL)
    {
        newNode -> type = astNodeType::RAW_STRING_LITERAL;
        newNode -> value = t.value;
    }
    else if (t.type == tokenType::KEYWORD && (t.value == "true" || t.value == "false"))
    {
        newNode -> type = astNodeType::BOOL_LITERAL;
        newNode -> value = t.value;
    }
    return newNode;
}
static astNode* createInfix(const token &t)
{
    auto newNode = new astNode;
    if (t.type == tokenType::OPERATOR || (t.type == tokenType::KEYWORD && t.value == "as"))
    {
        newNode -> type = astNodeType::BINARY_OPERATOR;
        newNode -> value = t.value;
    }
    return newNode;
}
static astNode* parseExp(std::vector<token> &tokens, const int precedence)
{
    astNode* node = nullptr;
    while (true)
    {
        if (tokens.empty())
            throw compileError();
        auto prefix=tokens.back();tokens.pop_back();
        if (prefix.type == tokenType::OPERATOR && prefix.value == "(")
        {
            auto newNode = parseExp(tokens,0);
            if (tokens.empty() || tokens.back().type != tokenType::OPERATOR || tokens.back().value != ")")
                throw compileError();
            tokens.pop_back();
            if (node != nullptr)
                node -> children.push_back(newNode);
            else
                node = newNode;
            break;
        }
        auto newNode = createPrefix(prefix);
        if (node != nullptr)
            node -> children.push_back(newNode);
        else
            node = newNode;
        if (isItem(prefix))
            break;
        if (!isPrefixOperend(prefix))
            throw compileError();
    }

    while (true)
    {
        if (tokens.empty())
            break;
        auto infix=tokens.back();
        if (!isInfixOperand(infix))
            break;
        auto newNode = createInfix(infix);
        const auto prec = getPrecedence(infix);
        if (prec <= precedence)
            break;
        tokens.pop_back();
        newNode -> children.push_back(node);
        node = newNode;
        node -> children.push_back(parseExp(tokens, prec));
    }
    return node;
}
void parser::appendExpressionStatement(astNode* node)
{
    node->type=astNodeType::EXPRESSION_STATEMENT;

    // get all tokens
    std::vector<token> tokens;
    while (true)
    {
        auto tk=src.consume();
        if (tk.type == tokenType::ILLEGAL)
            throw compileError();
        if (tk.type == tokenType::OPERATOR && tk.value == ";")
            break;
        tokens.push_back(tk);
    }
    if (tokens.empty())
        throw compileError();

    std::reverse(tokens.begin(),tokens.end());

    // break statement
    if (tokens.back().type == tokenType::KEYWORD && tokens.back().value == "break")
    {
        if (tokens.size() > 1)
            throw compileError();
        auto newNode=new astNode;
        newNode->type = astNodeType::BREAK;
        node->children.push_back(newNode);
        return ;
    }

    // continue statement
    if (tokens.back().type == tokenType::KEYWORD && tokens.back().value == "continue")
    {
        if (tokens.size() > 1)
            throw compileError();
        auto newNode=new astNode;
        newNode->type = astNodeType::CONTINUE;
        node->children.push_back(newNode);
        return ;
    }

    // return statement
    if (tokens.back().type == tokenType::KEYWORD && tokens.back().value == "return")
    {
        auto newNode=new astNode;
        newNode->type = astNodeType::RETURN;
        node->children.push_back(newNode);
        if (tokens.size() == 1)
            return ;
        tokens.pop_back();
        node=newNode;
    }

    // pratt parser for complicated expression
    node->children.push_back(parseExp(tokens, 0));
    if (!tokens.empty())
        throw compileError();
}

astNode* parser::solve()
{
    auto root=new astNode;
    appendProgram(root);
    return root;
}