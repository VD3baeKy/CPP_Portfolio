// include/atm/Types.hpp
#pragma once
#include <cstdint>
#include <string>

namespace atm {

using Amount = std::int64_t; // JPY想定（1000円単位のバリデーションは上位で）

struct NotePlan {
  int n10000{0};
  int n1000{0};
  bool empty() const noexcept { return n10000 == 0 && n1000 == 0; }
};

enum class TxnType {
  Balance,
  Withdraw
};

enum class TxnStatus {
  Init,
  Reserved,
  Committed,
  RolledBack,
  Failed
};

enum class ErrorCode {
  None = 0,
  Unauthorized,
  PinRetryExceeded,
  InsufficientFunds,
  DailyLimitExceeded,
  CashShortage,
  CombinationNotPossible,
  InvalidAmount,
  NetworkError,
  Timeout,
  Cancelled,
  NotImplemented,
  Unknown
};

struct Transaction {
  std::string id;
  std::string cardId;
  TxnType type{TxnType::Withdraw};
  Amount amount{0};
  TxnStatus status{TxnStatus::Init};
};

} // namespace atm
