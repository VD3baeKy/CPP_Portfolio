// include/atm/Result.hpp
#pragma once
#include <utility>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <string>
#include "Types.hpp"

namespace atm {

// Unit相当
using Unit = std::monostate;

template <class T>
class Result {
public:
  static Result Ok(T value) {
    Result r;
    r.data_ = std::move(value);
    return r;
  }

  static Result Err(ErrorCode ec, std::string msg = {}) {
    Result r;
    r.data_ = Error{ec, std::move(msg)};
    return r;
  }

  bool has_value() const noexcept { return std::holds_alternative<T>(data_); }
  explicit operator bool() const noexcept { return has_value(); }

  const T& value() const {
    if (!has_value()) throw std::logic_error("Result has no value");
    return std::get<T>(data_);
  }

  T& value() {
    if (!has_value()) throw std::logic_error("Result has no value");
    return std::get<T>(data_);
  }

  ErrorCode error() const noexcept {
    if (has_value()) return ErrorCode::None;
    return std::get<Error>(data_).code;
  }

  const std::string& message() const {
    if (has_value()) {
      static const std::string empty;
      return empty;
    }
    return std::get<Error>(data_).message;
  }

private:
  struct Error {
    ErrorCode code{ErrorCode::Unknown};
    std::string message{};
  };

  std::variant<T, Error> data_;
};

} // namespace atm
