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

    //new
    src.expect({tokenType::KEYWORD,"fn"});
    node->value=src.expect(tokenType::IDENTIFIER).value;

    //parameters
    src.expect({tokenType::OPERATOR,"("});
    auto* newNode=new astNode;
    appendParameters(newNode);
    node->children.push_back(newNode);
    src.expect({tokenType::OPERATOR,")"});

    // -> type
    auto tk=src.peek();
    newNode=new astNode;
    if (tk == (token){tokenType::OPERATOR, "->"})
        src.consume(),appendType(newNode);
    else
    {
        newNode->type=astNodeType::TYPE;
        newNode->value = "()";
    }
    node->children.push_back(newNode);

    // statement block
    src.expect({tokenType::OPERATOR,"{"});
    newNode=new astNode;
    appendStatementBlock(newNode);
    node->children.push_back(newNode);
    src.expect({tokenType::OPERATOR,"}"});
}

void parser::appendType(astNode *node)
{
    node->type=astNodeType::TYPE;

    auto tk=src.consume();
    if (tk == (token){tokenType::OPERATOR, "("})
        src.expect({tokenType::OPERATOR, ")"});
    else if (tk == (token){tokenType::OPERATOR, "["})
    {
        auto newType=new astNode;
        node->children.push_back(newType);
        appendType(newType);
        src.expect({tokenType::OPERATOR, ";"});
        auto newExpr=new astNode;
        node->children.push_back(newExpr);
        appendSimpleExpression(newExpr);
        src.expect({tokenType::OPERATOR, "]"});
    }
    else if (tk == (token){tokenType::IDENTIFIER, "i32"})
        node->value="i32";
    else if (tk == (token){tokenType::IDENTIFIER, "u32"})
        node->value="u32";
    else if (tk == (token){tokenType::IDENTIFIER, "isize"})
        node->value="isize";
    else if (tk == (token){tokenType::IDENTIFIER, "usize"})
        node->value="usize";
    else if (tk == (token){tokenType::IDENTIFIER, "bool"})
        node->value="bool";
    else if (tk == (token){tokenType::IDENTIFIER, "char"})
        node->value="char";
    else if (tk == (token){tokenType::IDENTIFIER, "str"})
        node->value="str";
    else if (tk == (token){tokenType::IDENTIFIER, "String"})
        node->value="String";
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

    bool flag=true;
    while (tk.type == tokenType::IDENTIFIER)
    {
        if (!flag)
            throw compileError();
        flag=false;
        auto newNode=new astNode;
        appendTypedIdentifier(newNode);
        node->children.push_back(newNode);

        tk=src.peek();
        if (tk.type == tokenType::OPERATOR && tk.value == ",")
            src.consume(),tk=src.peek(),flag=true;
    }
}

