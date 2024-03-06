#include <string>

namespace file
{
  char* LoadFile(const char* _sFilename, size_t& uSize_, bool _bRelativePath = true);

  const std::string& GetWorkingDirectory();
}