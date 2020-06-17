#include "legacydb-test-contract.hpp"

using namespace std;
using namespace eosio;
using namespace eosio::internal_use_do_not_use;
using namespace legacydb;

const std::vector<char> data_0{ 9, 7, 6 };
const std::vector<char> data_1{ 1, 2, 3, 4, 0, 4, 3, 2, 1 };
const std::vector<char> data_2{ 3, 4, 3, 4 };
const std::vector<char> data_3{};
const std::vector<char> data_4{ 4, 5, 6, 7 };

void store_i64(name scope, name table, name payer, uint64_t id, const std::vector<char>& data) {
   db_store_i64(scope.value, table.value, payer.value, id, data.data(), data.size());
}

int find_i64(name code, name scope, name table, uint64_t id) {
   return db_find_i64(code.value, scope.value, table.value, id);
}

int lowerbound_i64(name code, name scope, name table, uint64_t id) {
   return db_lowerbound_i64(code.value, scope.value, table.value, id);
}

int upperbound_i64(name code, name scope, name table, uint64_t id) {
   return db_upperbound_i64(code.value, scope.value, table.value, id);
}

int end_i64(name code, name scope, name table) { return db_end_i64(code.value, scope.value, table.value); }

#define WRAP_SECONDARY(NAME, TYPE)                                                                                     \
   struct NAME {                                                                                                       \
      using cdt_wrapper = eosio::_multi_index_detail::secondary_index_db_functions<TYPE>;                              \
      static int32_t store(name scope, name table, name payer, uint64_t id, const TYPE& secondary) {                   \
         return cdt_wrapper::db_idx_store(scope.value, table.value, payer.value, id, secondary);                       \
      }                                                                                                                \
      static void update(int32_t iterator, name payer, const TYPE& secondary) {                                        \
         return cdt_wrapper::db_idx_update(iterator, payer.value, secondary);                                          \
      }                                                                                                                \
      static void    remove(int32_t iterator) { return cdt_wrapper::db_idx_remove(iterator); }                         \
      static int32_t find_secondary(name code, name scope, name table, const TYPE& secondary, uint64_t& primary) {     \
         return cdt_wrapper::db_idx_find_secondary(code.value, scope.value, table.value, secondary, primary);          \
      }                                                                                                                \
      static int32_t find_primary(name code, name scope, name table, TYPE& secondary, uint64_t primary) {              \
         return cdt_wrapper::db_idx_find_primary(code.value, scope.value, table.value, primary, secondary);            \
      }                                                                                                                \
      static int32_t lowerbound(name code, name scope, name table, TYPE& secondary, uint64_t& primary) {               \
         return cdt_wrapper::db_idx_lowerbound(code.value, scope.value, table.value, secondary, primary);              \
      }                                                                                                                \
      static int32_t upperbound(name code, name scope, name table, TYPE& secondary, uint64_t& primary) {               \
         return cdt_wrapper::db_idx_upperbound(code.value, scope.value, table.value, secondary, primary);              \
      }                                                                                                                \
      static int32_t end(name code, name scope, name table) {                                                          \
         return cdt_wrapper::db_idx_end(code.value, scope.value, table.value);                                         \
      }                                                                                                                \
      static int32_t next(int32_t iterator, uint64_t& primary) {                                                       \
         return cdt_wrapper::db_idx_next(iterator, &primary);                                                          \
      }                                                                                                                \
      static int32_t previous(int32_t iterator, uint64_t& primary) {                                                   \
         return cdt_wrapper::db_idx_previous(iterator, &primary);                                                      \
      }                                                                                                                \
      static void check_find_secondary(name code, name scope, name table, int expected_itr, uint64_t expected_primary, \
                                       const TYPE& secondary_search) {                                                 \
         uint64_t primary   = 0xfeedbeef;                                                                              \
         auto     secondary = secondary_search;                                                                        \
         auto     itr       = find_secondary(code, scope, table, secondary, primary);                                  \
         eosio::check(itr == expected_itr && primary == expected_primary,                                              \
                      "check_find failure: code: " + code.to_string() + " scope: " + scope.to_string() + " table: " +  \
                            table.to_string() + " secondary_search: " + convert_to_json(secondary_search) +            \
                            " itr: " + convert_to_json(itr) + " expected_itr: " + convert_to_json(expected_itr) +      \
                            " secondary: " + convert_to_json(secondary) + " primary: " + convert_to_json(primary) +    \
                            " expected_primary: " + convert_to_json(expected_primary) + "\n");                         \
      }                                                                                                                \
      static void check_next(int32_t iterator, int expected_itr, uint64_t expected_primary) {                          \
         uint64_t primary = 0xfeedbeef;                                                                                \
         auto     itr     = next(iterator, primary);                                                                   \
         eosio::check(itr == expected_itr && primary == expected_primary,                                              \
                      "check_next failure: itr: " + convert_to_json(itr) + " expected_itr: " +                         \
                            convert_to_json(expected_itr) + " primary: " + convert_to_json(primary) +                  \
                            " expected_primary: " + convert_to_json(expected_primary) + "\n");                         \
      }                                                                                                                \
      static void check_prev(int32_t iterator, int expected_itr, uint64_t expected_primary) {                          \
         uint64_t primary = 0xfeedbeef;                                                                                \
         auto     itr     = previous(iterator, primary);                                                               \
         eosio::check(itr == expected_itr && primary == expected_primary,                                              \
                      "check_prev failure: itr: " + convert_to_json(itr) + " expected_itr: " +                         \
                            convert_to_json(expected_itr) + " primary: " + convert_to_json(primary) +                  \
                            " expected_primary: " + convert_to_json(expected_primary) + "\n");                         \
      }                                                                                                                \
      static void check_lowerbound(name code, name scope, name table, int expected_itr, uint64_t expected_primary,     \
                                   const TYPE& secondary_search, const TYPE& expected_secondary) {                     \
         uint64_t primary   = 0xfeedbeef;                                                                              \
         auto     secondary = secondary_search;                                                                        \
         auto     itr       = lowerbound(code, scope, table, secondary, primary);                                      \
         eosio::check(itr == expected_itr && primary == expected_primary && secondary == expected_secondary,           \
                      "check_lowerbound failure: code: " + code.to_string() + " scope: " + scope.to_string() +         \
                            " table: " + table.to_string() +                                                           \
                            " secondary_search: " + convert_to_json(secondary_search) +                                \
                            " itr: " + convert_to_json(itr) + " expected_itr: " + convert_to_json(expected_itr) +      \
                            " secondary: " + convert_to_json(secondary) + " expected_secondary: " +                    \
                            convert_to_json(expected_secondary) + " primary: " + convert_to_json(primary) +            \
                            " expected_primary: " + convert_to_json(expected_primary) + "\n");                         \
      }                                                                                                                \
      static void check_upperbound(name code, name scope, name table, int expected_itr, uint64_t expected_primary,     \
                                   const TYPE& secondary_search, const TYPE& expected_secondary) {                     \
         uint64_t primary   = 0xfeedbeef;                                                                              \
         auto     secondary = secondary_search;                                                                        \
         auto     itr       = upperbound(code, scope, table, secondary, primary);                                      \
         eosio::check(itr == expected_itr && primary == expected_primary && secondary == expected_secondary,           \
                      "check_upperbound failure: code: " + code.to_string() + " scope: " + scope.to_string() +         \
                            " table: " + table.to_string() +                                                           \
                            " secondary_search: " + convert_to_json(secondary_search) +                                \
                            " itr: " + convert_to_json(itr) + " expected_itr: " + convert_to_json(expected_itr) +      \
                            " secondary: " + convert_to_json(secondary) + " expected_secondary: " +                    \
                            convert_to_json(expected_secondary) + " primary: " + convert_to_json(primary) +            \
                            " expected_primary: " + convert_to_json(expected_primary) + "\n");                         \
      }                                                                                                                \
   };