void parser::appendTypedIdentifier(astNode *node)
{
    node->type=astNodeType::TYPED_IDENTIFIER;

    node->value = src.expect(tokenType::IDENTIFIER).value;
    src.expect({tokenType::OPERATOR, ":"});

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
            src.consume();
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

    src.expect({tokenType::KEYWORD, "let"});

    auto newNode=new astNode;
    newNode->type=astNodeType::IDENTIFIER;
    newNode->value=src.expect(tokenType::IDENTIFIER).value;
    node->children.push_back(newNode);

    src.expect({tokenType::OPERATOR, ":"});
    newNode=new astNode;
    appendType(newNode);
    node->children.push_back(newNode);

    auto tk=src.consume();
    if (tk == (token){tokenType::OPERATOR, "="})
    {
        newNode=new astNode;
        appendSimpleExpression(newNode);
        node->children.push_back(newNode);
        src.expect({tokenType::OPERATOR, ";"});
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
        (t.type == tokenType::OPERATOR && t.value == "::") ||
        (t.type == tokenType::OPERATOR && t.value == ".") ||
        (t.type == tokenType::KEYWORD && t.value == "as");
}
static int getPrecedence(const token& t)
{
    if (t.type == tokenType::OPERATOR && t.value == "::")
        return 1020;
    if (t.type == tokenType::OPERATOR && t.value == ".")
        return 1010;
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
astNode* parser::parseExp(const int precedence)
{
    astNode* node = nullptr, *cur = nullptr;
    while (true)
    {
        auto prefix=src.consume();
        // left bracket
        if (prefix.type == tokenType::OPERATOR && prefix.value == "(")
        {
            auto newNode = parseExp(0);
            src.expect({tokenType::OPERATOR,")"});
            if (cur != nullptr)
                cur -> children.push_back(newNode);
            else
                node = newNode;
            break;
        }
        // block expression
        else if (prefix.type == tokenType::OPERATOR && prefix.value == "{")
        {
            auto newNode=new astNode;
            appendStatementBlock(newNode);
            src.expect({tokenType::OPERATOR, "}"});
            if (cur != nullptr)
                cur -> children.push_back(newNode);
            else
                node = newNode;
            break;
        }
        // array
        if (prefix.type == tokenType::OPERATOR && prefix.value == "[")
        {
            auto newNode = new astNode;
            newNode -> type = astNodeType::ARRAY_BUILD;
            auto newGroup = new astNode;
            newNode -> children.push_back(newGroup);
            appendGroupExpression(newGroup);
            src.expect({tokenType::OPERATOR,"]"});
            if (cur != nullptr)
                cur -> children.push_back(newNode);
            else
                node = newNode;
            break;
        }
        // loop statement
        if (prefix.type == tokenType::KEYWORD && prefix.value == "loop")
        {
            src.expect({tokenType::OPERATOR, "{"});
            auto newNode=new astNode;
            newNode->type = astNodeType::LOOP;
            if (cur != nullptr)
                cur->children.push_back(newNode);
            else
                node = newNode;
            auto newBlock=new astNode;
            newNode->children.push_back(newBlock);
            appendStatementBlock(newBlock);
            src.expect({tokenType::OPERATOR, "}"});
            break;
        }
        //while statement
        if (prefix.type == tokenType::KEYWORD && prefix.value == "while")
        {
            auto newNode=new astNode;
            newNode->type = astNodeType::WHILE;
            if (cur != nullptr)
                cur->children.push_back(newNode);
            else
                node = newNode;

            src.expect({tokenType::OPERATOR, "("});
            auto newBlock=new astNode;
            newNode->children.push_back(newBlock);
            appendSimpleExpression(newBlock);
            src.expect({tokenType::OPERATOR, ")"});

            src.expect({tokenType::OPERATOR, "{"});
            newBlock=new astNode;
            newNode->children.push_back(newBlock);
            appendStatementBlock(newBlock);
            src.expect({tokenType::OPERATOR, "}"});
            break;
        }
        //if statement
        if (prefix.type == tokenType::KEYWORD && prefix.value == "if")
        {
            auto newNode=new astNode;
            newNode->type = astNodeType::IF;
            if (cur != nullptr)
                cur->children.push_back(newNode);
            else
                node = newNode;
            cur = newNode;

            src.expect({tokenType::OPERATOR, "("});
            auto newBlock=new astNode;
            newNode->children.push_back(newBlock);
            appendSimpleExpression(newBlock);
            src.expect({tokenType::OPERATOR, ")"});

            src.expect({tokenType::OPERATOR, "{"});
            newBlock=new astNode;
            newNode->children.push_back(newBlock);
            appendStatementBlock(newBlock);
            src.expect({tokenType::OPERATOR, "}"});
            if (src.peek() != (token){tokenType::KEYWORD, "else"})
                break;
            else
                continue;
        }
        //else statement
        if (prefix.type == tokenType::KEYWORD && prefix.value == "else")
        {
            auto newNode=new astNode;
            newNode->type = astNodeType::ELSE;
            if (cur != nullptr)
                cur->children.push_back(newNode);
            else
                node = newNode;
            cur = newNode;
            auto tk = src.peek();
            if (tk == (token){tokenType::KEYWORD, "if"})
                continue;
            src.expect({tokenType::OPERATOR, "{"});
            auto newBlock=new astNode;
            newNode->children.push_back(newBlock);
            appendStatementBlock(newBlock);
            src.expect({tokenType::OPERATOR, "}"});
            break;
        }
        // return statement
        if (prefix.type == tokenType::KEYWORD && prefix.value == "return")
        {
            auto newNode=new astNode;
            if (cur != nullptr)
                cur->children.push_back(newNode);
            else
                node = newNode;
            cur = newNode;
            newNode->type = astNodeType::RETURN;
            if (src.peek() != (token){tokenType::OPERATOR, ";"})
            {
                auto newExpr=new astNode;
                newNode->children.push_back(newExpr);
                appendSimpleExpression(newExpr);
            }
            return node;
        }
        auto newNode = createPrefix(prefix);
        if (cur != nullptr)
            cur -> children.push_back(newNode);
        else
            node = newNode;
        cur = newNode;
        if (isItem(prefix))
            break;
        if (!isPrefixOperend(prefix))
            throw compileError();
    }

    while (true)
    {
        auto infix=src.peek();
        if (infix == (token){tokenType::OPERATOR, "("})
        {
            src.consume();
            auto newNode = new astNode;
            newNode -> type = astNodeType::FUNCTION_CALL;
            newNode -> children.push_back(node);
            node = newNode;
            newNode = new astNode;
            node -> children.push_back(newNode);
            appendGroupExpression(newNode);
            src.expect({tokenType::OPERATOR, ")"});
            continue;
        }
        if (infix == (token){tokenType::OPERATOR, "["})
        {
            src.consume();
            auto newNode = new astNode;
            newNode -> type = astNodeType::ARRAY_INDEX;
            newNode -> children.push_back(node);
            node = newNode;
            newNode = new astNode;
            node -> children.push_back(newNode);
            appendSimpleExpression(newNode);
            src.expect({tokenType::OPERATOR, "]"});
            continue;
        }
        if (!isInfixOperand(infix))
            break;
        auto newNode = createInfix(infix);
        const auto prec = getPrecedence(infix);
        if (prec <= precedence)
            break;
        src.consume();
        newNode -> children.push_back(node);
        node = newNode;
        node -> children.push_back(parseExp(prec));
    }
    return node;
}
void parser::appendSimpleExpression(astNode* node)
{
    // pratt parser for complicated expression
    node->type=astNodeType::EXPRESSION_STATEMENT;
    node->children.push_back(parseExp(0));
}
void parser::appendExpressionStatement(astNode* node)
{
    node->type=astNodeType::EXPRESSION_STATEMENT;

    auto tk=src.peek();

    // break statement
    if (tk.type == tokenType::KEYWORD && tk.value == "break")
    {
        src.consume();
        auto newNode=new astNode;
        node->children.push_back(newNode);
        tk = src.peek();
        if (tk.type == tokenType::OPERATOR && tk.value == ";")
            return src.consume(),void();
        appendSimpleExpression(newNode);
        newNode->type = astNodeType::BREAK;
    }

    // continue statement
    else if (tk.type == tokenType::KEYWORD && tk.value == "continue")
    {
        src.consume();
        auto newNode=new astNode;
        newNode->type = astNodeType::CONTINUE;
        node->children.push_back(newNode);
        src.consume();
    }

    else
        appendSimpleExpression(node);
    if (src.previous != (token){tokenType::OPERATOR, "}"})
    {
        if (src.peek() == (token){tokenType::OPERATOR, "}"})
        {
            auto son=node->children.back();
            node->children.pop_back();
            auto newNode=new astNode;
            node->children.push_back(newNode);
            newNode->type = astNodeType::RETURN_CUR;
            newNode->children.push_back(son);
        }
        else
            src.expect({tokenType::OPERATOR, ";"});
    }
}

void parser::appendGroupExpression(astNode* node)
{
    node->type = astNodeType::GROUP_EXPRESSION;
    bool flag=true;
    while (true)
    {
        auto tk=src.peek();
        if (tk.type == tokenType::OPERATOR && (tk.value == ")" || tk.value == "]" || tk.value == "}"))
            break;
        if (!flag)
            throw compileError();
        flag=false;
        auto newNode = new astNode;
        node->children.push_back(newNode);
        appendSimpleExpression(newNode);
        tk=src.peek();
        if (tk == (token){tokenType::OPERATOR, ","})
            flag=true,src.consume();
    }
}

astNode* parser::solve()
{
    auto root=new astNode;
    appendProgram(root);
    return root;
}