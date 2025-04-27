#pragma once

#include <string>

namespace file
{
  class InFile
  {
  public:    
    InFile(const char* _sFilename, bool _bRelativePath = true);
    ~InFile();

    const std::string& GetFilename() const { return m_sFilename; }
    char* GetData() const { return m_pData; }
    size_t GetSize() const { return m_uSize; }

  private:
    std::string m_sFilename;
    char* m_pData;
    size_t m_uSize;
  };
}