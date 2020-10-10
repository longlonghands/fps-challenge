#pragma once

#include <stdint.h>
#include <mutex>
#include <deque>
#include <condition_variable>
#include "config.hpp"

namespace common {
   template <typename T>
   class CircularBuffer {
    public:
      CircularBuffer(uint32_t cap) : m_capacity(cap) {
         if (m_capacity == 0)
            m_capacity = 1;
      }

      ~CircularBuffer() {
         clear();
      }

      void push_front(T& pkt) {
         std::unique_lock<std::mutex> lock(m_mutex);
         if (m_queue.size() >= m_capacity) {
            // instead of poping the old one we will wait
				//m_queue.pop_back();
				m_isFull.wait(lock);
         }
         m_queue.push_back(std::move(pkt));
      }

      T pop_back() {
         std::unique_lock<std::mutex> lock(m_mutex);
         if (m_queue.size() < 1)
            return T{};
			auto wasFull = m_queue.size() >= m_capacity;

         T obj = std::move(m_queue.front());

         m_queue.pop_front();
			if (wasFull) {
				m_isFull.notify_one();
			}
         return std::move(obj);
      }

      void clear() {
         std::unique_lock<std::mutex> lock(m_mutex);
         if (m_queue.empty())
            return;
         m_queue.clear();
      }

      bool isEmpty() const {
         std::unique_lock<std::mutex> lock(m_mutex);
         return m_queue.empty();
      }

      uint32_t Count() const {
         std::unique_lock<std::mutex> lock(m_mutex);
         return uint32_t(m_queue.size());
      }

      T mean() const {
			std::unique_lock<std::mutex> lock(m_mutex);
			T sum = 0;
			for (auto i:m_queue) {
				sum+=i;
			}
			return sum/m_capacity;
		}

    private:
      uint32_t m_capacity;
      std::deque<T> m_queue;
		std::condition_variable m_isFull;
      mutable std::mutex m_mutex;
   };

}  // namespace common
