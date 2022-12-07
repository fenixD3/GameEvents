#include "FileExtractor.h"

FileExtractor::FileExtractor(std::string_view file_path)
    : m_File(file_path)
{
    using namespace std::string_literals;

    if (!m_File)
    {
        throw std::runtime_error("File "s + file_path.data() + " wasn't opened");
    }
}

std::optional<IExtractor::tokens_container> FileExtractor::GetNextCommandTokens()
{
    IExtractor::tokens_container result;
    std::string token;

    while (m_File >> token)
    {
        result.push_back(std::move(token));
        if (m_File.peek() == '\n')
        {
            break;
        }
    }

    return result.empty() ? std::nullopt : std::optional<IExtractor::tokens_container>(std::move(result));
}
