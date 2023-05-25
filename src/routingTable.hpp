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
        int nextHop_;
        /* 次ポップデバイスの距離 */
        double distance_;
        /* エントリが有効かどうか */
        bool isValid_;

    public:
        Entry() = default;
        Entry(int nextHop, double distance = -1.0);

        int getNextHop() const;
        void updateEntry(int nextHop, double distance = -1.0);
        void markInvalid();
        bool isValid() const;
    };

    /* ルーティングテーブル */
    map<int, Entry> table_;

public:
    int getNextHop(int destination) const;
    vector<int> getDestinations() const;

    void updateEntry(int destination, int nextHop, double distance = -1.0);
    void markEntryInvalid(int destination);
};

/* ルーティングテーブルクラス */
using Table = RoutingTable;

class Packet
{
private:
    const int packet_id_;
    const size_t seq_;
    const int sender_id_;

    Table routingTable_; // 送信元デバイスのルーティングテーブル
};

#endif
