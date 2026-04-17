#pragma once
#include <memory>
#include <tiny_orm/database.hpp>

namespace Config {
inline std::shared_ptr<TinyORM::Database> getDatabase() {
  static auto db = TinyORM::Database::create(TinyORM::DbType::SQLITE,
                                             "clover.db" // SQLite database file
  );
  return db;
}
} // namespace Config