WRAP_SECONDARY(idx64, uint64_t);
WRAP_SECONDARY(idx128, uint128_t);
WRAP_SECONDARY(idx_double, double);
WRAP_SECONDARY(idx256, checksum256);

std::vector<char> get_i64(int itr) {
   std::vector<char> result(db_get_i64(itr, nullptr, 0));
   db_get_i64(itr, result.data(), result.size());
   return result;
}

void check_lowerbound_i64(name code, name scope, name table, uint64_t id, int expected_itr,
                          const std::vector<char>& expected_data) {
   auto itr = lowerbound_i64(code, scope, table, id);
   eosio::check(itr == expected_itr, "check_lowerbound_i64 failure: wrong iterator. code: " + code.to_string() +
                                           " scope: " + scope.to_string() + " table: " + table.to_string() +
                                           " id: " + to_string(id) + " itr: " + to_string(itr) +
                                           " expected_itr: " + to_string(expected_itr) + "\n");
   if (expected_itr >= 0)
      eosio::check(get_i64(expected_itr) == expected_data, "check_lowerbound_i64 failure: wrong data");
}

void check_upperbound_i64(name code, name scope, name table, uint64_t id, int expected_itr,
                          const std::vector<char>& expected_data) {
   auto itr = upperbound_i64(code, scope, table, id);
   eosio::check(itr == expected_itr, "check_upperbound_i64 failure: wrong iterator. code: " + code.to_string() +
                                           " scope: " + scope.to_string() + " table: " + table.to_string() +
                                           " id: " + to_string(id) + " itr: " + to_string(itr) +
                                           " expected_itr: " + to_string(expected_itr) + "\n");
   if (expected_itr >= 0)
      eosio::check(get_i64(expected_itr) == expected_data, "check_upperbound_i64 failure: wrong data");
}

void check_find_i64(name code, name scope, name table, uint64_t id, int expected_itr,
                    const std::vector<char>& expected_data) {
   auto itr = find_i64(code, scope, table, id);
   eosio::check(itr == expected_itr, "check_find_i64 failure: wrong iterator. code: " + code.to_string() +
                                           " scope: " + scope.to_string() + " table: " + table.to_string() +
                                           " id: " + to_string(id) + " itr: " + to_string(itr) +
                                           " expected_itr: " + to_string(expected_itr) + "\n");
   if (expected_itr >= 0)
      eosio::check(get_i64(expected_itr) == expected_data, "check_find_i64 failure: wrong data");
}

void check_end_i64(name code, name scope, name table, int expected_itr) {
   auto itr = end_i64(code, scope, table);
   eosio::check(itr == expected_itr, "check_end_i64 failure: wrong iterator. code: " + code.to_string() +
                                           " scope: " + scope.to_string() + " table: " + table.to_string() + " itr: " +
                                           to_string(itr) + " expected_itr: " + to_string(expected_itr) + "\n");
}

void check_next_i64(int itr, int expected_itr, uint64_t expected_id) {
   uint64_t id       = 0xfeedbeef;
   auto     next_itr = db_next_i64(itr, &id);
   eosio::check(next_itr == expected_itr, "check_next_i64 failure: wrong iterator. itr: " + to_string(itr) + " next: " +
                                                to_string(next_itr) + " expected: " + to_string(expected_itr) + "\n");
   eosio::check(id == expected_id, "check_next_i64 failure: wrong id. itr: " + to_string(itr) +
                                         " next: " + to_string(next_itr) + " id: " + to_string(id) +
                                         " expected: " + to_string(expected_id));
}

void check_prev_i64(int itr, int expected_itr, uint64_t expected_id) {
   uint64_t id       = 0xfeedbeef;
   auto     next_itr = db_previous_i64(itr, &id);
   eosio::check(next_itr == expected_itr, "check_prev_i64 failure: wrong iterator. itr: " + to_string(itr) + " prev: " +
                                                to_string(next_itr) + " expected: " + to_string(expected_itr) + "\n");
   eosio::check(id == expected_id, "check_prev_i64 failure: wrong id. itr: " + to_string(itr) +
                                         " prev: " + to_string(next_itr) + " id: " + to_string(id) +
                                         " expected: " + to_string(expected_id));
}

name p(name prefix, name suffix) { return name{ prefix.value | (suffix.value >> 10) }; }

checksum256 to_cs(const char (&cs)[65]) {
   uint8_t bytes[32];
   (void)unhex(bytes, cs, cs + 64);
   return checksum256{ bytes };
}

void store_multiple(name scope, name table, name payer, uint64_t id, const std::vector<char>& data, uint64_t i64,
                    uint128_t i128, double d, const char (&cs)[65]) {
   store_i64(scope, table, payer, id, data);
   idx64::store(p("a"_n, scope), p("a"_n, table), payer, id, i64);
   idx128::store(p("b"_n, scope), p("b"_n, table), payer, id, i128);
   idx_double::store(p("c"_n, scope), p("c"_n, table), payer, id, d);
   idx256::store(p("d"_n, scope), p("d"_n, table), payer, id, to_cs(cs));
}

void check_lowerbound_multiple(name code, name scope, name table, int expected_itr, uint64_t expected_primary,
                               uint64_t idx64_search, uint64_t idx64_expected, uint128_t idx128_search,
                               uint128_t idx128_expected, double dbl_search, double dbl_expected,
                               const char (&cs_search)[65], const char (&cs_expected)[65]) {
   idx64::check_lowerbound(code, p("a"_n, scope), p("a"_n, table), expected_itr, expected_primary, idx64_search,
                           idx64_expected);
   idx128::check_lowerbound(code, p("b"_n, scope), p("b"_n, table), expected_itr, expected_primary, idx128_search,
                            idx128_expected);
   idx_double::check_lowerbound(code, p("c"_n, scope), p("c"_n, table), expected_itr, expected_primary, dbl_search,
                                dbl_expected);
   idx256::check_lowerbound(code, p("d"_n, scope), p("d"_n, table), expected_itr, expected_primary, to_cs(cs_search),
                            to_cs(cs_expected));
}

