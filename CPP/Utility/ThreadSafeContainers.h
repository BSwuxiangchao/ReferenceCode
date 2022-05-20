#ifndef THREADSAFECONTAINERS_H
#define THREADSAFECONTAINERS_H

#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <map>

/*
* 线程安全队列: 支持多个线程对队列中的数据进行处理
*
*/
template<typename T>
class ThreadSafeQueue
{
private:
	mutable std::mutex mtx;
	std::queue<T> data_queue;
	std::condition_variable data_cond;

public:
	ThreadSafeQueue() {}
	ThreadSafeQueue(ThreadSafeQueue const& other)
	{
		std::lock_guard<std::mutex> lock(other.mtx);
		data_queue = other.data_queue;
	}

	void push(T new_value)
	{
		std::lock_guard<std::mutex> lock(mtx);
		data_queue.push(new_value);
		data_cond.notify_one();
	}

	void wait_and_pop(T& value)
	{
		std::unique_lock<std::mutex> lock(mtx);
		data_cond.wait(lock, [this] {return !data_queue.empty(); });
		value = data_queue.front();
		data_queue.pop();
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lock(mtx);
		data_cond.wait(lock, [this] {return !data_queue.empty(); });
		std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
		data_queue.pop();
		return res;
	}

	bool try_pop(T& value)
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (data_queue.empty())
			return false;
		value = data_queue.front();
		data_queue.pop();
		return true;
	}

	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (data_queue.empty())
			return std::shared_ptr<T>();
		std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
		data_queue.pop();
		return res;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lock(mtx);
		return data_queue.empty();
	}

	std::size_t size()
	{
		std::lock_guard<std::mutex> lock(mtx);
		return data_queue.size();
	}

	void clear()
	{
		std::lock_guard<std::mutex> lock(mtx);
		std::queue<T> empty;
		std::swap(data_queue, empty);
	}
};


/*
* 线程安全Map: 允许多个线程操作Map中的数据. 不同于上面的先进先出队列，Map中的数据项通过一个唯一的key获取
* Map中的数据是按照从小到大的顺序排序的
*/
template<typename T>
class ThreadSafeMap
{
private:
	mutable std::mutex mtx;
	std::map<uint64_t, T> data_queue;
	std::condition_variable data_cond;

public:
	ThreadSafeMap() {}
	ThreadSafeMap(ThreadSafeMap const& other)
	{
		std::lock_guard<std::mutex> lock(other.mtx);
		data_queue = other.data_queue;
	}

	void push(T new_value, uint64_t key)
	{
		std::lock_guard<std::mutex> lock(mtx);
		data_queue.insert(std::pair<uint64_t, T>(key, new_value));
		data_cond.notify_one();
	}

	void wait_and_pop(T& value, uint64_t key)
	{
		std::unique_lock<std::mutex> lock(mtx);
		data_cond.wait(lock, [this, key] {return data_queue.count(key); });
		value = data_queue.at(key);
		data_queue.erase(key);
	}

	std::shared_ptr<T> wait_and_pop(uint64_t key)
	{
		std::unique_lock<std::mutex> lock(mtx);
		data_cond.wait(lock, [this, key] {return data_queue.count(key); });
		std::shared_ptr<T> res(std::make_shared<T>(data_queue.at(key)));
		data_queue.erase(key);
		return res;
	}

	bool try_pop(T& value, uint64_t key)
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (data_queue.empty())
			return false;
		value = data_queue.at(key);
		data_queue.erase(key);
		return true;
	}

	std::shared_ptr<T> try_pop(uint64_t key)
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (data_queue.empty())
			return std::shared_ptr<T>();
		std::shared_ptr<T> res(std::make_shared<T>(data_queue.at(key)));
		data_queue.erase(key);
		return res;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lock(mtx);
		return data_queue.empty();
	}

	std::size_t size()
	{
		std::lock_guard<std::mutex> lock(mtx);
		return data_queue.size();
	}

	void clear()
	{
		std::lock_guard<std::mutex> lock(mtx);
		data_queue.clear();
	}
};


#endif // THREADSAFECONTAINERS_H
