#ifndef __SERIALIZE_MEMORY_H__
#define __SERIALIZE_MEMORY_H__

#include <memory>

#ifndef DEF_PTR
#define DEF_PTR(val) typedef std::tr1::shared_ptr<val> val##Ptr;
#endif

#define  AUTOMEMORY_SUPPORT_CSTRING
#define  ERROR_WRITE_MEMORY 0xf1
#define  ERROR_READ_MEMORY 0xf2

#ifdef AUTOMEMORY_SUPPORT_CSTRING
#include <atlstr.h>
#endif

#include <string>

class CSerializeMemory
{
public:
    CSerializeMemory()
        :m_pBuff(NULL)
        ,m_dwSize(0)
        ,m_dwAllocSize(0)
        ,m_dwCurPos(0)
        ,m_bAutoDel(TRUE)
    {
    }

    CSerializeMemory(DWORD dwAllocSize)
    {
        m_bAutoDel = TRUE;
        m_dwAllocSize = dwAllocSize;
        m_dwCurPos = 0;
        try
        {
            m_pBuff = (BYTE *)malloc(dwAllocSize);
        }
        catch(...)
        {
            m_pBuff = NULL;
        }
        m_dwSize = 0;

    };
    ~CSerializeMemory()
    {
        if (m_bAutoDel && m_pBuff)
        {
            free(m_pBuff);
        }

    }
    //************************************
    // Method:    AllocMem
    // FullName:  CSerializeMemory::AllocMem 预分配内存
    // Access:    public 
    // Returns:   BOOL   是否分配成功
    // Description:
    // Parameter: DWORD dwNewSize 分配大小
    //************************************
    BOOL AllocMem(DWORD dwNewSize)
    {

        if (m_dwAllocSize < dwNewSize)
        {
            if (NULL == m_pBuff)
            {
                m_pBuff =  (BYTE *)malloc(dwNewSize);
            }
            else
            {
                m_pBuff = (BYTE *)realloc(m_pBuff,dwNewSize);
            }
            m_dwAllocSize = dwNewSize;
        }
        return m_pBuff!=NULL;
    }

    //************************************
    // Method:    Reset
    // FullName:  CSerializeMemory::Reset 内存重置
    // Access:    public 
    // Returns:   void
    // Description:
    // Parameter: BOOL bFreeMem
    //************************************
    void Reset(BOOL bFreeMem = FALSE)
    {
        if (bFreeMem)
        {
            if (m_bAutoDel && m_pBuff)
            {
                free(m_pBuff);
                m_pBuff = NULL;

            }
            m_dwAllocSize = 0;
        }
        m_dwCurPos = 0;
        m_dwSize = 0;
    }

    //************************************
    // Method:    IsEmpty
    // FullName:  CSerializeMemory::IsEmpty 是否为空
    // Access:    public 
    // Returns:   BOOL
    // Description:
    //************************************
    BOOL IsEmpty()
    {
        return m_pBuff == NULL;
    }

    //************************************
    // Method:    GetCurPos
    // FullName:  CSerializeMemory::GetCurPos 当前位置
    // Access:    public 
    // Returns:   DWORD
    // Description:
    //************************************
    DWORD GetCurPos()
    {
        return m_dwCurPos;
    }

    DWORD GetRemainingSize()
    {
        return m_dwSize - m_dwCurPos;
    }

    //************************************
    // Method:    SeekToPos
    // FullName:  CSerializeMemory::SeekToPos 定位到指定内存位置
    // Access:    public 
    // Returns:   BOOL
    // Description:
    // Parameter: DWORD dwCurPos
    //************************************
    BOOL SeekToPos(DWORD dwCurPos)
    {
        if (dwCurPos <= m_dwAllocSize)
        {
            m_dwCurPos = dwCurPos;
            return TRUE;
        }
        return FALSE;
    }
    //************************************
    // Method:    GetSize
    // FullName:  CSerializeMemory::GetSize 总大小
    // Access:    public 
    // Returns:   DWORD
    // Description:
    //************************************
    DWORD GetSize()
    {
        return m_dwSize;
    }

