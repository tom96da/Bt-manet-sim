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
    class Entry
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
        Entry(int nextHop_id, double distance = -1.0);

        int getNextHop() const;
        bool isValid() const;

        void updateEntry(int nextHop_id, double distance = -1.0);
        void markInvalid();
    };

    /* ルーティングテーブル */
    map<int, Entry> table_;

public:
    int getNextHop(int dest_id) const;
    vector<int> getDestinations() const;

    void updateEntry(int dest_id, int nextHop_id, double distance = -1.0);
    void markEntryInvalid(int dest_id);
};

/* ルーティングテーブルクラス */
using Table = RoutingTable;

#endif
