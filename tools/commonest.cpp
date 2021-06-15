#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <regex>
#if defined(WIN32)
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <atlstr.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h> 
#include <execinfo.h>     /* for backtrace() */  
#include <pthread.h>
#include <signal.h>
#include <sys/resource.h>
#include <netdb.h>
#include <sys/stat.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <iconv.h>
#include <stdarg.h>
#endif
#include "md5.h"
#include "commonest.h"


void ASCIIToMD5(const char* strSrc, char* strDst)
{
	unsigned char result[32] = { 0 };
	md5_state_t state;
	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)strSrc, strlen(strSrc));
	md5_finish(&state, result);

	char buf[4];
	char key[34] = { 0 };
	for (int i = 0; i < 16; i++)
	{
		int size = i * 2;
		//sprintf_s(buf, "%02x", result[i]);
		snprintf(buf, sizeof(buf), "%02x", result[i]);
		memcpy(&key[size], buf, 2);
	}
	memcpy(strDst, key, 32);
}

int GetIndexOfSigns(unsigned char ch)
{
	if (ch >= '0' && ch <= '9')
	{
		return ch - '0';
	}
	if (ch >= 'A' && ch <= 'F')
	{
		return ch - 'A' + 10;
	}
	if (ch >= 'a' && ch <= 'f')
	{
		return ch - 'a' + 10;
	}
	return -1;
}


int GetHexToDec(const char *p,int len)
{
	if (len > 4 || len <= 0)
	{
		return 0;
	}
	char strDataLen[5] = { 0 };
	memcpy(strDataLen, p, len);
	return HexToDec(strDataLen);
}

int HexToDec(char* src)
{
	int sum = 0, t = 1, i, len = strlen(src);

	for (i = len - 1; i >= 0; i--)
	{
		int num = GetIndexOfSigns(*(src + i));
		if (num < 0)
		{
			sum = 0;
			break;
		}
		sum += t * num;
		t *= 16;
	}

	return sum;
}

int GetModulePath(char* s_path)
{
    #if defined(WIN32)
	CString path;
	GetModuleFileName(NULL, path.GetBufferSetLength(256), 256);
	path.ReleaseBuffer();
	int pos = path.ReverseFind('\\');
	path = path.Left(pos);
	USES_CONVERSION;
	strcpy(s_path, (LPSTR)(LPCTSTR)path);
    #else
	int cnt = readlink("/proc/self/exe", s_path, 256);
	if (cnt < 0 || cnt >= 1024)
	{
		return -1;
	}
	//最后一个'/' 后面是可执行程序名，去掉可执行程序的名字，只保留路径
	for (int i = cnt; i >= 0; --i)
	{
		if (s_path[i] == '/')
		{
			s_path[i + 1] = '\0';
			break;
		}
	}
	#endif
	return 0;
}

#if defined(WIN32)
int Startup()
{
	_setmaxstdio(5000);
	return 0;
}

#else

void dump()
{
	int j, nptrs;
	void *buffer[16];
	char **strings;

	nptrs = backtrace(buffer, 16);

	printf("backtrace() returned %d addresses\n", nptrs);

	strings = backtrace_symbols(buffer, nptrs);
	if (strings == NULL) {
		perror("backtrace_symbols");
		exit(EXIT_FAILURE);
	}

	for (j = 0; j < nptrs; j++)
		printf("  [%02d] %s\n", j, strings[j]);

	free(strings);
}

void signal_handler(int signo)
{    
	printf("\n=========>>>catch signal %d <<<=========\n", signo);

	printf("Dump stack start...\n");
	dump();
	printf("Dump stack end...\n\n");

	char buff[64] = { 0x00 };

	sprintf(buff, "cat /proc/%d/maps", getpid());

	system((const char*)buff);

	signal(signo, SIG_DFL); /* 恢复信号默认处理 */
	raise(signo);           /* 重新发送信号 */
}

