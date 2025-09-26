#include "parser.h"

parser::parser():src(""){}

parser::parser(const std::string &code):src(code){}

void parser::appendProgram(astNode *node)
{
    node->type=astNodeType::PROGRAM;

    while (src.peek().type != tokenType::ILLEGAL)
        appendItem(node);
    if (!src.checkEnd())
        throw compileError();
}

void parser::appendStruct(astNode *node)
{
    node->type = astNodeType::STRUCT;

    src.expect({tokenType::KEYWORD,"struct"});
    node->value=src.expect(tokenType::IDENTIFIER).value;

    auto newNode = new astNode;
    newNode->type = astNodeType::PARAMETERS;
    node->children.push_back(newNode);
    if (src.peek() != (token){tokenType::OPERATOR, ";"})
    {
        src.expect({tokenType::OPERATOR,"{"});
        appendParameters(newNode);
        src.expect({tokenType::OPERATOR,"}"});
    }
    else
        src.expect({tokenType::OPERATOR,";"});
}

void parser::appendEnum(astNode *node)
{
    node->type = astNodeType::ENUM;

    src.expect({tokenType::KEYWORD, "enum"});
    node -> value = src.expect(tokenType::IDENTIFIER).value;

    src.expect({tokenType::OPERATOR, "{"});
    bool flag=true;
    while (src.peek() != (token){tokenType::OPERATOR, "}"})
    {
        if (!flag)
            throw compileError();
        flag = false;
        auto newId = new astNode;
        newId -> type = astNodeType::IDENTIFIER;
        newId -> value = src.expect(tokenType::IDENTIFIER).value;
        node -> children.push_back(newId);

        if (src.peek() == (token){tokenType::OPERATOR, ","})
            flag = true, src.consume();
    }
    src.expect({tokenType::OPERATOR, "}"});
}

void parser::appendFunction(astNode *node)
{
    node->type=astNodeType::FUNCTION;

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
    if (src.peek() == (token){tokenType::OPERATOR, ";"})
        src.consume();
    else
    {
        src.expect({tokenType::OPERATOR,"{"});
        newNode=new astNode;
        appendStatementBlock(newNode);
        node->children.push_back(newNode);
        src.expect({tokenType::OPERATOR,"}"});
    }
}

void parser::appendType(astNode *node)
{
    node->type=astNodeType::TYPE;

    auto tk=src.consume();
    if (tk == (token){tokenType::OPERATOR, "("})
        src.expect({tokenType::OPERATOR, ")"}), node->value = "()";
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
    else if (tk.type == tokenType::IDENTIFIER)
        node->value = tk.value;
    else if (tk == (token){tokenType::KEYWORD, "Self"})
        node->value = "Self";
    else
        throw compileError();
}

