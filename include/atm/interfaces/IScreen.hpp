// include/atm/interfaces/IScreen.hpp
#pragma once
#include <string>
#include <vector>
#include "atm/Result.hpp"

namespace atm {

// 画面表示用インターフェース（入力はIKeypadで実施）
class IScreen {
public:
  virtual ~IScreen() = default;

  // 通常メッセージの表示
  virtual Result<Unit> showMessage(const std::string& msg) = 0;

  // エラーメッセージの表示
  virtual Result<Unit> showError(const std::string& msg) = 0;

  // メニューの表示（選択はIKeypadで読み取る想定）
  virtual Result<Unit> showMenu(const std::string& title,
                                const std::vector<std::string>& items) = 0;

  // 画面クリア
  virtual Result<Unit> clear() = 0;
};

} // namespace atm
