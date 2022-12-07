#include "interfaces/IExtractor.h"

#include <fstream>
#include <string_view>

class FileExtractor : public IExtractor
{
private:
    std::fstream m_File;

public:
    FileExtractor(std::string_view file_path);

    std::optional<IExtractor::tokens_container> GetNextCommandTokens() override;
};
