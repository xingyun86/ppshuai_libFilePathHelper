// FIlePathHelper.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FIlePathHelper.h"


// This is an example of an exported variable
//FILEPATHHELPER_API int nFIlePathHelper=0;

// This is an example of an exported function.
//FILEPATHHELPER_API int fnFIlePathHelper(void)
//{
//    return 42;
//}

// This is the constructor of a class that has been exported.
// see FIlePathHelper.h for the class definition
//CFIlePathHelper::CFIlePathHelper()
//{
//    return;
//}

#include <windows.h>
#include <shlobj.h>
#include <sddl.h>
#include <atlconv.h>

#include <tchar.h>
#include <stdio.h>
#include <string>

HRESULT
GetFolderDescriptionIdA(LPCSTR pszPath, SHDESCRIPTIONID *pdid)
{
	HRESULT hr;
	LPITEMIDLIST pidl;
	USES_CONVERSION;
	if (SUCCEEDED(hr = SHParseDisplayName(A2W(pszPath), NULL, &pidl, 0, NULL)))
	{
		IShellFolder *psf;
		LPCITEMIDLIST pidlChild;
		if (SUCCEEDED(hr = SHBindToParent(pidl, IID_IShellFolder, (void**)&psf, &pidlChild)))
		{
			hr = SHGetDataFromIDListW(psf, pidlChild, SHGDFIL_DESCRIPTIONID, pdid, sizeof(*pdid));
			psf->Release();
		}
		CoTaskMemFree(pidl);
	}
	return hr;
}
HRESULT
GetFolderDescriptionIdW(LPCWSTR pszPath, SHDESCRIPTIONID *pdid)
{
	HRESULT hr;
	LPITEMIDLIST pidl;
	if (SUCCEEDED(hr = SHParseDisplayName(pszPath, NULL, &pidl, 0, NULL)))
	{
		IShellFolder *psf;
		LPCITEMIDLIST pidlChild;
		if (SUCCEEDED(hr = SHBindToParent(pidl, IID_IShellFolder, (void**)&psf, &pidlChild)))
		{
			hr = SHGetDataFromIDListW(psf, pidlChild, SHGDFIL_DESCRIPTIONID, pdid, sizeof(*pdid));
			psf->Release();
		}
		CoTaskMemFree(pidl);
	}
	return hr;
}