    //************************************
    // Method:    SeekToBegin
    // FullName:  CSerializeMemory::SeekToBegin 定位到内存开始
    // Access:    public 
    // Returns:   BOOL
    // Description:
    //************************************
    BOOL SeekToBegin()
    {
        m_dwCurPos = 0;
        return m_dwAllocSize !=0;
    }

    //************************************
    // Method:    SeekToEnd
    // FullName:  CSerializeMemory::SeekToEnd 定位到内存尾部
    // Access:    public 
    // Returns:   BOOL
    // Description:
    //************************************
    BOOL SeekToEnd()
    {
        m_dwCurPos = m_dwSize;
        return m_dwAllocSize !=0;
    }

    //************************************
    // Method:    Write
    // FullName:  CSerializeMemory::Write 将数据写入内存
    // Access:    public 
    // Returns:   BOOL
    // Description:
    // Parameter: void * pBuff
    // Parameter: DWORD dwSize
    //************************************
    BOOL Write(void *pBuff,DWORD dwSize)
    {
        if (dwSize == 0)
        {
            return TRUE;
        }
       if (!TestBuff(m_dwCurPos + dwSize))
       {
           return FALSE;
       }
        memcpy(m_pBuff + m_dwCurPos,pBuff,dwSize);
        m_dwCurPos += dwSize;
        m_dwSize = max(m_dwSize,m_dwCurPos);
        return TRUE;
    }

    //************************************
    // Method:    Read
    // FullName:  CSerializeMemory::Read 从内存读取数据
    // Access:    public 
    // Returns:   BOOL
    // Description:
    // Parameter: void * pBuff
    // Parameter: DWORD dwSize
    //************************************
    BOOL Read(void *pBuff,DWORD dwSize)
    {
        if (m_dwCurPos + dwSize > m_dwSize)
        {
            return FALSE;
        }
        memcpy(pBuff,m_pBuff + m_dwCurPos,dwSize);
        m_dwCurPos += dwSize;
        m_dwSize = max(m_dwSize,m_dwCurPos);
        return dwSize;
    }

    //************************************
    // Method:    CurPosBuff
    // FullName:  CSerializeMemory::CurPosBuff 当前位置内存数据指针
    // Access:    public 
    // Returns:   void *
    // Description:
    // Parameter: DWORD nOffset
    //************************************
    void *CurPosBuff(DWORD nOffset = 0)
    {
        if (nOffset == 0 && m_dwCurPos >= m_dwSize)
        {
            return NULL;
        }
        if (!AllocMem(m_dwCurPos + nOffset))
        {
            return NULL;
        }
        void * pBuff = m_pBuff + m_dwCurPos;
        m_dwCurPos += nOffset;
        m_dwSize = max(m_dwSize,m_dwCurPos);
        return pBuff;
    }

    //************************************
    // Method:    ZeroBuff
    // FullName:  CSerializeMemory::ZeroBuff 清空内存
    // Access:    public 
    // Returns:   void
    // Description:
    //************************************
    void ZeroBuff()
    {
        DWORD dwLen = m_bAutoDel?m_dwAllocSize:m_dwSize;
        if(m_pBuff && dwLen)
            memset(m_pBuff,0,dwLen);
    }

    template<class T>
    operator T *()
    {
        return reinterpret_cast<T *>(m_pBuff);
    }

    template<class T>
    CSerializeMemory& operator << (const T& Val)
    {
        if (!Write((void *)&Val,sizeof(Val)))
        {
            throw(ERROR_WRITE_MEMORY);
        }
        return *this;
    }

    template<class T>
    CSerializeMemory& operator >> (T& Val)
    {
        if (!Read((void *)&Val,sizeof(Val)))
        {
            throw(ERROR_READ_MEMORY);
        }
        return *this;
    }
//以下为了32位和64位兼容
    CSerializeMemory& operator << (const long& Val)
    {
        int nTemVal = (int)Val;
        if (!Write((void *)&nTemVal,sizeof(nTemVal)))
        {
            throw(ERROR_WRITE_MEMORY);
        }
        return *this;
    }

    CSerializeMemory& operator >> (long& Val)
    {
         int nTemVal = (int)Val;
        if (!Read((void *)&nTemVal,sizeof(nTemVal)))
        {
            throw(ERROR_READ_MEMORY);
        }
        Val = nTemVal;
        return *this;
    }

