#pragma once
#include <iostream>
#include <map>
#include <sqlite3.h>
#include <string>
#include <vector>

class Database {
public:
  static Database &instance() {
    static Database db;
    return db;
  }

  bool initialize(const std::string &db_path = "clover.db") {
    int rc = sqlite3_open(db_path.c_str(), &db_);
    if (rc) {
      std::cerr << "Can't open database: " << sqlite3_errmsg(db_) << std::endl;
      return false;
    }
    return true;
  }

  ~Database() {
    if (db_) {
      sqlite3_close(db_);
    }
  }

  bool execute(const std::string &sql) {
    char *errmsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
      std::cerr << "SQL error: " << errmsg << std::endl;
      sqlite3_free(errmsg);
      return false;
    }
    return true;
  }

  std::vector<std::map<std::string, std::string>>
  query(const std::string &sql) {
    std::vector<std::map<std::string, std::string>> results;

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
      std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_)
                << std::endl;
      return results;
    }

    int col_count = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
      std::map<std::string, std::string> row;
      for (int i = 0; i < col_count; i++) {
        const char *col_name = sqlite3_column_name(stmt, i);
        const char *col_value = (const char *)sqlite3_column_text(stmt, i);
        row[col_name] = col_value ? col_value : "";
      }
      results.push_back(row);
    }

    sqlite3_finalize(stmt);
    return results;
  }

  int64_t lastInsertRowId() { return sqlite3_last_insert_rowid(db_); }

private:
  Database() = default;
  sqlite3 *db_ = nullptr;
};
