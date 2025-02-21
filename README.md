# Promise

- Class member function
```cpp
class Test {
 public:
  int first() {
    std::cout << "first" << std::endl;
    return 1;
  }
  int second(int first) {
    std::cout << "first" << first << std::endl;
    return 2;
  }
  std::string third(int second, int param) {
    std::cout << "second" << second << " param" << param << std::endl;
    return "end";
  }
  void end(const std::string& str) { std::cout << "end" << str << std::endl; }
};
```

- Lambda
```cpp
promise::Promise(
    [](int a, int b) {
      std::cout << a << " + " << b << " = " << a + b << std::endl;
      return a + b;
    },
    2, 3)
    .then(
        [](int sum, int multiplier) {
          std::cout << sum << " * " << multiplier << " = " << sum * multiplier
                    << std::endl;
          return sum * multiplier;
        },
        4)
    .execute();
```

- No return value
```cpp
  promise::Promise([](int x) { std::cout << "x:" << x << std::endl; }, 5)
      .then([] { std::cout << " done" << std::endl; })
      .execute();
```

- Return tuple
```cpp
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
```
