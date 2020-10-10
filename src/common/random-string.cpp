#include "random-string.hpp"
#include <random>
#include <string>

namespace common {
   std::string randomString(int len) {
      static auto& chrs = "0123456789"
         "abcdefghijklmnopqrstuvwxyz"
         "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

      thread_local static std::mt19937 rg{std::random_device{}()};
      thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

      std::string s;

      s.reserve(len);

      while(len--)
         s += chrs[pick(rg)];

      return s;
      
   }
}