void parser::appendParameters(astNode *node)
{
    node->type=astNodeType::PARAMETERS;

    auto tk=src.peek();
    bool flag=true;
    if (tk.type == tokenType::KEYWORD && tk.value == "self")
    {
        src.consume();
        flag = false;
        auto newNode = new astNode;
        newNode -> type = astNodeType::SELF;
        node -> children.push_back(newNode);

        auto newType = new astNode;
        newType -> type = astNodeType::TYPE;
        newNode->children.push_back(newType);
        if (src.peek() == (token){tokenType::OPERATOR, ":"})
        {
            src.consume();
            appendType(newType);
        }
        else
            newType -> value = "Self";

        tk=src.peek();
        if (tk.type == tokenType::OPERATOR && tk.value == ",")
            src.consume(),tk=src.peek(),flag = true;
    }

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
        if (tk.type == tokenType::KEYWORD && (tk.value == "fn" || tk.value == "struct" || tk.value == "enum" ||
            tk.value == "const" || tk.value == "trait" || tk.value == "impl"))
            appendItem(node);
        else if (tk.type == tokenType::OPERATOR && tk.value == ";")
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

    auto quant=new astNode;
    quant->type=astNodeType::QUANTIFIER;
    node->children.push_back(quant);
    if (src.peek() == (token){tokenType::KEYWORD, "mut"})
        quant->value="mut", src.consume();
    else
        quant->value="const";

    node->value=src.expect(tokenType::IDENTIFIER).value;

    auto newNode=new astNode;
    node->children.push_back(newNode);
    if (src.peek() == (token){tokenType::OPERATOR, ":"})
    {
        src.consume();
        appendType(newNode);
    }
    else if(node->value == "_")
    {
        newNode->type = astNodeType::TYPE;
        newNode->value = "_";
    }
    else
        throw compileError();

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

void parser::appendConstStatement(astNode* node)
{
    node->type=astNodeType::CONST_STATEMENT;

    src.expect({tokenType::KEYWORD, "const"});

    node->value=src.expect(tokenType::IDENTIFIER).value;

    src.expect({tokenType::OPERATOR, ":"});
    auto newNode=new astNode;
    appendType(newNode);
    node->children.push_back(newNode);

    src.expect({tokenType::OPERATOR, "="});
    newNode=new astNode;
    appendSimpleExpression(newNode);
    node->children.push_back(newNode);
    src.expect({tokenType::OPERATOR, ";"});
}

static bool isPrefixOperend(const token& t)
{
    return
        (t.type == tokenType::OPERATOR && t.value == "-") ||
        (t.type == tokenType::OPERATOR && t.value == "!");
}
static bool isItem(const token& t)
{
    return
        t.type == tokenType::CHAR_LITERAL ||
        t.type == tokenType::STRING_LITERAL ||
        t.type == tokenType::INTEGER_LITERAL ||
        t.type == tokenType::FLOAT_LITERAL ||
        t.type == tokenType::RAW_STRING_LITERAL ||
        (t.type == tokenType::KEYWORD && (t.value == "false" || t.value == "true" ||
            t.value == "self" || t.value == "Self" || t.value == "loop" || t.value == "while" ||
            t.value == "if" || t.value =="else" || t.value == "return" || t.value == "break" ||
            t.value == "continue")) ||
        t.type == tokenType::IDENTIFIER ||
        t == (token){tokenType::OPERATOR, "("} ||
        t == (token){tokenType::OPERATOR, "{"} ||
        t == (token){tokenType::OPERATOR, "["} ;
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
        (t.type == tokenType::OPERATOR && t.value == "(") ||
        (t.type == tokenType::OPERATOR && t.value == "[") ||
        (t.type == tokenType::OPERATOR && t.value == "{") ||
        (t.type == tokenType::KEYWORD && t.value == "as");
}
static int getPrecedence(const token& t)
{
    if (t.type == tokenType::OPERATOR && t.value == "::")
        return 1030;
    if (t.type == tokenType::OPERATOR && t.value == ".")
        return 1020;
    if (t.type == tokenType::OPERATOR && (t.value == "(" || t.value == "[" || t.value =="{"))
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
        (t.value == "i32" || t.value == "u32" || t.value == "isize" || t.value == "usize" || t.value == "bool" ||
            t.value == "char" || t.value == "str" || t.value == "String"))
    {
        newNode -> type = astNodeType::TYPE;
        newNode -> value = t.value;
    }
    else if (t == (token){tokenType::OPERATOR, "("})
    {
        newNode -> type = astNodeType::TYPE;
        newNode -> value = "()";
    }
    else if (t == (token){tokenType::KEYWORD, "return"})
        newNode -> type = astNodeType::RETURN;
    else if (t == (token){tokenType::KEYWORD, "break"})
        newNode -> type = astNodeType::BREAK;
    else if (t == (token){tokenType::KEYWORD, "continue"})
        newNode -> type = astNodeType::CONTINUE;
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
    else if (t.type == tokenType::KEYWORD && t.value == "self")
        newNode -> type = astNodeType::SELF;
    else if (t.type == tokenType::KEYWORD && t.value == "Self")
    {
        newNode -> type = astNodeType::TYPE;
        newNode -> value = "Self";
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
astNode* parser::parseExp(const int precedence, const bool firstFlag = false)
{
    astNode* node = nullptr, *cur = nullptr;
    while (true)
    {
        auto prefix=src.consume();
        // left bracket
        if (prefix.type == tokenType::OPERATOR && prefix.value == "(")
        {
            if (src.peek() == (token){tokenType::OPERATOR, ")"})
                src.consume();
            else
            {
                auto newNode = parseExp(0);
                src.expect({tokenType::OPERATOR,")"});
                if (cur != nullptr)
                    cur -> children.push_back(newNode);
                else
                    node = newNode;
                break;
            }
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
            if (firstFlag)
                return node;
            break;
        }
        // array
        else if (prefix.type == tokenType::OPERATOR && prefix.value == "[")
        {
            auto newNode = new astNode;
            newNode -> type = astNodeType::ARRAY_BUILD;
            auto newGroup = new astNode;
            int type = 0; // 1 -> [a, b] ; -1 -> [a; b]
            bool flag = true; // whether ',' or ';' seperated
            while (src.peek() != (token){tokenType::OPERATOR, "]"})
            {
                if (!flag)
                    throw compileError();
                auto newExpr = new astNode;
                appendSimpleExpression(newExpr);
                flag = false;
                newGroup->children.push_back(newExpr);
                if (src.peek() == (token){tokenType::OPERATOR, ","})
                {
                    src.consume();
                    if (type == -1)
                        throw compileError();
                    type = 1;
                    flag = true;
                }
                else if (src.peek() == (token){tokenType::OPERATOR, ";"})
                {
                    src.consume();
                    if (type == 1)
                        throw compileError();
                    type = -1;
                    flag = true;
                }
            }
            if (type == 0)
                type = 1;
            if (type == 1)
            {
                newNode->children.push_back(newGroup);
                newGroup->type = astNodeType::GROUP_EXPRESSION;
            }
            else
            {
                if (flag)
                    throw compileError();
                if (newGroup->children.size() != 2)
                    throw compileError();
                newNode->children = newGroup->children;
            }
            src.expect({tokenType::OPERATOR, "]"});
            if (cur != nullptr)
                cur -> children.push_back(newNode);
            else
                node = newNode;
            break;
        }
        // loop statement
        else if (prefix.type == tokenType::KEYWORD && prefix.value == "loop")
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
            if (firstFlag)
                return node;
            break;
        }
        //while statement
        else if (prefix.type == tokenType::KEYWORD && prefix.value == "while")
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
            if (firstFlag)
                return node;
            break;
        }
        //if statement
        else if (prefix.type == tokenType::KEYWORD && prefix.value == "if")
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
            {
                if (firstFlag)
                    return node;
                break;
            }
            continue;
        }
        //else statement
        else if (prefix.type == tokenType::KEYWORD && prefix.value == "else")
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
            if (firstFlag)
                return node;
            break;
        }
        // return statement
        else if (prefix.type == tokenType::KEYWORD && prefix.value == "return")
        {
            if (isItem(src.peek()) || isPrefixOperend(src.peek()))
            {
                auto newNode=new astNode;
                if (cur != nullptr)
                    cur->children.push_back(newNode);
                else
                    node = newNode;
                cur = newNode;
                newNode->type = astNodeType::RETURN;
                auto newExpr=new astNode;
                newNode->children.push_back(newExpr);
                appendSimpleExpression(newExpr);
                return node;
            }
        }
        // break statement
        else if (prefix.type == tokenType::KEYWORD && prefix.value == "break")
        {
            if (isItem(src.peek()) || isPrefixOperend(src.peek()))
            {
                auto newNode=new astNode;
                if (cur != nullptr)
                    cur->children.push_back(newNode);
                else
                    node = newNode;
                cur = newNode;
                newNode->type = astNodeType::BREAK;
                auto newExpr=new astNode;
                newNode->children.push_back(newExpr);
                appendSimpleExpression(newExpr);
                return node;
            }
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
        if (!isInfixOperand(infix))
            break;
        auto prec = getPrecedence(infix);
        if (prec <= precedence)
            break;
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
        if (infix == (token){tokenType::OPERATOR, "{"})
        {
            src.consume();
            auto newNode = new astNode;
            newNode -> type = astNodeType::STRUCT_BUILD;
            newNode -> children.push_back(node);
            node = newNode;
            newNode = new astNode;
            newNode -> type = astNodeType::FIELDS;
            node -> children.push_back(newNode);
            // {ID: Expr, ...}
            bool flag = true;
            while (src.peek() != (token){tokenType::OPERATOR,"}"})
            {
                if (!flag)
                    throw compileError();
                flag=false;
                auto newField = new astNode;
                newField -> type = astNodeType::FIELD;
                newNode -> children.push_back(newField);
                newField -> value = src.expect(tokenType::IDENTIFIER).value;

                src.expect({tokenType::OPERATOR, ":"});
                auto newExpr = new astNode;
                newField -> children.push_back(newExpr);
                appendSimpleExpression(newExpr);

                if (src.peek() == (token){tokenType::OPERATOR, ","})
                    flag=true, src.consume();
            }
            src.expect({tokenType::OPERATOR, "}"});
            continue;
        }
        auto newNode = createInfix(infix);
        src.consume();
        newNode -> children.push_back(node);
        node = newNode;
        // right associative
        if (prec == 900)
            prec --;
        node -> children.push_back(parseExp(prec));
    }
    return node;
}
void parser::appendSimpleExpression(astNode* node, bool firstFlag)
{
    // pratt parser for complicated expression
    node->type=astNodeType::EXPRESSION_STATEMENT;
    node->children.push_back(parseExp(0, firstFlag));
}
void parser::appendExpressionStatement(astNode* node)
{
    node->type=astNodeType::EXPRESSION_STATEMENT;

    auto tk=src.peek();

    appendSimpleExpression(node, true);

    if (src.peek() != (token){tokenType::OPERATOR, ";"})
        node->type = astNodeType::RETURN_CUR;
    else
        src.consume();
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

void parser::appendItem(astNode* node)
{
    auto tk=src.peek();
    if (tk.type != tokenType::KEYWORD)
        throw compileError();
    if (tk.value == "fn")
    {
        auto newNode = new astNode;
        node->children.push_back(newNode);
        appendFunction(newNode);
    }
    else if (tk.value == "struct")
    {
        auto newNode = new astNode;
        node->children.push_back(newNode);
        appendStruct(newNode);
    }
    else if (tk.value == "enum")
    {
        auto newNode = new astNode;
        node->children.push_back(newNode);
        appendEnum(newNode);
    }
    else if (tk.value == "const")
    {
        auto newNode = new astNode;
        node->children.push_back(newNode);
        appendConstStatement(newNode);
    }
    else if (tk.value == "impl")
    {
        auto newNode = new astNode;
        node->children.push_back(newNode);
        appendImpl(newNode);
    }
    else if (tk.value == "trait")
    {
        auto newNode = new astNode;
        node->children.push_back(newNode);
        appendTrait(newNode);
    }
    else
        throw compileError();
}

void parser::appendAssociatedItem(astNode *node)
{
    node -> type = astNodeType::ASSOCIATED_ITEM;
    while (src.peek() != (token){tokenType::OPERATOR, "}"})
    {
        auto tk=src.peek();
        if (tk.type != tokenType::KEYWORD)
            throw compileError();
        if (tk.value == "fn")
        {
            auto newNode = new astNode;
            node->children.push_back(newNode);
            appendFunction(newNode);
        }
        else if (tk.value == "const")
        {
            auto newNode = new astNode;
            node->children.push_back(newNode);
            appendConstStatement(newNode);
        }
        else
            throw compileError();
    }
}

void parser::appendImpl(astNode *node)
{
    node -> type = astNodeType::IMPL;
    src.expect({tokenType::KEYWORD, "impl"});
    auto newType = new astNode;
    node -> children.push_back(newType);
    appendType(newType);

    if (src.peek() == (token){tokenType::KEYWORD, "for"})
    {
        src.consume();
        newType = new astNode;
        node -> children.push_back(newType);
        appendType(newType);
    }

    src.expect({tokenType::OPERATOR, "{"});
    auto newGroup = new astNode;
    node -> children.push_back(newGroup);
    appendAssociatedItem(newGroup);
    src.expect({tokenType::OPERATOR, "}"});
}

void parser::appendTrait(astNode *node)
{
    node -> type = astNodeType::TRAIT;
    src.expect({tokenType::KEYWORD, "trait"});
    auto newType = new astNode;
    node -> children.push_back(newType);
    appendType(newType);

    src.expect({tokenType::OPERATOR, "{"});
    auto newGroup = new astNode;
    node -> children.push_back(newGroup);
    appendAssociatedItem(newGroup);
    src.expect({tokenType::OPERATOR, "}"});
}

astNode* parser::solve()
{
    const auto root=new astNode;
    appendProgram(root);
    return root;
}