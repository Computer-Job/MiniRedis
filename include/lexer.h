#pragma once

#include <string>
#include <vector>

class Lexer
{
private:
    enum class StringError
    {
        None,
        MissingLeadingSeparator,   // a"String"
        MissingTrailingSeparator,  // "String"n
        EmptyString,               // ""
        UnterminatedString,        // "String
        EscapedClosingQuote        // "Sample \"
    };

    std::vector<std::string> tokens{};
    std::string alert{};

    StringError quoteHandler(std::string& input, std::string& read, size_t& position, char& prior);
    StringError tokenHandler(std::string& input);
    void Reset();

public:
    std::vector<std::string> Lex(std::string text);
    std::string getAlert();
};
