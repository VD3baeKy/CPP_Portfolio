# CPP_Portfolio

C++ミニ課題（ATM開発想定）— 2〜3日で作れる実務寄りサンプル

 
## 目的

* ATMの中核である「状態管理」「デバイス抽象化」「バックエンド連携」「信頼性（失敗時の復元）」をC++で示す。
* 設計→実装→テスト→ドキュメントを一気通貫で提示。

 
## 開発条件

* 言語/ビルド: C++17（CMake）
* 依存ライブラリ: 任意（テストはCatch2 or GoogleTest推奨、JSONはnlohmann/json推奨）。外部導入が難しければ自作で可。
* 実行形態: コンソールアプリ（デバイスはソフトウェアで模擬）
* 品質: -Wall -Wextra -Wpedantic、RAII/スマートポインタ、例外安全（またはエラーコードの一貫性）、単体テスト10ケース以上

 
## 機能要件（Must）

### 認証
1. カード挿入→カードID読取→PIN入力（最大3回）。3回失敗でカード保持（Retain）。
2. セッションタイムアウト（無操作30秒でカード排出 or 保持ポリシーに従う。簡易で可）

### 取引
1. 残高照会
2. 現金引き出し
3. 1回の引き出しは1000円単位。日次上限（初期値: 50,000円、設定ファイルで変更可）
4. ATMの現金在庫（紙幣: 10,000円/1,000円）から最適な組合せで払出。組み合わせ不可なら「この金額は現在の在庫では払い出せません」
5. 口座残高・日次上限・在庫の3条件を満たした場合のみ実行

### ロギング/監査
1. 取引ID、時刻、カードID、操作、結果（成功/失敗理由）をファイルに追記
2. デバッグログ（INFO/ERROR）を別出力でも可

### バックエンド連携（模擬）
1. IBankApiインターフェース（残高照会、引き出し確定）を用意し、スタブ実装でJSONファイルから口座データを読み書き
2. 疑似ネットワーク障害（一定確率で失敗）とリトライ（最大3回、指数バックオフ）を実装
3. トランザクション整合性: 引き出しは2段階で疑似的に担保
4. reserve（残高・日次上限チェック＋予約）→ dispense（ATM在庫確保）→ commit（口座残高確定）
5. どこかで失敗したら必ずrollbackして元に戻す（在庫と口座の両方）

### 状態機械
1. Idle → CardInserted → PinEntry → Authenticated → MenuSelect（Balance/Withdraw）→ Processing → Dispensing → Ejecting → Idle
2. いつでもCancelで安全に中断（カード排出、予約/在庫はrollback）

 
## 機能要件（Should/拡張）

1. レシート出力（ファイル出力で可）
2. 管理者モード（パスコードで突入）で現金在庫の確認・補充
3. PIN試行3回失敗でカード保持フラグを口座側にも反映（次回は認証不可）

 
## 非機能要件

1. 設定ファイル（JSON）で以下を外だし
    -1. 日次上限、在庫の初期値、疑似ネットワーク障害率、タイムアウト秒
2. 設計の抽象化
    -1. ICardReader, IKeypad, IScreen, ICashDispenser, IReceiptPrinter, IBankApi の純粋仮想インターフェース
    -2. 具体実装はConsoleCardReaderなどのモックで代替

 
## テスト
1. ロジック層（キャッシュディスペンスの組み合わせ、日次上限、PIN回数制限、リトライとロールバック）を中心に単体テスト
2. 擬似乱数を固定化してネットワーク失敗を再現可能に

 
## 設計スケッチ（クラス/責務）

1. State(enum class): Idle, CardInserted, PinEntry, Authenticated, Menu, Processing, Dispensing, Ejecting
2. Services
    -1. IBankApi { getBalance(cardId), reserveWithdrawal(cardId, amount, txnId), commit(txnId), rollback(txnId) }
    -2. ICashDispenser { canDispense(amount), planNotes(amount), lockNotes(plan), releaseNotes(plan), dispense(plan) }
    -3. ICardReader { readCard(), retainCard(), ejectCard() }
    -4. IKeypad { readPIN(), readAmount(), confirm() }
    -5. IScreen { show(msg) }
    -6. IReceiptPrinter { print(Receipt) }
