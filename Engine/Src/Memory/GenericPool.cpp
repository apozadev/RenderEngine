#include <memory>
#include "Memory/GenericPool.h"
#include "Core/Exception.h"

#define CHUNK_SIZE 8

  unsigned int calculateNeededChunks(size_t size)
  {
    return ((size_t)ceil((float)(((float)size) / ((float)CHUNK_SIZE))));
  }

  void GenericPool::Initialize(size_t size)
  {
    if (m_pData)
    {
      THROW_GENERIC_EXCEPTION("[MEMORY] Pool already initialized(realloc?) ")
      return;
    }

    m_totalSize = size;

    m_numChunks = calculateNeededChunks(m_totalSize);

    size_t actualSize = m_numChunks * (size_t)CHUNK_SIZE;

    m_pData = malloc(actualSize);

    m_pChunkUsedBytes = new size_t[m_numChunks]();
  }

  void* GenericPool::Allocate(size_t size, size_t alignment)
  {

    if (m_pData == nullptr)
    {
      THROW_GENERIC_EXCEPTION("[MEMORY] Pool not initilized")
    }

    int candidateIdx = -1;

    size_t offset = 0;

    if (alignment > CHUNK_SIZE)
    {
      size_t requiredSize = size + alignment;
      void* ptr = std::align(alignment, size, m_pData, requiredSize);
      offset = (char*)ptr - (char*)m_pData;
    }

    size_t totalSize = size + offset;

    unsigned int neededChunks = calculateNeededChunks(totalSize);

    unsigned int currIdx = m_cursorIdx;
    for (unsigned int chunksSkipped = 0; chunksSkipped < m_numChunks;)
    {

      if (currIdx >= m_numChunks)
      {
        currIdx = 0;
      }

      // Check following chunks are all free
      if (candidateIdx >= 0)
      {
        if (currIdx - candidateIdx >= neededChunks)
        {
          m_pChunkUsedBytes[candidateIdx] = totalSize;
          m_cursorIdx = currIdx;
          return (char*)m_pData + candidateIdx * CHUNK_SIZE + offset;
        }
        else if (m_pChunkUsedBytes[candidateIdx] > 0)
        {
          candidateIdx = -1;
        }
      }
      // Found candidate chunk
      else if (m_pChunkUsedBytes[currIdx] == 0 && (m_totalSize - currIdx * (size_t)CHUNK_SIZE) >= size)
      {
        candidateIdx = currIdx;
      }

      //skip chunks 
      unsigned int chunksToSkip = calculateNeededChunks(m_pChunkUsedBytes[currIdx]);
      if (chunksToSkip < 1) chunksToSkip = 1;
      currIdx += chunksToSkip;
      chunksSkipped += chunksToSkip;
    }

    THROW_GENERIC_EXCEPTION("[MEMORY] Pool out of free chunks")

    return nullptr;
  }

  bool GenericPool::Deallocate(void* ptr)
  {

    long long diff = ((char*)ptr - (char*)m_pData);

    if (diff % CHUNK_SIZE != 0)
    {
      THROW_GENERIC_EXCEPTION("[MEMORY] the provided pointer is not aligned with the chunk size. All pool objects start at the begining of a chunk.")
      return false;
    }

    long long chunkIdx = ((char*)ptr - (char*)m_pData) / CHUNK_SIZE;
    if (chunkIdx < 0 || chunkIdx > m_numChunks)
    {      
      return false;
    }
    m_pChunkUsedBytes[chunkIdx] = 0;
    return true;
  }

  void GenericPool::DeallocateAll()
  {
    if (m_pChunkUsedBytes)
    {
      memset(m_pChunkUsedBytes, 0, m_numChunks * sizeof(size_t));
      m_cursorIdx = 0;
    }
    else
    {
      // TODO: [WARNING] Nothing to deallocate
    }
  }
