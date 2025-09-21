// include/atm/interfaces/ICardReader.hpp
#pragma once
#include <string>
#include "atm/Result.hpp"

namespace atm {

class ICardReader {
public:
  virtual ~ICardReader() = default;

  virtual Result<std::string> readCardId() = 0;
  virtual Result<Unit> ejectCard() = 0;
  virtual Result<Unit> retainCard() = 0;
};

} // namespace atm
