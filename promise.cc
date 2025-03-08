#include <iostream>

#include "promise.h"

int main() {
  auto p = promise::Promise([] {
             std::cout << "returning a tuple" << std::endl;
             return std::make_tuple(1, 2);
           })
               .then(
                   [](int a, int b, int c) {
                     std::cout << "a + b + c = " << a + b + c << std::endl;
                     return a + b + c;
                   },
                   3);

  const auto result = p.execute();
  std::cout << "Result: " << result << std::endl;
  return 0;
}