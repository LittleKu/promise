#include <iostream>

#include "promise.h"

int main() {
  promise::Promise([] {
    std::cout << "returning a tuple" << std::endl;
    return std::make_tuple(1, 2);
  })
      .then(
          [](int a, int b, int c) {
            std::cout << "a + b + c = " << a + b + c << std::endl;
            return a + b + c;
          },
          3)
      .execute();
  return 0;
}