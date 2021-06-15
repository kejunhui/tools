#pragma once
#include "boost/thread/mutex.hpp"
#include "boost/thread/shared_mutex.hpp"
#include "boost/thread/condition.hpp"

//boost::mutex,boost::try_mutex,boost::timed_mutex,boost::recursive_mutex(µÝ¹é),boost::recursive_try_mutex,boost::recursive_timed_mutex
//boost::lock_guard; boost::shared_lock; boost::upgrade_lock; boost::unique_lock; boost::upgrade_to_unique_lock
//boost::mutex::scoped_lock ÇøÓòËø
//boost::unique_lock<boost::timed_mutex> lk( m, std::chrono::milliseconds(3) ); // ³¬Ê±3Ãë

typedef boost::mutex CMutex;
typedef boost::mutex::scoped_lock CMutexScoped;
typedef boost::unique_lock<boost::mutex> CMutexLocker;

typedef boost::shared_mutex CReadWriteLock;
typedef boost::shared_lock<boost::shared_mutex> CReadLocker;
typedef boost::unique_lock<boost::shared_mutex> CWriteLocker;

typedef boost::condition CCondition;

class CSemaphore {
public:
	explicit CSemaphore(int count = 0) : count_(count) {
	}

	void release() {
		boost::unique_lock<boost::mutex> lock(mutex_);
		++count_;
		cv_.notify_one();
	}

	void acquire() {
		boost::unique_lock<boost::mutex> lock(mutex_);
		cv_.wait(lock, [=] { return count_ > 0; });
		--count_;
	}

private:
	boost::mutex mutex_;
	boost::condition_variable cv_;
	int count_;
};

class spin_mutex  // ×ÔÐýËø
{
public:
	spin_mutex() = default;
	spin_mutex(const spin_mutex&) = delete;
	spin_mutex& operator= (const spin_mutex&) = delete;
	void lock() {
		while (flag.test_and_set(std::memory_order_acquire));
	}
	void unlock() {
		flag.clear(std::memory_order_release);
	}
private:
	std::atomic_flag flag = ATOMIC_FLAG_INIT;
};