void check_next_multiple(int32_t iterator, int expected_itr, uint64_t expected_primary) {
   idx64::check_next(iterator, expected_itr, expected_primary);
   idx128::check_next(iterator, expected_itr, expected_primary);
   idx_double::check_next(iterator, expected_itr, expected_primary);
   idx256::check_next(iterator, expected_itr, expected_primary);
}

void check_prev_multiple(int32_t iterator, int expected_itr, uint64_t expected_primary) {
   idx64::check_prev(iterator, expected_itr, expected_primary);
   idx128::check_prev(iterator, expected_itr, expected_primary);
   idx_double::check_prev(iterator, expected_itr, expected_primary);
   idx256::check_prev(iterator, expected_itr, expected_primary);
}

void check_upperbound_multiple(name code, name scope, name table, int expected_itr, uint64_t expected_primary,
                               uint64_t idx64_search, uint64_t idx64_expected, uint128_t idx128_search,
                               uint128_t idx128_expected, double dbl_search, double dbl_expected,
                               const char (&cs_search)[65], const char (&cs_expected)[65]) {
   idx64::check_upperbound(code, p("a"_n, scope), p("a"_n, table), expected_itr, expected_primary, idx64_search,
                           idx64_expected);
   idx128::check_upperbound(code, p("b"_n, scope), p("b"_n, table), expected_itr, expected_primary, idx128_search,
                            idx128_expected);
   idx_double::check_upperbound(code, p("c"_n, scope), p("c"_n, table), expected_itr, expected_primary, dbl_search,
                                dbl_expected);
   idx256::check_upperbound(code, p("d"_n, scope), p("d"_n, table), expected_itr, expected_primary, to_cs(cs_search),
                            to_cs(cs_expected));
}

void check_find_secondary_multiple(name code, name scope, name table, int expected_itr, uint64_t expected_primary,
                                   uint64_t idx64_search, uint128_t idx128_search, double dbl_search,
                                   const char (&cs_search)[65]) {
   idx64::check_find_secondary(code, p("a"_n, scope), p("a"_n, table), expected_itr, expected_primary, idx64_search);
   idx128::check_find_secondary(code, p("b"_n, scope), p("b"_n, table), expected_itr, expected_primary, idx128_search);
   idx_double::check_find_secondary(code, p("c"_n, scope), p("c"_n, table), expected_itr, expected_primary, dbl_search);
   idx256::check_find_secondary(code, p("d"_n, scope), p("d"_n, table), expected_itr, expected_primary,
                                to_cs(cs_search));
}

auto big(uint64_t msb, uint64_t lsb) { return (uint128_t(msb) << 64) | lsb; }

