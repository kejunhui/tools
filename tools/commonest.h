#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "definition.h"
///////////////////////////////////////////////////////

extern void ASCIIToMD5(const char* strSrc, char* strDst);

extern int HexToDec(char* src);

extern int GetHexToDec(const char *src, int len);

extern int GetIndexOfSigns(unsigned char ch);

extern int GetModulePath(char* s_path);

// 字符串分割
extern int StringSplit(std::vector<std::string>& dst, const std::string& src, const std::string& separator);

// 去掉前后空格
extern std::string& StringTrim(std::string &str);

extern int Startup();

extern double NEFormat(const char* data);

extern std::string GetHostName2Ip(const std::string &name = "");

extern int SaveData(const std::string & filename, const unsigned char* byteArray, const int & nLen);

extern std::string FormatString(const char* format, ...);

extern int ParseUrl(std::string url, std::unordered_map<std::string, std::string> &map);

extern void ThriftLog(const char* log);

extern int64_t Atoi64(const char* str);

