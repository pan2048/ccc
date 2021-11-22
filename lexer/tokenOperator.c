#include "lexer.h"

int scanOperator(struct token *t, int c)
{
    // Determine the token based on
    // the input character
    switch (c)
    {
    case EOF:
        t->token = T_EOF;
        return (0);
    case '+':
        if ((c = next()) == '+')
        {
            t->token = T_INC;
        }
        else if (c == '=')
        {
            t->token = T_ASPLUS;
        }
        else
        {
            putback(c);
            t->token = T_PLUS;
        }
        break;
    case '-':
        c = next();
        if (scanInteger(t, c))
        { // Negative int literal
            t->intvalue = -t->intvalue;
            break;
        }

        if (c == '-')
        {
            t->token = T_DEC;
        }
        else if (c == '>')
        {
            t->token = T_ARROW;
        }
        else if (c == '=')
        {
            t->token = T_ASMINUS;
        }
        else
        {
            putback(c);
            t->token = T_MINUS;
        }
        break;
    case '*':
        if ((c = next()) == '=')
        {
            t->token = T_ASSTAR;
        }
        else
        {
            putback(c);
            t->token = T_STAR;
        }
        break;
    case '/':
        if ((c = next()) == '=')
        {
            t->token = T_ASSLASH;
        }
        else
        {
            putback(c);
            t->token = T_SLASH;
        }
        break;
    case '%':
        if ((c = next()) == '=')
        {
            t->token = T_ASMOD;
        }
        else
        {
            putback(c);
            t->token = T_MOD;
        }
        break;
    case ';':
        t->token = T_SEMI;
        break;
    case '{':
        t->token = T_LBRACE;
        break;
    case '}':
        t->token = T_RBRACE;
        break;
    case '(':
        t->token = T_LPAREN;
        break;
    case ')':
        t->token = T_RPAREN;
        break;
    case '[':
        t->token = T_LBRACKET;
        break;
    case ']':
        t->token = T_RBRACKET;
        break;
    case '~':
        t->token = T_INVERT;
        break;
    case '^':
        t->token = T_XOR;
        break;
    case ',':
        t->token = T_COMMA;
        break;
    case '.':
        t->token = T_DOT;
        break;
    case ':':
        t->token = T_COLON;
        break;
    case '?':
        t->token = T_QUESTION;
        break;
    case '=':
        if ((c = next()) == '=')
        {
            t->token = T_EQ;
        }
        else
        {
            putback(c);
            t->token = T_ASSIGN;
        }
        break;
    case '!':
        if ((c = next()) == '=')
        {
            t->token = T_NE;
        }
        else
        {
            putback(c);
            t->token = T_LOGNOT;
        }
        break;
    case '<':
        if ((c = next()) == '=')
        {
            t->token = T_LE;
        }
        else if (c == '<')
        {
            t->token = T_LSHIFT;
        }
        else
        {
            putback(c);
            t->token = T_LT;
        }
        break;
    case '>':
        if ((c = next()) == '=')
        {
            t->token = T_GE;
        }
        else if (c == '>')
        {
            t->token = T_RSHIFT;
        }
        else
        {
            putback(c);
            t->token = T_GT;
        }
        break;
    case '&':
        if ((c = next()) == '&')
        {
            t->token = T_LOGAND;
        }
        else
        {
            putback(c);
            t->token = T_AMPER;
        }
        break;
    case '|':
        if ((c = next()) == '|')
        {
            t->token = T_LOGOR;
        }
        else
        {
            putback(c);
            t->token = T_OR;
        }
        break;
    default:
        // The character isn't part of any recognised token, error
        fatalc("Unrecognised character", c);
    }
    return (1);
}