auto big(uint64_t lsb) { return big(0xffff'ffff'ffff'ffff, lsb); }

void legacydb_contract::write() {
   print("write\n");
   // clang-format off
   store_multiple("scope1"_n, "table1"_n, get_self(), 20, data_0, 0x2020, 0x2020'2020, -0.25,                                                                  "00000000000000000000000000000000000000000000000000000000000000ff");
   store_multiple("scope1"_n, "table1"_n, get_self(), 21, data_1, 0x2121, 0x2121'2121, -0.125,                                                                 "000000000000000000000000000000000000000000000000000000000000ff00");
   store_multiple("scope1"_n, "table1"_n, get_self(), 22, data_2, 0x2222, 0x2222'2222, 0.0,                                                                    "0000000000000000000000000000000000000000000000000000000000ff0000");
   store_multiple("scope1"_n, "table1"_n, get_self(), 23, data_3, 0x2323, 0x2323'2323, 0.125,                                                                  "00000000000000000000000000000000000000000000000000000000ff000000");
   store_multiple("scope1"_n, "table1"_n, get_self(), 24, data_4, 0x2424, 0x2424'2424, 0.25,                                                                   "000000000000000000000000000000000000000000000000000000ff00000000");

   store_multiple("scope1"_n, "table2"_n, get_self(), 30, data_4, 0x3030, 0x3030'3030, -2.0,                                                                   "00000000000000000000000000000000000000000000000000000000000000ff");
   store_multiple("scope1"_n, "table2"_n, get_self(), 31, data_0, 0x3131, 0x3131'3131, -1.0,                                                                   "0000000000000000000000000000000000000000000000ff0000000000000000");
   store_multiple("scope1"_n, "table2"_n, get_self(), 32, data_2, 0x3232, 0x3232'3232, -0.0,                                                                   "000000000000000000000000000000ff00000000000000000000000000000000");
   store_multiple("scope1"_n, "table2"_n, get_self(), 33, data_1, 0x3333, 0x3333'3333, 1.0,                                                                    "00000000000000ff000000000000000000000000000000000000000000000000");
   store_multiple("scope1"_n, "table2"_n, get_self(), 34, data_3, 0x3434, 0x3434'3434, 2.0,                                                                    "000000ff00000000000000000000000000000000000000000000000000000000");

   store_multiple("scope1"_n, "table3"_n, get_self(), 40, data_0, 0x4040, 0x4040'4040, exp2(-500),                                                             "00000000000000000000000000000000000000000000000000000000000000ff");
   store_multiple("scope1"_n, "table3"_n, get_self(), 41, data_1, 0x4141, 0x4141'4141, exp2(-200),                                                             "000000000000000000000000000000000000000000000000000000ff00000000");
   store_multiple("scope1"_n, "table3"_n, get_self(), 42, data_2, 0x4242, 0x4242'4242, exp2(0),                                                                "0000000000000000000000000000000000000000000000ff0000000000000000");
   store_multiple("scope1"_n, "table3"_n, get_self(), 43, data_3, 0x4343, 0x4343'4343, exp2(200),                                                              "00000000000000000000000000000000000000ff000000000000000000000000");
   store_multiple("scope1"_n, "table3"_n, get_self(), 44, data_4, 0x4444, 0x4444'4444, exp2(500),                                                              "000000000000000000000000000000ff00000000000000000000000000000000");

   store_multiple("scope2"_n, "table1"_n, get_self(), 50, data_0, 0x5050, 0x5050'5050, pow(2, 0),                                                              "000000000000000000000000000000ff00000000000000000000000000000000");
   store_multiple("scope2"_n, "table1"_n, get_self(), 51, data_1, 0x5151, 0x5151'5151, pow(2, 100),                                                            "0000000000000000000000ff0000000000000000000000000000000000000000");
   store_multiple("scope2"_n, "table1"_n, get_self(), 52, data_2, 0x5252, 0x5252'5252, pow(2, 200),                                                            "00000000000000ff000000000000000000000000000000000000000000000000");
   store_multiple("scope2"_n, "table1"_n, get_self(), 53, data_3, 0x5353, 0x5353'5353, pow(2, 300),                                                            "000000ff00000000000000000000000000000000000000000000000000000000");
   store_multiple("scope2"_n, "table1"_n, get_self(), 54, data_4, 0x5454, 0x5454'5454, pow(2, 400),                                                            "00ff000000000000000000000000000000000000000000000000000000000000");

   store_multiple("scope2"_n, "atable"_n, get_self(), 60, data_0, 0x6060, 0x6060'6060, -pow(2, 400),                                                           "0000000000000000000000000000000000000000000000000000000000000000");
   store_multiple("scope2"_n, "atable"_n, get_self(), 61, data_1, 0x6161, 0x6161'6161, -pow(2, 300),                                                           "0000000000000000000000000000000000000000000000000000000000000000");
   store_multiple("scope2"_n, "atable"_n, get_self(), 62, data_2, 0x6262, 0x6262'6262, -pow(2, 200),                                                           "0000000000000000000000000000000000000000000000000000000000000000");
   store_multiple("scope2"_n, "atable"_n, get_self(), 63, data_3, 0x6363, 0x6363'6363, -pow(2, 100),                                                           "0000000000000000000000000000000000000000000000000000000000000000");
   store_multiple("scope2"_n, "atable"_n, get_self(), 64, data_4, 0x6464, 0x6464'6464, -pow(2, 0),                                                             "0000000000000000000000000000000000000000000000000000000000000000");
   store_multiple("scope2"_n, "atable"_n, get_self(), 0xffff'ffff'ffff'fffe, data_2, 0xffff'ffff'ffff'fffe, big(0xffff'ffff'ffff'fffe), pow(2, 1000),          "0000000000000000000000000000000000000000000000000000000000000000");
   store_multiple("scope2"_n, "atable"_n, get_self(), 0xffff'ffff'ffff'ffff, data_3, 0xffff'ffff'ffff'ffff, big(0xffff'ffff'ffff'ffff), pow(2, 1000) * 1.125,  "0000000000000000000000000000000000000000000000000000000000000000");

   store_multiple("scope.x"_n, "table1"_n, get_self(), 54, data_0, 0x5454, 0x5454'5454, -1.0,                                                                  "0000000000000000000000000000000000000000000000000000000000000000");
   store_multiple("scope.x"_n, "table2"_n, get_self(), 54, data_1, 0x5454, 0x5454'5454, 1.0,                                                                   "0000000000000000000000000000000000000000000000000000000000000000");
   // clang-format on

   idx64::store("nope"_n, "just.2nd"_n, get_self(), 42, 42);
} // legacydb_contract::write()

// This is one big test to help make sure:
// * Iterator indexes are reused when they should be. CDT's multi_index depends on this.
// * Iterator indexes are not reused when they shouldn't be. CDT's multi_index depends on this.
// * Nodeos and rodeos generate the same iterator indexes. This isn't a requirement since rodeos
//   doesn't enforce consensus, but this property acts as a convenient proxy for testing that
//   nodeos and rodeos have the same logical behavior. CDT's multi_index doesn't need this
//   property and users which call the intrinsics directly shouldn't depend on it.
//
// Unintential side effect of this test: if someone makes a change to nodeos which changes the
// specific iterator indexes it returns, this test may catch it. Such a change is consensus breaking.
void legacydb_contract::read() {
   print("read\n");

   /////////////////////////////////
   // find items in order -> produces iterator indexes in order
   // creates iterators -2, 0 - 4
   /////////////////////////////////

   check_lowerbound_i64(get_self(), "scope1"_n, "table1"_n, 0, 0, data_0);
   check_lowerbound_i64(get_self(), "scope1"_n, "table1"_n, 20, 0, data_0); // reuse existing itr
   check_lowerbound_i64(get_self(), "scope1"_n, "table1"_n, 21, 1, data_1);
   check_lowerbound_i64(get_self(), "scope1"_n, "table1"_n, 21, 1, data_1); // reuse existing itr
   check_lowerbound_i64(get_self(), "scope1"_n, "table1"_n, 22, 2, data_2);
   check_lowerbound_i64(get_self(), "scope1"_n, "table1"_n, 23, 3, data_3);
   check_lowerbound_i64(get_self(), "scope1"_n, "table1"_n, 24, 4, data_4);
   check_lowerbound_i64(get_self(), "scope1"_n, "table1"_n, 25, -2, {});
   check_lowerbound_i64(get_self(), "nope"_n, "nada"_n, 25, -1, {});

   // clang-format off
   check_lowerbound_multiple(get_self(), "scope1"_n, "table1"_n, 0,  20,           0x0,    0x2020,  0x0,         0x2020'2020,  -9.0,   -0.25,  "0000000000000000000000000000000000000000000000000000000000000000", "00000000000000000000000000000000000000000000000000000000000000ff");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table1"_n, 0,  20,           0x2020, 0x2020,  0x2020'2020, 0x2020'2020,  -0.25,  -0.25,  "00000000000000000000000000000000000000000000000000000000000000ff", "00000000000000000000000000000000000000000000000000000000000000ff");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table1"_n, 1,  21,           0x2121, 0x2121,  0x2121'2121, 0x2121'2121,  -0.125, -0.125, "000000000000000000000000000000000000000000000000000000000000ff00", "000000000000000000000000000000000000000000000000000000000000ff00");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table1"_n, 1,  21,           0x2121, 0x2121,  0x2121'2121, 0x2121'2121,  -0.125, -0.125, "000000000000000000000000000000000000000000000000000000000000ff00", "000000000000000000000000000000000000000000000000000000000000ff00");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table1"_n, 1,  21,           0x2120, 0x2121,  0x2121'2120, 0x2121'2121,  -0.1875,-0.125, "000000000000000000000000000000000000000000000000000000000000feff", "000000000000000000000000000000000000000000000000000000000000ff00");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table1"_n, 2,  22,           0x2222, 0x2222,  0x2222'2222, 0x2222'2222,   0.0,    0.0,   "0000000000000000000000000000000000000000000000000000000000ff0000", "0000000000000000000000000000000000000000000000000000000000ff0000");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table1"_n, 3,  23,           0x2323, 0x2323,  0x2323'2323, 0x2323'2323,   0.125,  0.125, "00000000000000000000000000000000000000000000000000000000ff000000", "00000000000000000000000000000000000000000000000000000000ff000000");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table1"_n, 4,  24,           0x2423, 0x2424,  0x2424'2423, 0x2424'2424,   0.1875, 0.25,  "000000000000000000000000000000000000000000000000000000feffffffff", "000000000000000000000000000000000000000000000000000000ff00000000");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table1"_n, 4,  24,           0x2424, 0x2424,  0x2424'2424, 0x2424'2424,   0.25,   0.25,  "000000000000000000000000000000000000000000000000000000ff00000000", "000000000000000000000000000000000000000000000000000000ff00000000");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table1"_n, -2, 0xfeedbeef,   0x2525, 0x2525,  0x2525'2525, 0x2525'2525,   1.25,   1.25,  "123456789ABCDEF0837489273953467859372347722281123322823842835835", "123456789ABCDEF0837489273953467859372347722281123322823842835835");
   check_lowerbound_multiple(get_self(), "nope"_n,   "nada"_n,   -1, 0xfeedbeef,   0x2525, 0x2525,  0x2525'2525, 0x2525'2525,   1.25,   1.25,  "9837438972347345793474567058969803274923874743573458967498567948", "9837438972347345793474567058969803274923874743573458967498567948");
   // clang-format on

   check_find_i64(get_self(), "scope1"_n, "table1"_n, 0, -2, {});
   check_find_i64(get_self(), "scope1"_n, "table1"_n, 20, 0, data_0);
   check_find_i64(get_self(), "scope1"_n, "table1"_n, 21, 1, data_1);
   check_find_i64(get_self(), "scope1"_n, "table1"_n, 22, 2, data_2);
   check_find_i64(get_self(), "scope1"_n, "table1"_n, 23, 3, data_3);
   check_find_i64(get_self(), "scope1"_n, "table1"_n, 24, 4, data_4);
   check_find_i64(get_self(), "scope1"_n, "table1"_n, 25, -2, {});
   check_find_i64(get_self(), "nope"_n, "nada"_n, 25, -1, {});

   // clang-format off
   check_find_secondary_multiple(get_self(), "scope1"_n, "table1"_n, -2,  0xfeedbeef,   0x0,    0x0,         -9.0,   "0000000000000000000000000000000000000000000000000000000000000000");
   check_find_secondary_multiple(get_self(), "scope1"_n, "table1"_n,  0,  20,           0x2020, 0x2020'2020, -0.25,  "00000000000000000000000000000000000000000000000000000000000000ff");
   check_find_secondary_multiple(get_self(), "scope1"_n, "table1"_n,  1,  21,           0x2121, 0x2121'2121, -0.125, "000000000000000000000000000000000000000000000000000000000000ff00");
   check_find_secondary_multiple(get_self(), "scope1"_n, "table1"_n,  1,  21,           0x2121, 0x2121'2121, -0.125, "000000000000000000000000000000000000000000000000000000000000ff00");
   check_find_secondary_multiple(get_self(), "scope1"_n, "table1"_n,  2,  22,           0x2222, 0x2222'2222,  0.0,   "0000000000000000000000000000000000000000000000000000000000ff0000");
   check_find_secondary_multiple(get_self(), "scope1"_n, "table1"_n,  3,  23,           0x2323, 0x2323'2323,  0.125, "00000000000000000000000000000000000000000000000000000000ff000000");
   check_find_secondary_multiple(get_self(), "scope1"_n, "table1"_n,  4,  24,           0x2424, 0x2424'2424,  0.25,  "000000000000000000000000000000000000000000000000000000ff00000000");
   check_find_secondary_multiple(get_self(), "scope1"_n, "table1"_n, -2,  0xfeedbeef,   0x2525, 0x2525'2525,  1.25,  "123456789ABCDEF0837489273953467859372347722281123322823842835835");
   check_find_secondary_multiple(get_self(), "nope"_n,   "nada"_n,   -1,  0xfeedbeef,   0x2525, 0x2525'2525,  1.25,  "9837438972347345793474567058969803274923874743573458967498567948");
   // clang-format on

   check_upperbound_i64(get_self(), "scope1"_n, "table1"_n, 0, 0, data_0);
   check_upperbound_i64(get_self(), "scope1"_n, "table1"_n, 19, 0, data_0);
   check_upperbound_i64(get_self(), "scope1"_n, "table1"_n, 20, 1, data_1);
   check_upperbound_i64(get_self(), "scope1"_n, "table1"_n, 21, 2, data_2);
   check_upperbound_i64(get_self(), "scope1"_n, "table1"_n, 22, 3, data_3);
   check_upperbound_i64(get_self(), "scope1"_n, "table1"_n, 23, 4, data_4);
   check_upperbound_i64(get_self(), "scope1"_n, "table1"_n, 24, -2, {});
   check_upperbound_i64(get_self(), "scope1"_n, "table1"_n, 25, -2, {});
   check_upperbound_i64(get_self(), "scope1"_n, "table1"_n, 0xffff'ffff'ffff'fffe, -2, {});
   check_upperbound_i64(get_self(), "scope1"_n, "table1"_n, 0xffff'ffff'ffff'ffff, -2, {});
   check_upperbound_i64(get_self(), "nope"_n, "nada"_n, 25, -1, {});

   // clang-format off
   check_upperbound_multiple(get_self(), "scope1"_n, "table1"_n,  0, 20,         0x2019, 0x2020, 0x2020'2019, 0x2020'2020, -0.50,  -0.25,  "00000000000000000000000000000000000000000000000000000000000000fe", "00000000000000000000000000000000000000000000000000000000000000ff");
   check_upperbound_multiple(get_self(), "scope1"_n, "table1"_n,  1, 21,         0x2020, 0x2121, 0x2020'2020, 0x2121'2121, -0.25,  -0.125, "00000000000000000000000000000000000000000000000000000000000000ff", "000000000000000000000000000000000000000000000000000000000000ff00");
   check_upperbound_multiple(get_self(), "scope1"_n, "table1"_n,  1, 21,         0x2120, 0x2121, 0x2121'2120, 0x2121'2121, -0.1875,-0.125, "000000000000000000000000000000000000000000000000000000000000feff", "000000000000000000000000000000000000000000000000000000000000ff00");
   check_upperbound_multiple(get_self(), "scope1"_n, "table1"_n,  2, 22,         0x2221, 0x2222, 0x2222'2221, 0x2222'2222, -0.0625, 0.0,   "0000000000000000000000000000000000000000000000000000000000feffff", "0000000000000000000000000000000000000000000000000000000000ff0000");
   check_upperbound_multiple(get_self(), "scope1"_n, "table1"_n,  2, 22,         0x2121, 0x2222, 0x2121'2121, 0x2222'2222, -0.125,  0.0,   "000000000000000000000000000000000000000000000000000000000000ff00", "0000000000000000000000000000000000000000000000000000000000ff0000");
   check_upperbound_multiple(get_self(), "scope1"_n, "table1"_n,  3, 23,         0x2222, 0x2323, 0x2222'2222, 0x2323'2323,  0.0,    0.125, "0000000000000000000000000000000000000000000000000000000000ff0000", "00000000000000000000000000000000000000000000000000000000ff000000");
   check_upperbound_multiple(get_self(), "scope1"_n, "table1"_n,  4, 24,         0x2323, 0x2424, 0x2323'2323, 0x2424'2424,  0.125,  0.25,  "00000000000000000000000000000000000000000000000000000000ff000000", "000000000000000000000000000000000000000000000000000000ff00000000");
   check_upperbound_multiple(get_self(), "scope1"_n, "table1"_n, -2, 0xfeedbeef, 0x2424, 0x2424, 0x2424'2424, 0x2424'2424,  0.25,   0.25,  "000000000000000000000000000000000000000000000000000000ff00000000", "000000000000000000000000000000000000000000000000000000ff00000000");
   // clang-format on

   check_next_i64(0, 1, 21);
   check_next_i64(1, 2, 22);
   check_next_i64(2, 3, 23);
   check_next_i64(3, 4, 24);
   check_next_i64(4, -2, 0xfeedbeef);
   check_next_i64(-2, -1, 0xfeedbeef);

   check_next_multiple(0, 1, 21);
   check_next_multiple(1, 2, 22);
   check_next_multiple(2, 3, 23);
   check_next_multiple(3, 4, 24);
   check_next_multiple(4, -2, 0xfeedbeef);
   check_next_multiple(-2, -1, 0xfeedbeef);

   check_prev_i64(-2, 4, 24);
   check_prev_i64(4, 3, 23);
   check_prev_i64(3, 2, 22);
   check_prev_i64(2, 1, 21);
   check_prev_i64(1, 0, 20);
   check_prev_i64(0, -1, 0xfeedbeef);

   check_prev_multiple(-2, 4, 24);
   check_prev_multiple(4, 3, 23);
   check_prev_multiple(3, 2, 22);
   check_prev_multiple(2, 1, 21);
   check_prev_multiple(1, 0, 20);
   check_prev_multiple(0, -1, 0xfeedbeef);

   /////////////////////////////////
   // find items out of order -> produces iterator indexes out of order
   // creates iterators -3, 5 - 9
   /////////////////////////////////

   check_find_i64(get_self(), "scope1"_n, "table2"_n, 31, 5, data_0);
   check_find_i64(get_self(), "scope1"_n, "table2"_n, 33, 6, data_1);
   check_find_i64(get_self(), "scope1"_n, "table2"_n, 32, 7, data_2);
   check_find_i64(get_self(), "scope1"_n, "table2"_n, 34, 8, data_3);
   check_find_i64(get_self(), "scope1"_n, "table2"_n, 30, 9, data_4);
   check_find_i64(get_self(), "scope1"_n, "table2"_n, 0, -3, {});
   check_find_i64(get_self(), "scope1"_n, "table2"_n, 35, -3, {});

   // clang-format off
   check_find_secondary_multiple(get_self(), "scope1"_n, "table2"_n,  5, 31,         0x3131, 0x3131'3131, -1.0, "0000000000000000000000000000000000000000000000ff0000000000000000");
   check_find_secondary_multiple(get_self(), "scope1"_n, "table2"_n,  6, 33,         0x3333, 0x3333'3333,  1.0, "00000000000000ff000000000000000000000000000000000000000000000000");
   check_find_secondary_multiple(get_self(), "scope1"_n, "table2"_n,  7, 32,         0x3232, 0x3232'3232,  0.0, "000000000000000000000000000000ff00000000000000000000000000000000");
   check_find_secondary_multiple(get_self(), "scope1"_n, "table2"_n,  8, 34,         0x3434, 0x3434'3434,  2.0, "000000ff00000000000000000000000000000000000000000000000000000000");
   check_find_secondary_multiple(get_self(), "scope1"_n, "table2"_n,  9, 30,         0x3030, 0x3030'3030, -2.0, "00000000000000000000000000000000000000000000000000000000000000ff");
   check_find_secondary_multiple(get_self(), "scope1"_n, "table2"_n, -3, 0xfeedbeef, 0x0000, 0x0000'0000, -3.0, "0000000000000000000000000000000000000000000000000000000000000000");
   check_find_secondary_multiple(get_self(), "scope1"_n, "table2"_n, -3, 0xfeedbeef, 0x3535, 0x3535'3535,  7.5, "ff00000000000000000000000000000000000000000000000000000000000000");
   // clang-format on

   check_lowerbound_i64(get_self(), "scope1"_n, "table2"_n, 31, 5, data_0);
   check_lowerbound_i64(get_self(), "scope1"_n, "table2"_n, 33, 6, data_1);
   check_lowerbound_i64(get_self(), "scope1"_n, "table2"_n, 32, 7, data_2);
   check_lowerbound_i64(get_self(), "scope1"_n, "table2"_n, 34, 8, data_3);
   check_lowerbound_i64(get_self(), "scope1"_n, "table2"_n, 30, 9, data_4);
   check_lowerbound_i64(get_self(), "scope1"_n, "table2"_n, 0, 9, data_4);
   check_lowerbound_i64(get_self(), "scope1"_n, "table2"_n, 35, -3, {});

   // clang-format off
   check_lowerbound_multiple(get_self(), "scope1"_n, "table2"_n,  5, 31,         0x3131, 0x3131,   0x3131'3131, 0x3131'3131, -1.0, -1.0, "0000000000000000000000000000000000000000000000ff0000000000000000", "0000000000000000000000000000000000000000000000ff0000000000000000");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table2"_n,  6, 33,         0x3333, 0x3333,   0x3333'3333, 0x3333'3333,  1.0,  1.0, "00000000000000ff000000000000000000000000000000000000000000000000", "00000000000000ff000000000000000000000000000000000000000000000000");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table2"_n,  7, 32,         0x3232, 0x3232,   0x3232'3232, 0x3232'3232,  0.0,  0.0, "000000000000000000000000000000ff00000000000000000000000000000000", "000000000000000000000000000000ff00000000000000000000000000000000");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table2"_n,  8, 34,         0x3434, 0x3434,   0x3434'3434, 0x3434'3434,  2.0,  2.0, "000000ff00000000000000000000000000000000000000000000000000000000", "000000ff00000000000000000000000000000000000000000000000000000000");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table2"_n,  9, 30,         0x3030, 0x3030,   0x3030'3030, 0x3030'3030, -2.0, -2.0, "00000000000000000000000000000000000000000000000000000000000000ff", "00000000000000000000000000000000000000000000000000000000000000ff");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table2"_n,  9, 30,         0x0000, 0x3030,   0x0000'0000, 0x3030'3030, -3.0, -2.0, "0000000000000000000000000000000000000000000000000000000000000000", "00000000000000000000000000000000000000000000000000000000000000ff");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table2"_n, -3, 0xfeedbeef, 0x3535, 0x3535,   0x3535'3535, 0x3535'3535,  7.5,  7.5, "ff00000000000000000000000000000000000000000000000000000000000000", "ff00000000000000000000000000000000000000000000000000000000000000");
   // clang-format on

   check_next_i64(9, 5, 31);
   check_next_i64(5, 7, 32);
   check_next_i64(7, 6, 33);
   check_next_i64(6, 8, 34);
   check_next_i64(8, -3, 0xfeedbeef);
   check_next_i64(-3, -1, 0xfeedbeef);

   check_next_multiple(9, 5, 31);
   check_next_multiple(5, 7, 32);
   check_next_multiple(7, 6, 33);
   check_next_multiple(6, 8, 34);
   check_next_multiple(8, -3, 0xfeedbeef);
   check_next_multiple(-3, -1, 0xfeedbeef);

   check_prev_i64(-3, 8, 34);
   check_prev_i64(8, 6, 33);
   check_prev_i64(6, 7, 32);
   check_prev_i64(7, 5, 31);
   check_prev_i64(5, 9, 30);
   check_prev_i64(9, -1, 0xfeedbeef);

   check_prev_multiple(-3, 8, 34);
   check_prev_multiple(8, 6, 33);
   check_prev_multiple(6, 7, 32);
   check_prev_multiple(7, 5, 31);
   check_prev_multiple(5, 9, 30);
   check_prev_multiple(9, -1, 0xfeedbeef);

   /////////////////////////////////
   // iterate through table that has been only partially searched through
   // creates iterators -4, 10 - 14
   /////////////////////////////////

   check_lowerbound_i64(get_self(), "scope1"_n, "table3"_n, 40, 10, data_0);
   check_lowerbound_i64(get_self(), "scope1"_n, "table3"_n, 43, 11, data_3);
   check_next_i64(10, 12, 41);
   check_next_i64(12, 13, 42);
   check_next_i64(13, 11, 43);
   check_next_i64(11, 14, 44);
   check_next_i64(14, -4, 0xfeedbeef);
   check_next_i64(-4, -1, 0xfeedbeef);

   // clang-format off
   check_lowerbound_multiple(get_self(), "scope1"_n, "table3"_n, 10, 40, 0x4040, 0x4040, 0x4040'4040, 0x4040'4040, exp2(-500), exp2(-500), "00000000000000000000000000000000000000000000000000000000000000ff", "00000000000000000000000000000000000000000000000000000000000000ff");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table3"_n, 11, 43, 0x4343, 0x4343, 0x4343'4343, 0x4343'4343, exp2(200),  exp2(200),  "00000000000000000000000000000000000000ff000000000000000000000000", "00000000000000000000000000000000000000ff000000000000000000000000");
   check_next_multiple(10, 12, 41);
   check_next_multiple(12, 13, 42);
   check_next_multiple(13, 11, 43);
   check_next_multiple(11, 14, 44);
   check_next_multiple(14, -4, 0xfeedbeef);
   check_next_multiple(-4, -1, 0xfeedbeef);
   // clang-format on

   check_prev_i64(-4, 14, 44);
   check_prev_i64(14, 11, 43);
   check_prev_i64(11, 13, 42);
   check_prev_i64(13, 12, 41);
   check_prev_i64(12, 10, 40);
   check_prev_i64(10, -1, 0xfeedbeef);

   check_prev_multiple(-4, 14, 44);
   check_prev_multiple(14, 11, 43);
   check_prev_multiple(11, 13, 42);
   check_prev_multiple(13, 12, 41);
   check_prev_multiple(12, 10, 40);
   check_prev_multiple(10, -1, 0xfeedbeef);

   check_lowerbound_i64(get_self(), "scope1"_n, "table3"_n, 40, 10, data_0);
   check_lowerbound_i64(get_self(), "scope1"_n, "table3"_n, 0, 10, data_0);
   check_lowerbound_i64(get_self(), "scope1"_n, "table3"_n, 41, 12, data_1);
   check_lowerbound_i64(get_self(), "scope1"_n, "table3"_n, 42, 13, data_2);
   check_lowerbound_i64(get_self(), "scope1"_n, "table3"_n, 43, 11, data_3);
   check_lowerbound_i64(get_self(), "scope1"_n, "table3"_n, 44, 14, data_4);
   check_lowerbound_i64(get_self(), "scope1"_n, "table3"_n, 45, -4, {});

   // clang-format off
   check_lowerbound_multiple(get_self(), "scope1"_n, "table3"_n, 10, 40,         0x4040, 0x4040, 0x4040'4040, 0x4040'4040, exp2(-500), exp2(-500), "00000000000000000000000000000000000000000000000000000000000000ff", "00000000000000000000000000000000000000000000000000000000000000ff");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table3"_n, 10, 40,         0x0000, 0x4040, 0x0000'0000, 0x4040'4040, 0.0,        exp2(-500), "0000000000000000000000000000000000000000000000000000000000000000", "00000000000000000000000000000000000000000000000000000000000000ff");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table3"_n, 12, 41,         0x4141, 0x4141, 0x4141'4141, 0x4141'4141, exp2(-200), exp2(-200), "000000000000000000000000000000000000000000000000000000ff00000000", "000000000000000000000000000000000000000000000000000000ff00000000");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table3"_n, 13, 42,         0x4242, 0x4242, 0x4242'4242, 0x4242'4242, exp2(0),    exp2(0),    "0000000000000000000000000000000000000000000000ff0000000000000000", "0000000000000000000000000000000000000000000000ff0000000000000000");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table3"_n, 11, 43,         0x4343, 0x4343, 0x4343'4343, 0x4343'4343, exp2(200),  exp2(200),  "00000000000000000000000000000000000000ff000000000000000000000000", "00000000000000000000000000000000000000ff000000000000000000000000");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table3"_n, 14, 44,         0x4444, 0x4444, 0x4444'4444, 0x4444'4444, exp2(500),  exp2(500),  "000000000000000000000000000000ff00000000000000000000000000000000", "000000000000000000000000000000ff00000000000000000000000000000000");
   check_lowerbound_multiple(get_self(), "scope1"_n, "table3"_n, -4, 0xfeedbeef, 0x4445, 0x4445, 0x4444'4445, 0x4444'4445, exp2(501),  exp2(501),  "000000000000000000000000000000ff00000000000000000000000000000001", "000000000000000000000000000000ff00000000000000000000000000000001");
   // clang-format on

   check_find_i64(get_self(), "scope1"_n, "table3"_n, 40, 10, data_0);
   check_find_i64(get_self(), "scope1"_n, "table3"_n, 0, -4, {});
   check_find_i64(get_self(), "scope1"_n, "table3"_n, 41, 12, data_1);
   check_find_i64(get_self(), "scope1"_n, "table3"_n, 42, 13, data_2);
   check_find_i64(get_self(), "scope1"_n, "table3"_n, 43, 11, data_3);
   check_find_i64(get_self(), "scope1"_n, "table3"_n, 44, 14, data_4);
   check_find_i64(get_self(), "scope1"_n, "table3"_n, 45, -4, {});

   /////////////////////////////////
   // iterate (reverse) through table that has been only partially searched through
   // creates iterators -5, 15 - 19
   /////////////////////////////////

   check_find_i64(get_self(), "scope2"_n, "table1"_n, 50, 15, data_0);
   check_find_i64(get_self(), "scope2"_n, "table1"_n, 53, 16, data_3);
   check_prev_i64(-5, 17, 54);
   check_prev_i64(17, 16, 53);
   check_prev_i64(16, 18, 52);
   check_prev_i64(18, 19, 51);
   check_prev_i64(19, 15, 50);
   check_prev_i64(15, -1, 0xfeedbeef);

   check_next_i64(15, 19, 51);
   check_next_i64(19, 18, 52);
   check_next_i64(18, 16, 53);
   check_next_i64(16, 17, 54);
   check_next_i64(17, -5, 0xfeedbeef);
   check_next_i64(-5, -1, 0xfeedbeef);

   check_find_i64(get_self(), "scope2"_n, "table1"_n, 0, -5, {});
   check_find_i64(get_self(), "scope2"_n, "table1"_n, 50, 15, data_0);
   check_find_i64(get_self(), "scope2"_n, "table1"_n, 51, 19, data_1);
   check_find_i64(get_self(), "scope2"_n, "table1"_n, 52, 18, data_2);
   check_find_i64(get_self(), "scope2"_n, "table1"_n, 53, 16, data_3);
   check_find_i64(get_self(), "scope2"_n, "table1"_n, 54, 17, data_4);
   check_find_i64(get_self(), "scope2"_n, "table1"_n, 55, -5, {});

   check_lowerbound_i64(get_self(), "scope2"_n, "table1"_n, 0, 15, data_0);
   check_lowerbound_i64(get_self(), "scope2"_n, "table1"_n, 50, 15, data_0);
   check_lowerbound_i64(get_self(), "scope2"_n, "table1"_n, 51, 19, data_1);
   check_lowerbound_i64(get_self(), "scope2"_n, "table1"_n, 52, 18, data_2);
   check_lowerbound_i64(get_self(), "scope2"_n, "table1"_n, 53, 16, data_3);
   check_lowerbound_i64(get_self(), "scope2"_n, "table1"_n, 54, 17, data_4);
   check_lowerbound_i64(get_self(), "scope2"_n, "table1"_n, 55, -5, {});

   /////////////////////////////////
   // find items out of order using upperbound
   // creates iterators -6, 20 - 26
   /////////////////////////////////

   check_upperbound_i64(get_self(), "scope2"_n, "atable"_n, 63, 20, data_4);
   check_upperbound_i64(get_self(), "scope2"_n, "atable"_n, 60, 21, data_1);
   check_upperbound_i64(get_self(), "scope2"_n, "atable"_n, 0xffff'ffff'ffff'fffe, 22, data_3);
   check_upperbound_i64(get_self(), "scope2"_n, "atable"_n, 60, 21, data_1); // reuse existing itr
   check_upperbound_i64(get_self(), "scope2"_n, "atable"_n, 61, 23, data_2);
   check_upperbound_i64(get_self(), "scope2"_n, "atable"_n, 0xffff'ffff'ffff'ffff, -6, {});
   check_upperbound_i64(get_self(), "scope2"_n, "atable"_n, 59, 24, data_0);
   check_upperbound_i64(get_self(), "scope2"_n, "atable"_n, 58, 24, data_0); // reuse existing itr
   check_upperbound_i64(get_self(), "scope2"_n, "atable"_n, 0xffff'ffff'ffff'fffd, 25, data_2);
   check_upperbound_i64(get_self(), "scope2"_n, "atable"_n, 62, 26, data_3);

   check_lowerbound_i64(get_self(), "scope2"_n, "atable"_n, 64, 20, data_4);
   check_lowerbound_i64(get_self(), "scope2"_n, "atable"_n, 65, 25, data_2);
   check_lowerbound_i64(get_self(), "scope2"_n, "atable"_n, 61, 21, data_1);
   check_lowerbound_i64(get_self(), "scope2"_n, "atable"_n, 0xffff'ffff'ffff'ffff, 22, data_3);
   check_lowerbound_i64(get_self(), "scope2"_n, "atable"_n, 62, 23, data_2);
   check_lowerbound_i64(get_self(), "scope2"_n, "atable"_n, 60, 24, data_0);
   check_lowerbound_i64(get_self(), "scope2"_n, "atable"_n, 59, 24, data_0);
   check_lowerbound_i64(get_self(), "scope2"_n, "atable"_n, 0xffff'ffff'ffff'fffe, 25, data_2);
   check_lowerbound_i64(get_self(), "scope2"_n, "atable"_n, 0xffff'ffff'ffff'fffd, 25, data_2);
   check_lowerbound_i64(get_self(), "scope2"_n, "atable"_n, 63, 26, data_3);

   /////////////////////////////////
   // check end
   // creates iterators -7, -8
   /////////////////////////////////

   check_end_i64(get_self(), "scope1"_n, "table1"_n, -2);
   check_end_i64(get_self(), "scope1"_n, "table2"_n, -3);
   check_end_i64(get_self(), "scope1"_n, "table3"_n, -4);
   check_end_i64(get_self(), "scope2"_n, "table1"_n, -5);
   check_end_i64(get_self(), "scope2"_n, "atable"_n, -6);
   check_end_i64(get_self(), "scope.x"_n, "table2"_n, -7); // not searched for yet
   check_end_i64(get_self(), "scope.x"_n, "table1"_n, -8); // not searched for yet
   check_end_i64(get_self(), "nope"_n, "nada"_n, -1);

   /////////////////////////////////
   // non-existing table
   /////////////////////////////////

   check_lowerbound_i64(get_self(), "nope"_n, "nada"_n, 25, -1, {});
   check_find_i64(get_self(), "nope"_n, "nada"_n, 25, -1, {});
   check_upperbound_i64(get_self(), "nope"_n, "nada"_n, 25, -1, {});
   check_end_i64(get_self(), "nope"_n, "nada"_n, -1);

   /////////////////////////////////
   // empty table
   // creates iterators -9
   /////////////////////////////////

   check_lowerbound_i64(get_self(), "nope"_n, "just.2nd"_n, 42, -9, {});
   check_find_i64(get_self(), "nope"_n, "just.2nd"_n, 42, -9, {});
   check_upperbound_i64(get_self(), "nope"_n, "just.2nd"_n, 42, -9, {});
   check_end_i64(get_self(), "nope"_n, "just.2nd"_n, -9);

   // TODO: verify secondaries doen't share itr indexes with each other or with primary
   // TODO: verify secondary order when it doesn't match primary order
   // TODO: duplicate secondaries

} // legacydb_contract::read()

EOSIO_ACTION_DISPATCHER(legacydb::actions)