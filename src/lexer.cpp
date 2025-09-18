#include "lexer.h"

inline bool isBinLiteral(const char& ch){return ch>='0'&&ch<='1';}
inline bool isOctLiteral(const char& ch){return ch>='0'&&ch<='7';}
inline bool isDecLiteral(const char& ch){return ch>='0'&&ch<='9';}
inline bool isHexLiteral(const char& ch){return (ch>='0'&&ch<='9')||(ch>='a'&&ch<='f')||(ch>='A'&&ch<='F');}
inline bool isLetter(const char& ch){return (ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z');}

lexer::lexer(const std::string &code):code(code),pos(0){}

token lexer::consume()
{
    pos=matchWhitespace(pos);
    if(pos==code.size())
        return {tokenType::ILLEGAL,""};
    int trial=matchCharLiteral(pos);
    if(trial!=-1)
    {
        const auto res=(token){tokenType::CHAR_LITERAL,code.substr(pos+1,trial-pos-2)};
        pos=trial;
        return previous=res;
    }
    trial=matchRawStringLiteral(pos);
    if(trial!=-1)
    {
        const auto res=(token){tokenType::RAW_STRING_LITERAL,code.substr(pos+1,trial-pos-2)};
        pos=trial;
        return previous=res;
    }
    trial=matchStringLiteral(pos);
    if(trial!=-1)
    {
        const auto res=(token){tokenType::STRING_LITERAL,code.substr(pos+1,trial-pos-2)};
        pos=trial;
        return previous=res;
    }
    trial=matchFloatLiteral(pos);
    if(trial!=-1)
    {
        const auto res=(token){tokenType::FLOAT_LITERAL,code.substr(pos,trial-pos)};
        pos=trial;
        return previous=res;
    }
    trial=matchIntegerLiteral(pos);
    if(trial!=-1)
    {
        const auto res=(token){tokenType::INTEGER_LITERAL,code.substr(pos,trial-pos)};
        pos=trial;
        return previous=res;
    }
    trial=matchKeyword(pos);
    if(trial!=-1 && (trial == code.size() || !(isLetter(code[trial])||isDecLiteral(code[trial])||code[trial]=='_')))
    {
        const auto res=(token){tokenType::KEYWORD,code.substr(pos,trial-pos)};
        pos=trial;
        return previous=res;
    }
    trial=matchOperator(pos);
    if(trial!=-1)
    {
        const auto res=(token){tokenType::OPERATOR,code.substr(pos,trial-pos)};
        pos=trial;
        return previous=res;
    }
    trial=matchIdentifier(pos);
    if(trial!=-1)
    {
        const auto res=(token){tokenType::IDENTIFIER,code.substr(pos,trial-pos)};
        pos=trial;
        return previous=res;
    }
    return {tokenType::ILLEGAL,""};
}

token lexer::peek() const
{
    const int newPos=matchWhitespace(pos);
    if(newPos==code.size())
        return (token){tokenType::ILLEGAL,""};
    int trial=matchCharLiteral(newPos);
    if(trial!=-1)
        return (token){tokenType::CHAR_LITERAL,code.substr(newPos+1,trial-newPos-2)};
    trial=matchStringLiteral(newPos);
    if(trial!=-1)
        return (token){tokenType::STRING_LITERAL,code.substr(newPos+1,trial-newPos-2)};
    trial=matchFloatLiteral(newPos);
    if(trial!=-1)
        return (token){tokenType::FLOAT_LITERAL,code.substr(newPos,trial-newPos)};
    trial=matchIntegerLiteral(newPos);
    if(trial!=-1)
        return (token){tokenType::INTEGER_LITERAL,code.substr(newPos,trial-newPos)};
    trial=matchKeyword(newPos);
    if(trial!=-1 && (trial == code.size() || !(isLetter(code[trial])||isDecLiteral(code[trial])||code[trial]=='_')))
        return (token){tokenType::KEYWORD,code.substr(newPos,trial-newPos)};
    trial=matchOperator(newPos);
    if(trial!=-1)
        return (token){tokenType::OPERATOR,code.substr(newPos,trial-newPos)};
    trial=matchIdentifier(newPos);
    if(trial!=-1)
        return (token){tokenType::IDENTIFIER,code.substr(newPos,trial-newPos)};
    return {tokenType::ILLEGAL,""};
}

token lexer::expect(const token& exp)
{
    const auto curToken=consume();
    if (curToken != exp)
        throw compileError();
    return curToken;
}

token lexer::expect(const tokenType& exp)
{
    const auto curToken=consume();
    if (curToken.type != exp)
        throw compileError();
    return curToken;
}

int lexer::matchWhitespace(int pos) const
{
    int sta=0;
    while(pos<code.size())
    {
        if(sta==-1)
        {
            if(code[pos]=='\n')
                sta=0;
            pos++;
        }
        else if(sta>0)
        {
            if(pos+1<code.size()&&code[pos]=='/'&&code[pos+1]=='*')
                sta++,pos+=2;
            else if(pos+1<code.size()&&code[pos]=='*'&&code[pos+1]=='/')
                sta--,pos+=2;
            else
                pos++;
        }
        else
        {
            if(code[pos]==' '||code[pos]=='\t'||code[pos]=='\n'||code[pos]=='\r')
                pos++;
            else if(pos+1<code.size()&&code[pos]=='/'&&code[pos+1]=='/')
                sta=-1,pos+=2;
            else if(pos+1<code.size()&&code[pos]=='/'&&code[pos+1]=='*')
                sta=1,pos+=2;
            else
                break;
        }
    }
    if(sta)
        throw compileError();
    return pos;
}

int lexer::matchSingleChar(int pos,bool singleQuote=false) const
{
    if(pos==code.size())
        return -1;
    if (singleQuote && code[pos] == '\'')
        return pos+1;
    if(code[pos]!='\''&&code[pos]!='\\')
        return pos+1;
    if(pos+1<code.size()&&code[pos]=='\\'&&(code[pos+1]=='\''||code[pos+1]=='\"'||code[pos+1]=='n'||code[pos+1]=='r'
        ||code[pos+1]=='t'||code[pos+1]=='\\'||code[pos+1]=='0'))
        return pos+2;
    if(pos+3<code.size()&&code[pos]=='\\'&&code[pos+1]=='x'&&isOctLiteral(code[pos+2]&&isHexLiteral(pos+3)))
        return pos+3;
    return -1;
}


int lexer::matchCharLiteral(int pos) const
{
    if(pos+2>=code.size()||code[pos]!='\'')
        return -1;
    pos=matchSingleChar(pos+1);
    if(pos==-1||pos==code.size()||code[pos]!='\'')
        return -1;
    return pos+1;
}

int lexer::matchRawStringLiteral(int pos) const
{
    return -1;
}

int lexer::matchStringLiteral(int pos) const
{
    if(pos==code.size()||code[pos]!='\"')
        return -1;
    pos++;
    while(true)
    {
        if(pos==code.size()||pos==-1)
            return -1;
        if(code[pos]=='\"')
            return pos+1;
        int prePos=pos;
        pos=matchSingleChar(pos,true);
    }
}

int lexer::matchIntegerLiteral(int pos) const
{
    if(pos==code.size())
        return -1;
    if (code[pos]=='-')
        pos++;
    if(pos==code.size())
        return -1;
    if(code[pos]=='0'&&pos+1<code.size())
    {
        if(code[pos+1]=='b')
        {
            pos+=2;
            int has=false;
            while(true)
            {
                if(pos==code.size()||(code[pos]!='_'&&!isBinLiteral(code[pos])))
                    break;
                if(code[pos]!='_')
                    has=1;
                pos++;
            }
            if(!has)
                return -1;
            return pos;
        }
        if(code[pos+1]=='o')
        {
            pos+=2;
            int has=false;
            while(true)
            {
                if(pos==code.size()||(code[pos]!='_'&&!isOctLiteral(code[pos])))
                    break;
                if(code[pos]!='_')
                    has=1;
                pos++;
            }
            if(!has)
                return -1;
            return pos;
        }
        if(code[pos+1]=='x')
        {
            pos+=2;
            int has=false;
            while(true)
            {
                if(pos==code.size()||(code[pos]!='_'&&!isHexLiteral(code[pos])))
                    break;
                if(code[pos]!='_')
                    has=1;
                pos++;
            }
            if(!has)
                return -1;
            return pos;
        }
    }
    if(!isDecLiteral(code[pos]))
        return -1;
    pos++;
    while(pos!=code.size()&&(code[pos]=='_'||isOctLiteral(code[pos])))
        pos++;
    return pos;
}

int lexer::matchFloatLiteral(int pos) const
{
    return -1;
    if(pos==code.size())
        return -1;
    if (code[pos]=='-')
        pos++;
    if(pos==code.size()||!isDecLiteral(code[pos]))
        return -1;
    pos++;
    while(pos!=code.size()&&(isDecLiteral(code[pos])||code[pos]=='_'))
        pos++;
    if(pos==code.size()||code[pos]!='.')
        return -1;
    pos++;
    while(pos!=code.size()&&(isDecLiteral(code[pos])||code[pos]=='_'))
        pos++;
    return pos;
}

int lexer::matchKeyword(int pos) const
{
    if(pos+1<code.size()&&code.substr(pos,2)=="as")
        return pos+2;
    if(pos+4<code.size()&&code.substr(pos,5)=="break")
        return pos+5;
    if(pos+4<code.size()&&code.substr(pos,5)=="const")
        return pos+5;
    if(pos+7<code.size()&&code.substr(pos,8)=="continue")
        return pos+8;
    if(pos+3<code.size()&&code.substr(pos,4)=="else")
        return pos+4;
    if(pos+3<code.size()&&code.substr(pos,4)=="enum")
        return pos+4;
    if(pos+4<code.size()&&code.substr(pos,5)=="false")
        return pos+5;
    if(pos+1<code.size()&&code.substr(pos,2)=="fn")
        return pos+2;
    if(pos+2<code.size()&&code.substr(pos,3)=="for")
        return pos+3;
    if(pos+1<code.size()&&code.substr(pos,2)=="if")
        return pos+2;
    if(pos+3<code.size()&&code.substr(pos,4)=="impl")
        return pos+4;
    if(pos+1<code.size()&&code.substr(pos,2)=="in")
        return pos+2;
    if(pos+2<code.size()&&code.substr(pos,3)=="let")
        return pos+3;
    if(pos+3<code.size()&&code.substr(pos,4)=="loop")
        return pos+4;
    if(pos+4<code.size()&&code.substr(pos,5)=="match")
        return pos+5;
    if(pos+2<code.size()&&code.substr(pos,3)=="mod")
        return pos+3;
    if(pos+2<code.size()&&code.substr(pos,3)=="mut")
        return pos+3;
    if(pos+5<code.size()&&code.substr(pos,6)=="return")
        return pos+6;
    if(pos+3<code.size()&&code.substr(pos,4)=="self")
        return pos+4;
    if(pos+3<code.size()&&code.substr(pos,4)=="Self")
        return pos+4;
    if(pos+5<code.size()&&code.substr(pos,6)=="struct")
        return pos+6;
    if(pos+4<code.size()&&code.substr(pos,5)=="trait")
        return pos+5;
    if(pos+3<code.size()&&code.substr(pos,4)=="true")
        return pos+4;
    if(pos+4<code.size()&&code.substr(pos,5)=="while")
        return pos+5;
    return -1;
}

int lexer::matchOperator(int pos) const
{
    if(pos+2<code.size()&&code.substr(pos,3)=="<<=")
        return pos+3;
    if(pos+2<code.size()&&code.substr(pos,3)==">>=")
        return pos+3;
    if(pos+2<code.size()&&code.substr(pos,3)=="...")
        return pos+3;
    if(pos+2<code.size()&&code.substr(pos,3)=="..=")
        return pos+3;
    if(pos+1<code.size()&&code.substr(pos,2)=="<=")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="==")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="!=")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)==">=")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="&&")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="||")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="<<")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)==">>")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="+=")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="-=")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="*=")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="/=")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="%=")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="^=")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="&=")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="|=")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="..")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="::")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="->")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="<-")
        return pos+2;
    if(pos+1<code.size()&&code.substr(pos,2)=="=>")
        return pos+2;
    if(pos<code.size()&&code[pos]=='=')
        return pos+1;
    if(pos<code.size()&&code[pos]=='<')
        return pos+1;
    if(pos<code.size()&&code[pos]=='>')
        return pos+1;
    if(pos<code.size()&&code[pos]=='!')
        return pos+1;
    if(pos<code.size()&&code[pos]=='~')
        return pos+1;
    if(pos<code.size()&&code[pos]=='+')
        return pos+1;
    if(pos<code.size()&&code[pos]=='-')
        return pos+1;
    if(pos<code.size()&&code[pos]=='*')
        return pos+1;
    if(pos<code.size()&&code[pos]=='/')
        return pos+1;
    if(pos<code.size()&&code[pos]=='%')
        return pos+1;
    if(pos<code.size()&&code[pos]=='^')
        return pos+1;
    if(pos<code.size()&&code[pos]=='&')
        return pos+1;
    if(pos<code.size()&&code[pos]=='|')
        return pos+1;
    if(pos<code.size()&&code[pos]=='@')
        return pos+1;
    if(pos<code.size()&&code[pos]=='.')
        return pos+1;
    if(pos<code.size()&&code[pos]==',')
        return pos+1;
    if(pos<code.size()&&code[pos]==';')
        return pos+1;
    if(pos<code.size()&&code[pos]==':')
        return pos+1;
    if(pos<code.size()&&code[pos]=='#')
        return pos+1;
    if(pos<code.size()&&code[pos]=='$')
        return pos+1;
    if(pos<code.size()&&code[pos]=='?')
        return pos+1;
    if(pos<code.size()&&code[pos]=='{')
        return pos+1;
    if(pos<code.size()&&code[pos]=='}')
        return pos+1;
    if(pos<code.size()&&code[pos]=='[')
        return pos+1;
    if(pos<code.size()&&code[pos]==']')
        return pos+1;
    if(pos<code.size()&&code[pos]=='(')
        return pos+1;
    if(pos<code.size()&&code[pos]==')')
        return pos+1;
    return -1;
}

int lexer::matchIdentifier(int pos) const
{
    if(pos==code.size())
        return -1;
    if(!isLetter(code[pos])&&code[pos]!='_')
        return -1;
    pos++;
    while(isLetter(code[pos])||isDecLiteral(code[pos])||code[pos]=='_')
        pos++;
    return pos;
}

bool lexer::checkEnd() const {return matchWhitespace(pos)==code.size();}