    CSerializeMemory& operator << (const unsigned long& Val)
    {
        unsigned int nTemVal = (unsigned int)Val;
        if (!Write((void *)&nTemVal,sizeof(nTemVal)))
        {
            throw(ERROR_WRITE_MEMORY);
        }
        return *this;
    }

    CSerializeMemory& operator >> (unsigned long& Val)
    {
        unsigned int nTemVal = (unsigned int)Val;
        if (!Read((void *)&nTemVal,sizeof(nTemVal)))
        {
            throw(ERROR_READ_MEMORY);
        }
        Val = nTemVal;
        return *this;
    }

    CSerializeMemory& operator << (const std::string& Val)
    {
        int nSize = Val.size();
        *this << (int)((nSize+1) * sizeof(char));
        if (!Write((void *)Val.c_str(),Val.size()))
        {
            throw(ERROR_WRITE_MEMORY);
        }
        const char chNull(0);

        if (!Write((void *)&chNull,1))
        {
            throw(ERROR_WRITE_MEMORY);
        }

        return *this;
    }

    CSerializeMemory& operator >> (std::string& Val)
    {
        int nSize(0);
        *this >> nSize;
        char* pBuff = (char *)CurPosBuff(nSize);
        if (nSize == 1)
        {
            Val.clear();
            return *this;
        }
        if (NULL == pBuff)
        {
            throw(ERROR_READ_MEMORY);
        }
        Val = pBuff;
        return *this;
    }
    
	CSerializeMemory& operator << (const std::wstring& Val)
    {
        int nSize = Val.size();
        *this << (int)((nSize+1) * sizeof(WCHAR));
        if (!Write((void *)Val.c_str(),Val.size()*sizeof(WCHAR)))
        {
            throw(ERROR_WRITE_MEMORY);
        }
        const WCHAR chNull(0);

        if (!Write((void *)&chNull,sizeof(WCHAR)))
        {
            throw(ERROR_WRITE_MEMORY);
        }

        return *this;
    }

    CSerializeMemory& operator >> (std::wstring& Val)
    {
        int nSize(0);
        *this >> nSize;
        WCHAR* pBuff = (WCHAR *)CurPosBuff(nSize);
        if (nSize == 1)
        {
            Val.clear();
            return *this;
        }
        if (NULL == pBuff)
        {
            throw(ERROR_READ_MEMORY);
        }
        Val = pBuff;
        return *this;
    }

#if AUTOMEMORY_SUPPORT_CSTRING
    CSerializeMemory& operator << (const CString& Val)
    {
        int nSize = Val.GetLength();
        *this << (int)((nSize+1) * sizeof(TCHAR));
        if (!Write((void *)Val.GetString(),Val.GetLength()*sizeof(TCHAR)))
        {
            throw(ERROR_WRITE_MEMORY);
        }
        const TCHAR chNull(0);

        if (!Write((void *)&chNull,sizeof(TCHAR)))
        {
            throw(ERROR_WRITE_MEMORY);
        }

        return *this;
    }

    CSerializeMemory& operator >> (CString& Val)
    {
        int nSize(0);
        *this >> nSize;
        TCHAR* pBuff = (TCHAR *)CurPosBuff(nSize);
        if (NULL == pBuff)
        {
            throw(ERROR_READ_MEMORY);
        }
        Val = pBuff;
        return *this;
    }
#endif

private:
    BOOL TestBuff(DWORD lNewSize)
    {
        if (lNewSize > m_dwAllocSize)
        {
            if (!m_bAutoDel)
            {
                return FALSE;
            }
            //ATLASSERT(0);
            if(!AllocMem(lNewSize *3 >>1))
            {
                return FALSE;
            }
        }
        return TRUE;
    }
    CSerializeMemory( const CSerializeMemory& ) {}
    const CSerializeMemory& operator = ( const CSerializeMemory& ) {}

private:
    BYTE *m_pBuff;
    DWORD m_dwSize;
    DWORD m_dwAllocSize;
    DWORD m_dwCurPos;
    BOOL m_bAutoDel;

};

#endif