int Startup()
{
	struct rlimit rlim;
	rlim.rlim_cur = 256 * 1024L * 1024L;  // 256mb
    rlim.rlim_cur = 60000;
    rlim.rlim_max = 60000;
    if(setrlimit(RLIMIT_NOFILE, &rlim))
    {
        printf("warnning: setrlimit RLIMIT_NOFILE fail\n");
        return -1;
    }

	if (setrlimit(RLIMIT_STACK, &rlim))
	{
		printf("warnning: setrlimit RLIMIT_STACK fail\n");
		return -1;
	}

 	signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
   	signal(SIGCHLD, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
	signal(SIGABRT, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGBUS, signal_handler);
	signal(SIGSEGV, signal_handler); 
	signal(SIGFPE, signal_handler);
	signal(SIGINT, signal_handler);
	int i=daemon(1, 1);
	if(i != 0)
	{
		printf("warnning: daemon fail\n");
		return -1;
	}

	return 0;
}
#endif


// std::vector 的push 很耗时，不建议高并发下使用该函数
int StringSplit(std::vector<std::string>& dst, const std::string& src, const std::string& separator) 
{
	if (src.empty() || separator.empty())
		return 0;

	int nCount = 0;
	std::string temp;
	size_t pos = 0, offset = 0;

	// 分割第1~n-1个
	while ((pos = src.find_first_of(separator, offset)) != std::string::npos)
	{
		temp = src.substr(offset, pos - offset);
		if (temp.length() > 0) {
			dst.push_back(temp);
			nCount++;
		}
		offset = pos + 1;
	}

	// 分割第n个
	temp = src.substr(offset, src.length() - offset);
	if (temp.length() > 0) {
		dst.push_back(temp);
		nCount++;
	}
	return nCount;
}

std::string& StringTrim(std::string &str)
{
	if (str.empty()) {
		return str;
	}
	str.erase(0, str.find_first_not_of(" "));
	str.erase(str.find_last_not_of(" ") + 1);
	return str;
}

std::string GetHostName2Ip(const std::string &name)
{
	char host_name[128] = { 0 }, host[128] = { 0 };
	if (name.empty())
	{
		gethostname(host_name, sizeof(host_name));//获取本地主机名称
		printf("host_name:%s\n", host_name);
	}
	else memcpy(host_name, name.c_str(), name.length());

	int ret = 0;
	struct addrinfo *ailist = NULL, *aip = NULL;
	struct sockaddr_in *saddr;
	if ((ret = getaddrinfo(host_name, NULL, NULL, &ailist)) != 0) {
		printf("error %d : %s\n", ret, gai_strerror(ret));
	}
	else for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		saddr = (struct sockaddr_in*)aip->ai_addr;
		inet_ntop(aip->ai_family, (void *)&saddr->sin_addr, host, 128);  //网络字节流 ——》IP字符串
		printf("inet_ntop ip: %s\n", host);
		if (aip->ai_family == AF_INET) break;
	}
	return host;
}

int SaveData(const std::string & filename, const unsigned char* byteArray, const int & nLen)
{
	FILE * fp = NULL;
	fp = fopen(filename.c_str(), "ab");
	if (fp == NULL)
	{
		return -1;
	}
	for (int i = 0; i < nLen; i++)
	{
		unsigned char c = byteArray[i];
		fwrite(&c, 1, 1, fp);
	}
	if (fflush(fp) != 0)
	{
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

std::string FormatString(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	char buf[1024] = { 0 };
	vsprintf(buf, format, ap);
	va_end(ap);        //  清空参数指针
	return buf;
}

double NEFormat(const char* data)
{
	double dLL = 0, dDD = 0, dMM = 0;
	std::string strTemp = data;
	int nPos = strTemp.find('.');
	if (nPos < 2) return 0;
	dDD = atof(strTemp.substr(0, nPos - 2).c_str());
	dMM = atof(strTemp.substr(nPos - 2).c_str());
	dLL = dDD + dMM / 60;
	return dLL;
}

int ParseUrl(std::string url, std::unordered_map<std::string, std::string> &map)
{
	//std::string pattern{ "([^?&=]+)=([^?&=]*)" }; // url
	//std::regex re(pattern);
	//std::smatch result;
	//while (std::regex_search(url, result, re))
	//{
	//	std::string key = result[1];
	//	std::string value = result[2];
	//	StringTrim(key);
	//	StringTrim(value);
	//	map[key] = value;
	//	url = result.suffix().str();
	//}
	size_t pos = url.find_first_of("?", 0);
	if (pos == std::string::npos)
	{
		return -1;
	}
	std::string temp = url.substr(pos + 1, url.size() - pos);
	std::vector<std::string> param_vec;
	StringSplit(param_vec, temp, "&");
	for (int i = 0; i < param_vec.size(); i++)
	{
		std::vector<std::string> key_value;
		if (StringSplit(key_value, param_vec[i], "=") == 2)
		{
			map[key_value[0]] = key_value[1];
		}
	}
	return 0;
}

void ThriftLog(const char* log)
{
	printf("[HB-DEBUG][%s:%d]%s\n", __FILE__, __LINE__, log);
}

int64_t Atoi64(const char* str)
{
#if defined(WIN32)
	return _atoi64(str);
#else
	return strtol(str, NULL, 10);
#endif
}





