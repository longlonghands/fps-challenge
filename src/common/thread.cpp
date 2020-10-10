
#include "thread.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <memory>
#include <sstream>

namespace common { namespace async {

   void sleep(int ms) {
   #ifdef _WIN32
         Sleep(ms);
   #else
         usleep(ms * 1000);
   #endif
   }
      
   Thread::Thread() : m_context(nullptr) {
      m_context = new Context();
   }

   Thread::~Thread() {
      try {
         join();  //??
         if (m_handle) {
            m_handle->detach();
            m_handle.reset();
         }
         if (m_context != nullptr) {
            m_context->OwnerIsAlive = false;
            if (!m_context->OwnerIsAlive && !m_context->threadIsAlive) {
               delete m_context;
               m_context = nullptr;
            }
         }
      } catch (std::exception &) {
         return;
      }
   }

   bool Thread::start(std::function<void()> target) {
      if (isRunning())
         return false;

      m_context->reset();
      m_context->target = target;
      return startAsync();
   }

   bool Thread::start(std::function<void(void *)> target, void *arg) {
      if (isRunning())
         return false;

      m_context->reset();
      m_context->target1 = target;
      m_context->arg = arg;
      return startAsync();
   }

   void Thread::join() {
      // assert(this->tid() != Thread::currentID());
      if (!isRunning())
         return;
      if (m_handle != nullptr && m_handle->joinable())
         m_handle->join();
      // assert(!this->running());
   }

   bool Thread::waitForExit(int timeout /*= 5000*/) {
      int times = 0;
      int interval = 10;
      while (this->isRunning()) {
         async::sleep(interval);
         times++;
         if (timeout && ((times * interval) > timeout)) {
            // assert(0 && "deadlock; calling inside thread scope?");
            return false;
         }
      }
      return true;
   }

   std::thread::id Thread::id() const {
      return m_handle ? m_handle->get_id() : std::thread::id();
   }

   std::thread::id Thread::currentID() {
      return std::this_thread::get_id();
   }

   bool Thread::isStarted() const {
      return m_context && m_context->started;
   }

   bool Thread::isRunning() const {
      return m_context && m_context->running;
   }

   bool Thread::startAsync() {
      m_handle = std::make_unique<std::thread>(
          [](void *arg) {
             auto ptr = reinterpret_cast<Context *>(arg);

             ptr->threadIsAlive = true;
             ptr->started = true;
             do {
                runAsync(ptr);
                // Platform::sleep(1);
             } while (0);
             ptr->started = false;
             ptr->running = false;
             ptr->threadIsAlive = false;
             // if (!ptr->OwnerIsAlive && !ptr->threadIsAlive)
             //	delete ptr;
          },
          m_context);

      return true;  // m_handle->joinable();
   }

   void Thread::runAsync(Context::ptr c) {
      c->running = true;
      try {
         if (c->tid.size() < 2) {
            std::stringstream strBuilder;
            strBuilder << std::this_thread::get_id();
            c->tid = strBuilder.str();
         }
         if (c->target) {
            // assert((!c->cancelled()));
            c->target();
         } else if (c->target1) {
            c->target1(c->arg);
         } else {
            throw std::runtime_error("Async callback has no target");
         }
      } catch (std::exception &) {
#if 0
				throw exc;
#endif
      }

      c->running = false;
   }

}}  // namespace tidy::async