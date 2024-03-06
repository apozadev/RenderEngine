namespace file
{
  class File
  {
  public:
    File(const char* _sFilename, bool _bRelativePath = true);
    ~File();

    char* GetData() { return m_pData; }
    size_t GetSize() { return m_uSize; }

  private:
    char* m_pData;
    size_t m_uSize;
  };
}