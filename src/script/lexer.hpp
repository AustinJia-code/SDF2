/**
 * @file lexer.hpp
 * @brief Tokenizer for the SDF2 scripting language.
 * 
 * Treats # as comment until end of line
 */

#pragma once

#include <string>
#include <stdexcept>
#include <cctype>


enum class TokType
{
    Number, ID,
    Plus, Minus, Star, Slash,
    LParen, RParen, LBrack, RBrack,
    Comma, Equals,
    End
};

struct Token
{
    TokType     type;
    std::string text;
    double      number = 0;
};

class Lexer
{
    const std::string& src;
    size_t             pos  = 0;
    int                line = 1;

    char peek () const
    {
        return pos < src.size ()
                ? src[pos]
                : '\0';
    }

    char get ()
    {
        char c = peek ();
        if (c == '\n') line++;
        pos++;
        return c;
    }

    void skip_whitespace ()
    {
        while (pos < src.size ())
        {
            if (peek () == '#')
                while (pos < src.size () && peek () != '\n')
                    pos++;
            else if (std::isspace ((unsigned char) peek ()))
                get ();
            else
                break;
        }
    }

public:
    Lexer (const std::string& src_)
        : src (src_) {}

    Token next ()
    {
        skip_whitespace ();
        if (pos >= src.size ())
            return {TokType::End};

        char c = peek ();

        // Number literal (integer or float)
        if (std::isdigit ((unsigned char) c) ||
            (c == '.' && pos + 1 < src.size () &&
             std::isdigit ((unsigned char) src[pos + 1])))
        {
            size_t start = pos;

            while (pos < src.size () &&
                   (std::isdigit ((unsigned char) peek ()) || peek () == '.'))
                pos++;

            std::string text = src.substr (start, pos - start);
            return
            {
                TokType::Number,
                text,
                std::stod (text)
            };
        }

        // Identifier
        if (std::isalpha ((unsigned char) c) || c == '_')
        {
            size_t start = pos;
            while (pos < src.size () &&
                   (std::isalnum ((unsigned char) peek ()) || peek () == '_'))
                pos++;

            return
            {
                TokType::ID,
                src.substr (start, pos - start)
            };
        }

        get ();

        switch (c)
        {
            case '+': return { TokType::Plus,   "+" };
            case '-': return { TokType::Minus,  "-" };
            case '*': return { TokType::Star,   "*" };
            case '/': return { TokType::Slash,  "/" };
            case '(': return { TokType::LParen, "(" };
            case ')': return { TokType::RParen, ")" };
            case '[': return { TokType::LBrack, "[" };
            case ']': return { TokType::RBrack, "]" };
            case ',': return { TokType::Comma,  "," };
            case '=': return { TokType::Equals, "=" };
        }

        throw std::runtime_error (
            "line " + std::to_string (line) +
            ": unexpected character '" + c + "'");
    }

    int current_line () const
    {
        return line;
    }
};