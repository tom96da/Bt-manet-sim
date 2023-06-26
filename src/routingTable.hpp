/*!
 * @file routingTable.hpp
 * @author tom96da
 * @brief RoutingTable クラスのヘッダファイル
 * @date 2023-05-25
 */

#ifndef ROUTINGTABLE_HPP
#define ROUTINGTABLE_HPP

#include <map>
#include <vector>
using namespace std;

/* ルーティングテーブルクラス */
class RoutingTable {
   private:
    /* 各デバイスのエントリクラス */
    class Entry;
    /* ルーティングテーブル */
    map<int, Entry> table_;

   public:
    RoutingTable();

    int getNumEntry() const;
    int getIdNextHop(const int id_dest) const;
    int getDistance(const int id_dest) const;

    map<int, Entry> &getTable();
    vector<int> getDestinations() const;

    bool hasEntry(const int id_dest) const;

    bool setEntry(const int id_dest, const int id_nextHop_,
                  const int distance = 0);
    void markEntryInvalid(const int id_dest);
    void clearEntryAll();
};

/* 各デバイスのエントリクラス */
class RoutingTable::Entry {
   private:
    /* 次のホップデバイスのID */
    int id_nextHop_;
    /* 宛先デバイスまでのホップ距離 */
    int distance_;
    /* エントリが有効かどうか */
    bool isValid_;

   public:
    Entry() = default;
    Entry(const int id_nextHop_, const int distance = 0);

    int getIdNextHop() const;
    int getDistance() const;
    bool isValid() const;

    void setEntry(const int id_nextHop_, const int distance = 0);
    void markInvalid();
};

/* ルーティングテーブルクラス */
using Table = RoutingTable;

#include "routingTable.cpp"

#endif  // ROUTINGTABLE_HPP
