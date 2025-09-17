#include "parser.h"

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

void parser::appendExpressionStatement(astNode* node)
{
    node->type=astNodeType::EXPRESSION_STATEMENT;

    std::cerr<<"I'm always trying to do so!"<<std::endl;
    throw compileError();
}

astNode* parser::solve()
{
    auto root=new astNode;
    appendProgram(root);
    return root;
}