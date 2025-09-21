// include/atm/interfaces/ICashDispenser.hpp
#pragma once
#include "atm/Types.hpp"
#include "atm/Result.hpp"

namespace atm {

class ICashDispenser {
public:
  virtual ~ICashDispenser() = default;

  virtual bool canDispense(Amount amount) const = 0;
  virtual Result<NotePlan> planNotes(Amount amount) const = 0;

  // 払出前のロック/確保（在庫の二重確保防止）
  virtual Result<Unit> lockNotes(const NotePlan& plan) = 0;
  virtual Result<Unit> releaseNotes(const NotePlan& plan) = 0;

  // 実際の払出
  virtual Result<Unit> dispense(const NotePlan& plan) = 0;
};

} // namespace atm
