#include "lexer.h"

Lexer::StringError Lexer::quoteHandler(std::string& input, std::string& read, size_t& position, char& prior)
{
    if (prior != ' ')
    {
        return StringError::MissingLeadingSeparator;
    }
    prior = input[position];

    while (position < input.size())
    {
        if (prior == '"')
        {
            if (read.size() < 3) // Reject empty string
                return StringError::EmptyString;

            int backslashes{0};

            for (size_t i = read.size() - 2; i > 0 && read[i] == '\\'; i--)
            {
                backslashes++;
            }

            if (backslashes % 2 == 0)
            {
                if (input[position] != ' ')  // Reject "String"n
                    return StringError::MissingTrailingSeparator;

                return StringError::None;
            }
        }
        read += input[position];
        prior = input[position];
        position++;
    }

    if (read.size() < 3) // Reject empty string
        return StringError::EmptyString;

    if (read.back() != '"') // Reject unclosed quoted string
        return StringError::UnterminatedString;

    int backslashes{0};

    if (read[read.size() - 2] != '"')
    {
        for (size_t i = read.size() - 2; i > 0 && read[i] == '\\'; i--)
        {
            backslashes++;
        }

        if (backslashes % 2 != 0) // Reject "Sample \"
        {
            return StringError::EscapedClosingQuote;
        }
    }
    return StringError::None;
}

Lexer::StringError Lexer::tokenHandler(std::string& input)
{
    std::string read{};
    size_t position{};
    char selected{};
    char prior{};

    while (position < input.size())
    {
        selected = input[position];
        read += selected;
        position++;

        if (selected == '"')
        {
            Lexer::StringError error_message = quoteHandler(input, read, position, prior);

            if (error_message != StringError::None)
                return error_message;
        }

        if (selected == ' ')
        {
            read.pop_back();
            tokens.push_back(read);
            read = "";
        }
        prior = selected;
    }
    tokens.push_back(read);

    return Lexer::StringError::None;
}

void Lexer::Reset()
{
    tokens.clear();
    alert = "";
}

std::vector<std::string> Lexer::Lex(std::string input)
{
    Reset();

    StringError error_message = tokenHandler(input);
    
    switch (error_message)
    {
        case StringError::None:
            alert = "None";
            return tokens;
        case StringError::MissingLeadingSeparator:
            alert = "Missing leading separator";
            break;
        case StringError::MissingTrailingSeparator:
            alert = "Missing trailing separator";
            break;
        case StringError::EmptyString:
            alert = "Empty String";
            break;
        case StringError::UnterminatedString:
            alert = "Unterminated string";
            break;
        case StringError::EscapedClosingQuote:
            alert = "Escaped closing quote";
            break;
    }
    return {};
}

std::string Lexer::getAlert()
{
    return alert;
}
