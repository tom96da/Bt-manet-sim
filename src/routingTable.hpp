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
class RoutingTable
{
private:
    /* 各デバイスのエントリクラス */
    class Entry;
    /* ルーティングテーブル */
    map<int, Entry> table_;

public:
    RoutingTable();

    int getNextHop(const int dest_id) const;
    vector<int> getDestinations() const;

    bool hasEntry(const int dest_id) const;

    void setEntry(const int dest_id, const int nextHop_id,
                  const double distance = -1.0);
    void markEntryInvalid(const int dest_id);
};

/* 各デバイスのエントリクラス */
class RoutingTable::Entry
{
private:
    /* 次のホップデバイスのID */
    int nextHop_id_;
    /* 次ポップデバイスの距離 */
    double distance_;
    /* エントリが有効かどうか */
    bool isValid_;

public:
    Entry() = default;
    Entry(const int nextHop_id, const double distance = -1.0);

    int getNextHop() const;
    bool isValid() const;

    void setEntry(const int nextHop_id, const double distance = -1.0);
    void markInvalid();
};

/* ルーティングテーブルクラス */
using Table = RoutingTable;

#include "routingTable.cpp"

#endif // ROUTINGTABLE_HPP
