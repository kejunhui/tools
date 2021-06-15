#pragma once
#include <mutex>

/** 使用方法:
* 1. 定义类为单例:
*     class ConnectionPool {
*         SINGLETON(ConnectionPool) // Here
*     public:
*
* 2. 实现无参构造函数，析构函数
* 3. 获取单例类的对象:
*     ConnectionPool *pool = CSingleton<ConnectionPool>::getInstance();*/

template <typename T>
class CSingleton {
public:
	CSingleton(const CSingleton &other);
	CSingleton<T>& operator=(const CSingleton &other);
	static T* getInstance() {
		if (instance == nullptr) {
			mutex.lock();
			if (instance == nullptr) {
				instance.reset(new T());
			}
			mutex.unlock();
		}
		return instance.get();
	}

private:
	static std::mutex mutex;
	static std::unique_ptr<T> instance;
};

////////////////////////////////////////////////////////////////////////////////
template <typename T> std::mutex CSingleton<T>::mutex;
template <typename T> std::unique_ptr<T> CSingleton<T>::instance;


#define SINGLETON(Class)                        \
public:                                         \
    Class();                                    \
    ~Class();                                   \
    Class(const Class &other);                  \
    Class& operator=(const Class &other);       \
    friend class CSingleton<Class>;             

