#pragma once
#include <thirdparty/spdlog/include/sinks/basic_file_sink.h>

/////////////////////////////////////////////////////////////////////////////
// Internals
BOOL IsBadReadPtrV2(void* ptr);
BOOL FileExists(const char* szPath);
MODULEINFO GetModuleInfo(const char* szModule);
DWORD64 FindPatternSIMD(const char* szModule, const unsigned char* szPattern, const char* szMask);

/////////////////////////////////////////////////////////////////////////////
// Utility
void DbgPrint(LPCSTR sFormat, ...);
void PrintLastError(void);
void HexDump(const char* szHeader, const char* szLogger, const void* pData, int nSize);

string CreateDirectories(string svFilePath);
string ConvertToWinPath(const string& svInput);
string ConvertToUnixPath(const string& svInput);

string Base64Encode(const string& svInput);
string Base64Decode(const string& svInput);

bool CompareStringAlphabetically(const string& svA, const string& svB);
bool CompareStringLexicographically(const string& svA, const string& svB);

bool StringReplace(string& svInput, const string& svFrom, const string& svTo);
string StringEscape(const string& svInput);
string StringUnescape(const string& svInput);

vector<int> StringToBytes(const string& svInput, bool bNullTerminator);
vector<int> PatternToBytes(const string& svInput);
vector<int> IntToDigits(int value);

void PrintM128i8(__m128i in);
void PrintM128i16(__m128i in);
void PrintM128i32(__m128i in);
void PrintM128i64(__m128i in);

string PrintPercentageEscape(const string& svInput);

/////////////////////////////////////////////////////////////////////////////
