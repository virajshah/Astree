#include "FileUtil.h"

// one version by OS:
#ifdef _WIN32
#include <Windows.h>
using namespace std;
//////////////////////////////////////////////////////////////////////////////
string FileUtil::get_executable_path()
{
	 char szEXEPath[ MAX_PATH  ];
     DWORD nChars = GetModuleFileNameA( nullptr, szEXEPath, MAX_PATH  );
    return string(szEXEPath,nChars);
}
//////////////////////////////////////////////////////////////////////////////
vector<string> FileUtil::list(const string& sPathAndMask)
{
    vector<string> vsResult;
	  HANDLE hfind;
	  WIN32_FIND_DATAA wfd;

	  // List the folder content
	  hfind = FindFirstFileA(sPathAndMask.c_str(), &wfd );
	  if (hfind != INVALID_HANDLE_VALUE)
	  {
		  do
		  {
				  vsResult.push_back( wfd.cFileName);
		  }
		  while(FindNextFileA( hfind, &wfd));
		  FindClose(hfind);
	  }
	  
	return vsResult;
}
//////////////////////////////////////////////////////////////////////////////
#endif

#ifdef __unix__ //linux
#include <limits.h>
#include <unistd.h>
using namespace std;
//////////////////////////////////////////////////////////////////////////////
std::string FileUtil::get_executable_path()
{
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return string(result, (count > 0) ? count : 0);
}
//////////////////////////////////////////////////////////////////////////////
vector<string> FileUtil::list(const string& sPathAndMask)
{
    vector<string> vsResult;

//    for (const auto & entry : std::filesystem::directory_iterator(sPathAndMask))
//        vsResult.push_back(entry.path().string());

    return vsResult;
}
//////////////////////////////////////////////////////////////////////////////
#endif

//////////////////////////////////////////////////////////////////////////////
string FileUtil::get_path(string sFile)
{
    size_t iPos = sFile.find_last_of("\\");
    return sFile.substr(0, iPos);
}
//////////////////////////////////////////////////////////////////////////////
