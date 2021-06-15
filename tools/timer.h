#pragma once
#include <vector>
#include <atomic>
#include <functional>
#include <boost/timer.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace boost::asio;

template<typename Duration = boost::posix_time::milliseconds,bool newThread = true>
class CTimer
{
public:
	CTimer() : m_timer(m_ios, Duration(0)), m_isSingleShot(false), m_newThread(newThread){}
	~CTimer()
	{
		stop();
	}

	void start(unsigned int duration)
	{
		if (m_ios.stopped())
		{
			return;
		}

		m_isActive = true;
		m_duration = duration;
		m_timer.expires_at(m_timer.expires_at() + Duration(m_duration));
		m_func = [this]
		{
			m_timer.async_wait([this](const boost::system::error_code&)
			{
				for (auto& func : m_funcVec)
				{
					func();
				}

				if (!m_isSingleShot)
				{
					m_timer.expires_at(m_timer.expires_at() + Duration(m_duration));
					m_func();
				}
			});
		};

		m_func();
		if (m_newThread)
		{
			m_thread = boost::thread([this] { m_ios.run(); });
		}
		else
		{
			m_ios.run();
		}		
	}

	void stop()
	{
		m_ios.stop();
		if (m_newThread && m_thread.joinable())
		{
			m_thread.join();
		}
		m_isActive = false;
	}

	void bind(const std::function<void()>& func)
	{
		m_funcVec.emplace_back(func);
	}

	void setSingleShot(bool isSingleShot)
	{
		m_isSingleShot = isSingleShot;
	}

	bool isSingleShot() const
	{
		return m_isSingleShot;
	}

	bool isActive() const
	{
		return m_isActive;
	}

private:
	std::atomic<bool> m_isSingleShot;
	std::atomic<bool> m_newThread;
	std::function<void()> m_func = nullptr;
	std::vector<std::function<void()>> m_funcVec;
	boost::asio::io_service m_ios;
	boost::asio::deadline_timer m_timer;
	boost::thread m_thread;
	unsigned int m_duration = 0;
	bool m_isActive = false;
};