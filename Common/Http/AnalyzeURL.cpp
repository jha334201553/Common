#include "../defs.h"
#include "Config.h"
#include "AnalyzeURL.h"
#include <stdio.h>
#include "../StringFormat/StringFormat.h"

// WORD m_port;
// LPWSTR m_server_name; 
// LPWSTR m_object_name;
CURL::CURL(LPCSTR szurl)
{
    m_port = 80;
    m_scheme = INTERNET_SCHEME_HTTP;
    m_server_name = NULL; 
    m_object_name = NULL;
    SetUrlA(szurl);
}

CURL::CURL(LPCWSTR szurl)
{
    SetUrlW(szurl);
}

BOOL CURL::SetUrlA(LPCSTR szurl)
{
    BOOL retval = FALSE;
    if (szurl != NULL)
    {
        LPWSTR szwUrl = MulToWide(szurl);
        retval = SetUrlW(szwUrl);
        FreeString(szwUrl);
    }
    return retval;
}

BOOL CURL::SetUrlW(LPCWSTR szurl)
{
    m_port = 80;
    m_scheme = INTERNET_SCHEME_HTTP;
    m_server_name = NULL; 
    m_object_name = NULL;

    if (szurl != NULL)
    {
        DWORD dwurllen = wcslen(szurl);
        LPWSTR lpCopyUrl = (LPWSTR)AllocMemory((dwurllen+1)*sizeof(WCHAR));
        wcscpy(lpCopyUrl, szurl);

        ClenData();
        AnalyzeUrl(lpCopyUrl);

        FreeMemory(lpCopyUrl);
        return TRUE;
    }
    return FALSE;
}

void CURL::ClenData()
{
    if (m_server_name != NULL)
    {
        FreeMemory(m_server_name);
    }

    if (m_object_name != NULL)
    {
        FreeMemory(m_object_name);
    }
}

VOID CURL::AnalyzeUrl(LPWSTR szurl)
{
    LPWSTR enumpoint = szurl;
    while(*enumpoint != L'\0')
    {
        if (*enumpoint == L'\\')
        {
            *enumpoint = L'/';
        }
        enumpoint++;
    }

    if (memicmp(szurl, L"http", 8) == 0)
    {
        if ((szurl[4]|0x20) == L's')
        {
            m_port = 443;
            m_scheme = INTERNET_SCHEME_HTTPS;
        }
    }
    LPCWSTR hostname = wcsstr(szurl, L"//");
    if (hostname != NULL)
    {
        hostname += 2;
    }
    else
    {
        hostname = szurl;
    }

    DWORD dwHostNameLen = 0;
    LPCWSTR objname = wcschr(hostname, L'/');
    if (objname != NULL)
    {
        dwHostNameLen = objname-hostname;
    }
    else
    {
        dwHostNameLen = wcslen(hostname);
    }

    m_server_name = (LPWSTR)AllocMemory((dwHostNameLen+4)*sizeof(WCHAR));
    memcpy(m_server_name, hostname, dwHostNameLen*sizeof(WCHAR));

    // URL with port ? 
    LPWSTR objport = wcschr(m_server_name, L':');
    if (objport != NULL)
    {
        m_port = _wtol(objport+1);
        *objport = L'\0';
    }

    if (objname != NULL)
    {
        if (*objname != L'\0')
        {
            DWORD dwObjLen = wcslen(objname);
            m_object_name = (LPWSTR)AllocMemory((dwObjLen+1)*sizeof(WCHAR));
            memcpy(m_object_name, objname, dwObjLen*sizeof(WCHAR));
        }
    }
}

WORD CURL::GetPort()
{
    return m_port;
}

int CURL::GetScheme()
{
    return m_scheme;
}

LPCWSTR CURL::GetServerName()
{
    return m_server_name;
}

LPCWSTR CURL::GetObjectName()
{
    return m_object_name;
}

CURL::~CURL()
{
    ClenData();
}



//////////////////////////////////////////////////////////////////////////
//
// &��+��%���ո�?��#��= ��/ 
const char ChangeChar[] = {'&', '+', '%', ' ', '?', '#', '=', '/', ':', '\\', '.'};

// ����url 
BOOL WINAPI EncodeURLA(const char* szUrl, char* szEncodeUrl, DWORD dwbuflen)
{
    if (szUrl == NULL || szEncodeUrl == NULL || dwbuflen == 0)
    {
        return FALSE;
    }

    size_t ilen = strlen(szUrl);
    size_t allocbuf = 0;
    if (ilen > 2048)
    {
        allocbuf = ilen+4096;
    }
    else
    {
        allocbuf = ilen+ilen+ilen;
    }
    char* szCopyUrl = (char*)AllocMemory(allocbuf);
    if (szCopyUrl != NULL)
    {
        strcpy(szCopyUrl, szUrl);
        if (!IsTextUTF8(szCopyUrl, ilen))
        {
            ANSIToUTF8(szCopyUrl, allocbuf);
        }
        unsigned char* po = (unsigned char*)szCopyUrl;
        for (int i=0; i<dwbuflen-4;)
        {
            int k = 0;
            if (*po >= 0x80)
            {
                sprintf(szEncodeUrl+i, "%%%02X", *po);
                i += 3;
            }
            else 
            {
                for (k=0; k<sizeof(ChangeChar)/sizeof(ChangeChar[0]); k++)
                {
                    if (*po == ChangeChar[k])
                    {
                        sprintf(szEncodeUrl+i, "%%%02X", *po);
                        i += 3;
                        break;
                    }
                }
                if (k == sizeof(ChangeChar)/sizeof(ChangeChar[0]))
                {
                    szEncodeUrl[i] = *po;
                    i++;
                }
            }
            po++;
        }
    }
    FreeMemory(szCopyUrl);

    return TRUE;
}

BYTE GetByte(const char* szBuf)
{
    BYTE btval = 0;
    for (int i=0; i<2; i++)
    {
        if (szBuf[i] >= '0' && szBuf[i] <= '9')
        {
            btval = (btval<<4)|(szBuf[i]-'0');
        }
        else if ((szBuf[i]|0x20) >= 'a' && (szBuf[i]|0x20) <= 'f')
        {
            btval = (btval<<4)|((szBuf[i]|0x20)-'a'+10);
        }
    }
    return btval;
}

// ����url 
BOOL WINAPI DecodeURLA(const char* szUrl, char* szDecodeUrl, DWORD dwbuflen)
{
    if (szUrl == NULL || szDecodeUrl == NULL || dwbuflen == 0)
    {
        return FALSE;
    }

    size_t ilen = strlen(szUrl);
    char* szCopyUrl = (char*)AllocMemory(ilen+1024);

    char* pout = szCopyUrl;
    for (size_t i=0; i<ilen; i++)
    {
        if (szUrl[i] == '%')
        {
            *pout++ = (char)GetByte(szUrl+i+1);
            i += 2;
        }
        else
        {
            *pout++ = szUrl[i];
        }
    }

    if (IsTextUTF8(szCopyUrl, pout-szCopyUrl))
    {
        UTF8ToANSI(szCopyUrl, ilen+1024);
    }
    strncpy(szDecodeUrl, szCopyUrl, dwbuflen);
    FreeMemory(szCopyUrl);
    return TRUE;
}


