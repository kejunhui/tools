#pragma once

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#endif

//定义一个在日志后添加 文件名 函数名 行号 的宏定义
#ifndef suffix
#define suffix(msg)  std::string(msg).append("  <")\
        .append(__FILENAME__).append("> <").append(__func__)\
        .append("> <").append(std::to_string(__LINE__))\
        .append(">").c_str()
#endif

//在  spdlog.h   之前定义，才有效
#ifndef SPDLOG_TRACE_ON
#define SPDLOG_TRACE_ON
#endif

#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif

#include "commonest.h"
#include "configuration.h"
#include "spdlog/spdlog.h"
#include "spdlog/logger.h"
#include "spdlog/async.h" 
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

class Logger
{

public:
	static Logger& GetInstance() {
		static Logger m_instance;
		return m_instance;
	}

	auto GetLogger() { return nml_logger; }

private:
	Logger() {
		std::vector<spdlog::sink_ptr> sinkList;
//#ifdef _DEBUG
		int logLevel = ConfFile::GetConfigInfo().nLogLevel;
		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		consoleSink->set_level(spdlog::level::level_enum(logLevel));
		consoleSink->set_pattern("[%m-%d %T.%e][thread %t][%^%L%$]  %v");
		sinkList.push_back(consoleSink);
//#endif
		char path[512] = { 0 }, logPath[512] = { 0 };
		GetModulePath(path);
		sprintf(logPath, "%s/logs/decoder.txt", path);
		auto daily_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logPath,1048576*10,5);
		daily_sink->set_level(spdlog::level::trace);
		daily_sink->set_pattern("[%Y-%m-%d %T.%e][thread %t][%^%4l%$]  %v");
		sinkList.push_back(daily_sink);
		spdlog::init_thread_pool(4096, 1);
		nml_logger = std::make_shared<spdlog::async_logger>("async_logger", begin(sinkList), end(sinkList), spdlog::thread_pool(), spdlog::async_overflow_policy::block);

		//register it if you need to access it globally
		spdlog::register_logger(nml_logger);

		// 设置日志记录级别		
		nml_logger->set_level(spdlog::level::level_enum(logLevel));
		//设置当出发 err 或更严重的错误时立刻刷新日志到  disk 
		nml_logger->flush_on(spdlog::level::err);

		spdlog::flush_every(std::chrono::seconds(10));
	}

	~Logger() {
		spdlog::drop_all();
	}

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

private:
	std::shared_ptr<spdlog::async_logger> nml_logger;
};

//#define DEBUG Logger::GetInstance() 
#define LTRACE(msg,...) Logger::GetInstance().GetLogger()->trace(suffix(msg),__VA_ARGS__)
#define LDEBUG(...) Logger::GetInstance().GetLogger()->debug(__VA_ARGS__)
#define LINFO(...) Logger::GetInstance().GetLogger()->info(__VA_ARGS__)
#define LWARN(...) Logger::GetInstance().GetLogger()->warn(__VA_ARGS__)
#define LERROR(...) Logger::GetInstance().GetLogger()->error(__VA_ARGS__)
#define LCRITICAL(...) Logger::GetInstance().GetLogger()->critical(__VA_ARGS__)

#define criticalif(b, ...)                     \
    do {                                       \
        if ((b)) {                             \
           Logger::GetInstance().GetLogger()->critical(__VA_ARGS__); \
        }                                      \
    } while (0)

#ifdef WIN32  
#define errcode WSAGetLastError()
#endif
