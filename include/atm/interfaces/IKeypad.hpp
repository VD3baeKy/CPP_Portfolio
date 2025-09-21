// include/atm/interfaces/IKeypad.hpp
#pragma once
#include <string>
#include "atm/Result.hpp"
#include "atm/Types.hpp"

namespace atm {

class IKeypad {
public:
  virtual ~IKeypad() = default;

  virtual Result<std::string> readPin() = 0;
  virtual Result<Amount> readAmount() = 0;
  virtual Result<bool> confirm(const std::string& message) = 0;
};

} // namespace atm