std::string GetUserIDA()
{
	HANDLE hToken = NULL;
	LPBYTE pBuffer = NULL;
	LPSTR pszSID = NULL;
	DWORD dwBufferSize = 0;
	PTOKEN_USER pTokenUser = NULL;
	std::string strUserID = "";

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		if (GetTokenInformation(hToken, TokenUser, NULL, 0, &dwBufferSize) || (GetLastError() == ERROR_INSUFFICIENT_BUFFER))
		{
			pBuffer = new BYTE[dwBufferSize];

			if (pBuffer)
			{
				SecureZeroMemory(pBuffer, dwBufferSize * sizeof(BYTE));
				pTokenUser = reinterpret_cast<PTOKEN_USER>(pBuffer);

				if (GetTokenInformation(hToken, TokenUser, pTokenUser, dwBufferSize, &dwBufferSize) &&
					IsValidSid(pTokenUser->User.Sid) &&
					ConvertSidToStringSidA(pTokenUser->User.Sid, &pszSID))
				{
					strUserID = pszSID;
					if (pszSID)
					{
						LocalFree(pszSID);
						pszSID = NULL;
					}
				}

				delete[]pBuffer;
				pBuffer = NULL;
			}
		}

		if (hToken)
		{
			CloseHandle(hToken);
			hToken = NULL;
		}
	}

	return strUserID;
}
std::string GetRecycleBinPathA()
{
	HANDLE hFileFind = NULL;
	SHDESCRIPTIONID did = { 0 };
	WIN32_FIND_DATAA wfd = { 0 };
	CHAR RootPath[MAX_PATH] = { 0 };
	CHAR FindPath[MAX_PATH] = { 0 };
	CHAR FileName[MAX_PATH] = { 0 };
	std::string UserID = GetUserIDA();

	::GetModuleFileNameA(NULL, FileName, sizeof(FileName) / sizeof(*FileName));
	wsprintfA(RootPath, "%C:\\", *FileName);
	wsprintfA(FindPath, "%s*.*", RootPath);
	hFileFind = FindFirstFileA(FindPath, &wfd);
	if (hFileFind)
	{
		do
		{
			if ((wfd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY))
				== (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY) &&
				lstrcmpA(wfd.cFileName, ".") && lstrcmpA(wfd.cFileName, ".."))
			{
				if (SUCCEEDED(GetFolderDescriptionIdA(std::string(RootPath + std::string(wfd.cFileName) + "\\" + UserID).c_str(), &did)) &&
					did.clsid == CLSID_RecycleBin)
				{
					return std::string(RootPath + std::string(wfd.cFileName) + "\\" + UserID).c_str();
				}
			}
		} while (FindNextFileA(hFileFind, &wfd));
	}
	return ("");
}
std::wstring GetUserIDW()
{
	HANDLE hToken = NULL;
	LPBYTE pBuffer = NULL;
	LPTSTR pszSID = NULL;
	DWORD dwBufferSize = 0;
	PTOKEN_USER pTokenUser = NULL;
	std::wstring wstrUserID = L"";

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		if (GetTokenInformation(hToken, TokenUser, NULL, 0, &dwBufferSize) || (GetLastError() == ERROR_INSUFFICIENT_BUFFER))
		{
			pBuffer = new BYTE[dwBufferSize];

			if (pBuffer)
			{
				SecureZeroMemory(pBuffer, dwBufferSize * sizeof(BYTE));
				pTokenUser = reinterpret_cast<PTOKEN_USER>(pBuffer);

				if (GetTokenInformation(hToken, TokenUser, pTokenUser, dwBufferSize, &dwBufferSize) &&
					IsValidSid(pTokenUser->User.Sid) &&
					ConvertSidToStringSidW(pTokenUser->User.Sid, &pszSID))
				{
					wstrUserID = pszSID;
					if (pszSID)
					{
						LocalFree(pszSID);
						pszSID = NULL;
					}
				}

				delete[]pBuffer;
				pBuffer = NULL;
			}
		}

		if (hToken)
		{
			CloseHandle(hToken);
			hToken = NULL;
		}
	}

	return wstrUserID;
}
std::wstring GetRecycleBinPathW()
{
	HANDLE hFileFind = NULL;
	SHDESCRIPTIONID did = { 0 };
	WIN32_FIND_DATAW wfd = { 0 };
	WCHAR wRootPath[MAX_PATH] = { 0 };
	WCHAR wFindPath[MAX_PATH] = { 0 };
	WCHAR wFileName[MAX_PATH] = { 0 };
	std::wstring wUserID = GetUserIDW();

	::GetModuleFileNameW(NULL, wFileName, sizeof(wFileName) / sizeof(*wFileName));
	wsprintfW(wRootPath, L"%C:\\", *wFileName);
	wsprintfW(wFindPath, L"%s*.*", wRootPath);
	hFileFind = FindFirstFileW(wFindPath, &wfd);
	if (hFileFind)
	{
		do
		{
			if ((wfd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY))
				== (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY) &&
				lstrcmpW(wfd.cFileName, L".") && lstrcmpW(wfd.cFileName, L".."))
			{
				if (SUCCEEDED(GetFolderDescriptionIdW(std::wstring(wRootPath + std::wstring(wfd.cFileName) + L"\\" + wUserID).c_str(), &did)) &&
					did.clsid == CLSID_RecycleBin)
				{
					return std::wstring(wRootPath + std::wstring(wfd.cFileName) + L"\\" + wUserID).c_str();
				}
			}
		} while (FindNextFileW(hFileFind, &wfd));
	}
	return (L"");
}

bool GetSystemRecycleBinPathA(CHAR * wPath, int nSize)
{
	std::string a = GetRecycleBinPathA();
	if (a.length())
	{
		if (a.length() < nSize)
		{
			lstrcpyA(wPath, a.c_str());
			return true;
		}
	}

	return false;
}
bool GetSystemRecycleBinPathW(WCHAR * wPath, int nSize)
{
	std::wstring a = GetRecycleBinPathW();
	if (a.length())
	{
		if (a.length() < nSize)
		{
			lstrcpyW(wPath, a.c_str());
			return true;
		}
	}

	return false;
}