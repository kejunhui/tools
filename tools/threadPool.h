#pragma once
#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
using namespace std;

struct io_server
{
	io_server(): m_work(m_io) {}
	template<typename F, typename...Args>
	void post(F &&f, Args&&...args)
	{
		m_io.post(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	}
	boost::asio::io_service					m_io;
	boost::asio::io_service::work			m_work;
};

class ThreadPool 
{
public:
	explicit ThreadPool(int num) : m_count(num), m_stopped(false)
	{
		for (int i = 0 ; i < m_count; ++i) 
		{
			m_pServers.push_back(std::make_unique<io_server>());
		}
		for (int i = 0 ; i < m_count; ++i) 
		{
			m_threads.create_thread([this,i]() { m_pServers[i]->m_io.run();});
		}
	}
	~ThreadPool() 
	{
		stop();
	}

	io_server &dispatch(const int i)
	{ 
		return *(m_pServers[i & (m_count - 1)].get());
	}

	void stop() 
	{
		if (!m_stopped)
		{
			for (int i = 0; i<m_count; ++i)
			{
				m_pServers[i]->m_io.stop();
			}
			m_threads.join_all();
			m_stopped = true;
		}
	}
private:
	int										m_count;			// 线程池大小
	bool									m_stopped;
	boost::thread_group						m_threads;
	std::vector<std::unique_ptr<io_server>>	m_pServers;
};