3. Domain
    -1. Money（額、通貨/単位）、NotePlan（1万円札xN, 千円札xM）
    -2. Transaction { id, cardId, type, amount, status, timestamps }
4. Utility
    -1. Logger、Config、Result<T,Error>（またはstd::optional/std::expected相当）、RetryPolicy

 
## 入出力・シナリオ（受け入れ基準）

* シナリオ1: 正常引き出し
在庫: 1万円札×5、千円札×10。残高: 120,000、日次使用: 0
入力: 20,000円引き出し
期待: 1万円札×2が払い出され残高100,000、日次使用20,000、レシート発行、ログINFO
 
* シナリオ2: 在庫は足りるが組み合わせ不可
在庫: 1万円札×0、千円札×15。入力: 25,000円
期待: 失敗（25,000は千円札のみでは不可）。メッセージ表示、ログERROR、口座と在庫は不変
 
* シナリオ3: 口座残高不足
期待: reserveで拒否、適切なエラー表示、カード排出
 
* シナリオ4: 日次上限超過
日次上限50,000。午前に30,000使用済み。入力: 30,000
期待: 拒否（上限超過）
 
* シナリオ5: ネットワーク失敗リトライ
IBankApiが最初の2回失敗→3回目成功
期待: 自動リトライで成功。重複コミットなし（冪等性）
 
* シナリオ6: Dispense直前に障害
reserve成功→在庫ロック成功→dispenseで失敗を注入
期待: rollback（在庫・口座ともに元に戻る）、カード排出
 
* シナリオ7: PIN3回誤り
期待: カード保持、監査ログに記録
 
* シナリオ8: Cancel動作
MenuやProcessing前段でCancel→安全にカード排出、予約やロックは解除
 
* シナリオ9: タイムアウト
無操作30秒でセッション終了、カード排出、予約/ロック解除
 
* シナリオ10: 残高照会
期待: 現残高表示、ログ記録、状態はAuthenticatedに留まる
リポジトリ構成例
 
### /src, /include, /tests, /docs
* README: 概要、ビルド方法、実行例、設計図、テスト実行、制約
* docs/design.md: 状態遷移図（テキスト/簡易図）、主要クラス、エラーハンドリング方針、今後の改善
* config/config.json, data/accounts.json, data/atm_cash.json
* CI（任意）: GitHub Actionsでビルド＋テスト

 
## 評価観点（面接向けに可視化）

* 設計: インターフェース分離、単一責務、状態機械の明確さ
* 信頼性: 失敗注入とロールバックの整合性
* コード品質: モダンC++、例外/エラーの一貫性、命名、テストの粒度
* チーム適性: README/設計書の明瞭さ、Issue/PRの分割例（小さく通す）

 
## 実装のヒント

* 現金組み合わせは貪欲法＋在庫制約（1万円優先→不足分を千円で補う、不可なら失敗）
* 疑似ネットワーク失敗は乱数で制御（シード固定でテスト再現）
* トランザクションIDは時刻＋カウンタやUUID
* Result<T,Error>でドメインエラー（残高不足、上限超過、在庫不足、ネットワーク失敗、タイムアウト）を分類
* 例外は想定外（I/O失敗など）に限定し、ドメインは戻り値で扱うのも可

 
## 進め方（2〜3日想定）

1. Day1: 設計（状態機械、インターフェース）→ スケルトン作成 → キャッシュ計算とIBankApiスタブ
2. Day2: ATMController実装 → ロギング/設定 → 正常系と主要異常系テスト
3. Day3: リトライ/ロールバック/タイムアウト → 仕上げ（README、設計図、デモ台本作成）


## デモ台本（5分）
* 状態図1分 → 正常引き出し（シナリオ1）1.5分 → ネットワーク失敗からの自動回復（シナリオ5）1.5分 → ロールバック（シナリオ6）1分





状態図1分 → 正常引き出し（シナリオ1）1.5分 → ネットワーク失敗からの自動回復（シナリオ5）1.5分 → ロールバック（シナリオ6）1分
必要なら、簡単なインターフェース雛形（ヘッダ）やCMakeの最小構成、テストケース雛形もお渡しできます。希望の依存（Catch2/GoogleTest、JSON、SQLiteなど）を教えてください。
