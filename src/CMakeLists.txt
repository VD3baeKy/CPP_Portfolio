# src/CMakeLists.txt
add_library(atm_core INTERFACE)

target_include_directories(atm_core
  INTERFACE
    ${PROJECT_SOURCE_DIR}/include
)

# ヘッダのみの段階でも、依存は先に紐付けておくと便利
target_link_libraries(atm_core
  INTERFACE
    nlohmann_json::nlohmann_json
    SQLite::SQLite3
)
