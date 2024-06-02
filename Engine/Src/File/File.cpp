#include <stdlib.h>

#include "File/File.h"
#include "File/FileUtils.h"

namespace file
{
  File::File(const char* _sFilename, bool _bRelativePath)
  {
    m_sFilename = std::string(_sFilename);
    m_pData = file::LoadFile(_sFilename, m_uSize, _bRelativePath);
  }

  File::~File()
  {
    free(m_pData);
  }
}