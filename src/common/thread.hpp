#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <thread>

namespace common { namespace async {

   void sleep(int ms);
   
   /// This class implements a platform-independent
   /// wrapper around an operating system thread.
   class Thread {
    public:
      typedef std::shared_ptr<Thread> Ptr;

      Thread();
      virtual ~Thread();

      // call the OS to start the thread
      bool start(std::function<void()> target);
      bool start(std::function<void(void *)> target, void *arg);

      // Waits until the thread exits.
      void join();

      // Waits until the thread exits.
      // The thread should be canceled before calling this method.
      // This method must be called from outside the current thread
      // context or deadlock will ensue.
      bool waitForExit(int timeout = 5000);

      // Returns the native thread ID.
      std::thread::id id() const;

      // Returns the native thread ID of the current thread.
      static std::thread::id currentID();

      bool isStarted() const;
      bool isRunning() const;

    protected:
      // The context which we send to the thread context.
      // This allows us to gracefully handle late callbacks
      // and avoid the need for deferred destruction of Runner objects.
      struct Context {
         typedef Context *ptr;

         bool threadIsAlive;
         bool OwnerIsAlive;

         // Thread-safe POD members
         // May be accessed at any time
         std::string tid;
         bool started;
         bool running;

         // Non thread-safe members
         // Should not be accessed once the Runner is started
         std::function<void()> target;
         std::function<void(void *)> target1;
         void *arg;

         // The implementation is responsible for resetting
         // the context if it is to be reused.
         void reset() {
            OwnerIsAlive = true;
            threadIsAlive = false;
            tid = "";
            arg = nullptr;
            target = nullptr;
            target1 = nullptr;
            started = false;
            running = false;
         }

         Context() {
            reset();
         }

         ~Context() {
            // printf("\ncontext deleting ...\n");
         }
      };

      bool startAsync();

      static void runAsync(Context::ptr context);

      Thread(const Thread &) = delete;
      Thread &operator=(const Thread &) = delete;

      Context::ptr m_context;
      std::unique_ptr<std::thread> m_handle;
   };
}}  // namespace tidy::async
