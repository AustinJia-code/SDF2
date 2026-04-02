/**
 * @file parser.hpp
 * @brief Recursive-descent parser for the SDF2 scripting language.
 *
 * Grammar:
 *   program    -> assignment*
 *   assignment -> ID '=' expr
 *   expr       -> term   (('+' | '-') term)*
 *   term       -> unary  (('*' | '/') unary)*
 *   unary      -> '-' unary | primary
 *   primary    -> NUMBER
 *               | '[' (expr (',' expr)*)? ']'
 *               | ID '(' (expr (',' expr)*)? ')'
 *               | ID
 *               | '(' expr ')'
 */

#pragma once

#include "lexer.hpp"
#include <memory>
#include <vector>
#include <stdexcept>

/**
 * Node defs
 */
struct Node
{
    virtual ~Node () = default;
};
using  NodePtr     = std::unique_ptr<Node>;

struct NumberNode : Node
{
    double value;
};

struct IDNode : Node
{
    std::string name;
};

struct BinOpNode : Node
{
    char op;
    NodePtr left, right;
};

struct UnaryNode : Node
{
    char op;
    NodePtr operand;
};

struct CallNode : Node
{
    std::string          func;
    std::vector<NodePtr> args;
};

struct ListNode : Node
{
    std::vector<NodePtr> elements;
};

struct Assignment
{
    std::string name;
    NodePtr     value;
};

using Program = std::vector<Assignment>;

/**
 * Parser
 */
class Parser
{
    Lexer lex;
    Token cur;

    void advance ()
    {
        cur = lex.next ();
    }

    void expect (TokType t, const std::string& what)
    {
        if (cur.type != t)
            throw std::runtime_error (
                "expected " + what + ", got '" + cur.text + "'");

        advance ();
    }

    NodePtr expr ()
    {
        auto left = term ();

        while (cur.type == TokType::Plus || cur.type == TokType::Minus)
        {
            char op = cur.text[0];
            advance ();
            auto node = std::make_unique<BinOpNode> ();
            node->op = op;
            node->left = std::move (left);
            node->right = term ();
            left = std::move (node);
        }

        return left;
    }

    NodePtr term ()
    {
        auto left = unary ();
        while (cur.type == TokType::Star || cur.type == TokType::Slash)
        {
            char op = cur.text[0];
            advance ();
            auto node = std::make_unique<BinOpNode> ();
            node->op = op;
            node->left = std::move (left);
            node->right = unary ();
            left = std::move (node);
        }
        return left;
    }

    NodePtr unary ()
    {
        if (cur.type == TokType::Minus)
        {
            advance ();
            auto node = std::make_unique<UnaryNode> ();
            node->op = '-';
            node->operand = unary ();
            return node;
        }

        return primary ();
    }

    NodePtr primary ()
    {
        if (cur.type == TokType::Number)
        {
            auto node  = std::make_unique<NumberNode> ();
            node->value = cur.number;
            advance ();
            
            return node;
        }

        if (cur.type == TokType::LBrack)
        {
            advance ();
            auto node = std::make_unique<ListNode> ();

            if (cur.type != TokType::RBrack)
            {
                node->elements.push_back (expr ());
                while (cur.type == TokType::Comma)
                {
                    advance ();
                    node->elements.push_back (expr ());
                }
            }

            expect (TokType::RBrack, "]");
            return node;
        }

        if (cur.type == TokType::ID)
        {
            std::string name = cur.text;
            advance ();
            if (cur.type == TokType::LParen)
            {
                advance ();
                auto node  = std::make_unique<CallNode> ();
                node->func = name;
                if (cur.type != TokType::RParen)
                {
                    node->args.push_back (expr ());
                    while (cur.type == TokType::Comma)
                    {
                        advance ();
                        node->args.push_back (expr ());
                    }
                }
                expect (TokType::RParen, ")");
                return node;
            }
            auto node  = std::make_unique<IDNode> ();
            node->name = name;
            return node;
        }

        if (cur.type == TokType::LParen)
        {
            advance ();
            auto node = expr ();
            expect (TokType::RParen, ")");
            return node;
        }

        throw std::runtime_error ("unexpected token '" + cur.text + "'");
    }

public:
    Parser (const std::string& src)
        : lex (src)
    {
        advance ();
    }

    Program parse ()
    {
        Program prog;
        while (cur.type != TokType::End)
        {
            if (cur.type != TokType::ID)
                throw std::runtime_error
                    ("expected assignment, got '" + cur.text + "'");

            std::string name = cur.text;
            advance ();
            expect (TokType::Equals, "=");
            
            prog.push_back ({name, expr ()});
        }
        return prog;
    }
};