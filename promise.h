/**
 * @file      : promise.h
 * @author    : LittleKu<kklvzl@gmail.com>
 * @date      : 2025-03-06 23:57:18
 * @brief     :
 */
#ifndef PROMISE_H__
#define PROMISE_H__

#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace promise {

template <typename T>
concept IsTuple = requires { typename std::tuple_size<T>::type; };

template <typename F, typename Result, typename... Args>
auto invoke_next_with_args(F&& f, Result&& result, Args&&... args) {
  if constexpr (IsTuple<std::decay_t<Result>>) {
    return std::apply(
        [&](auto&&... rs) {
          return std::invoke(std::forward<F>(f),
                             std::forward<decltype(rs)>(rs)...,
                             std::forward<Args>(args)...);
        },
        std::forward<Result>(result));
  } else {
    return std::invoke(std::forward<F>(f), std::forward<Result>(result),
                       std::forward<Args>(args)...);
  }
}

template <typename T>
class Promise;

template <typename Ret, typename... Args>
class Promise<Ret(Args...)> {
 public:
  using FunctionType = std::function<Ret(Args...)>;

 private:
  struct State {
    FunctionType Func;
    std::tuple<Args...> Args;
  };

 public:
  explicit Promise(FunctionType func, Args... args)
      : state_(std::make_shared<State>(State{
            std::move(func), std::make_tuple(std::forward<Args>(args)...)})) {
    static_assert(std::is_invocable_v<FunctionType, Args...>,
                  "Function arguments don't match parameter types");
  }

  template <typename F, typename... NextArgs>
  auto then(F&& next, NextArgs&&... next_args) {
    using NextRet = decltype(std::apply(state_->Func, state_->Args));
    using NewRet = decltype(invoke_next_with_args(std::forward<F>(next),
                                                  std::declval<NextRet>(),
                                                  std::declval<NextArgs>()...));

    return Promise<NewRet()>(
        [state = state_, next = std::forward<F>(next),
         args_tuple =
             std::make_tuple(std::forward<NextArgs>(next_args)...)]() mutable {
          auto&& result = std::apply(state->Func, state->Args);
          return std::apply(
              [&](auto&&... args) {
                return invoke_next_with_args(
                    next, std::forward<decltype(result)>(result),
                    std::forward<decltype(args)>(args)...);
              },
              args_tuple);
        });
  }

  Ret execute() { return std::apply(state_->Func, state_->Args); }

 private:
  std::shared_ptr<State> state_{nullptr};
};

template <typename... Args>
class Promise<void(Args...)> {
 public:
  using FunctionType = std::function<void(Args...)>;

 private:
  struct State {
    FunctionType Func;
    std::tuple<Args...> Args;
  };

 public:
  explicit Promise(FunctionType func, Args... args)
      : state_(std::make_shared<State>(State{
            std::move(func), std::make_tuple(std::forward<Args>(args)...)})) {
    static_assert(std::is_invocable_v<FunctionType, Args...>,
                  "Function arguments don't match parameter types");
  }

  template <typename F, typename... NextArgs>
  auto then(F&& next, NextArgs&&... next_args) {
    using NewRet = decltype(std::invoke(next, std::declval<NextArgs>()...));
    return Promise<NewRet()>(
        [state = state_, next = std::forward<F>(next),
         args_tuple =
             std::make_tuple(std::forward<NextArgs>(next_args)...)]() mutable {
          std::apply(state->Func, state->Args);
          return std::apply(
              [&](auto&&... args) {
                return std::invoke(next, std::forward<decltype(args)>(args)...);
              },
              args_tuple);
        });
  }

  void execute() { std::apply(state_->Func, state_->Args); }

 private:
  std::shared_ptr<State> state_{nullptr};
};

template <typename F, typename... Args>
Promise(F&&, Args&&...) -> Promise<
    std::invoke_result_t<F, std::decay_t<Args>...>(std::decay_t<Args>...)>;

}  // namespace promise

#endif  // PROMISE_H__
