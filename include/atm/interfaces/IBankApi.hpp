// include/atm/interfaces/IBankApi.hpp
#pragma once
#include <string>
#include "atm/Types.hpp"
#include "atm/Result.hpp"

namespace atm {

class IBankApi {
public:
  virtual ~IBankApi() = default;

  // 口座残高取得
  virtual Result<Amount> getBalance(const std::string& cardId) = 0;

  // 引き出し予約（残高/日次上限チェック）。成功時は予約IDを返す
  virtual Result<std::string> reserveWithdrawal(const std::string& cardId, Amount amount) = 0;

  // 予約確定/ロールバック（冪等であることが望ましい）
  virtual Result<Unit> commit(const std::string& reservationId) = 0;
  virtual Result<Unit> rollback(const std::string& reservationId) = 0;

  // PIN検証（実運用ではHSM連携が多いがここでは簡易）
  virtual Result<Unit> verifyPin(const std::string& cardId, const std::string& pin) = 0;

  // PIN失敗回数が閾値を超えた場合にカードを無効化/保持フラグ反映など
  virtual Result<Unit> markCardRetained(const std::string& cardId) = 0;
};

} // namespace atm
