/*!
 * @file olsr.cpp
 * @author tom96da
 * @brief RoutingTable クラスのソースファイル
 * @date 2023-05-25
 */

#include "routingTable.hpp"

/* ルーティングテーブルクラス */

/* コンストラクタ */
RoutingTable::RoutingTable() {}

/*!
 * @return 総エントリ数
 */
int RoutingTable::getNumEntry() const { return table_.size(); }

/*!
 * @param id_dest 宛先デバイスID
 * @return 次ホップデバイスID
 */
int RoutingTable::getIdNextHop(const int id_dest) const {
    if (!table_.count(id_dest)) {
        return -1;
    }

    return table_.at(id_dest).getIdNextHop();
}

/*!
 * @param id_dest 宛先デバイスID
 * @return 宛先デバイスまでのホップ距離
 */
int RoutingTable::getDistance(const int id_dest) const {
    if (!table_.count(id_dest)) {
        return -1;
    }

    return table_.at(id_dest).getDistance();
}

/*!
 * @return ルーティングテーブル
 */
map<int, Table::Entry> &RoutingTable::getTable() { return table_; }

/*!
 * @return エントリ済みデバイスのID
 */
vector<int> RoutingTable::getDestinations() const {
    vector<int> destinations;
    for (const auto &[id_dest, _] : table_) {
        destinations.push_back(id_dest);
    }

    return destinations;
}

/*!
 * @brief 宛先のエントリがあるかを取得
 * @param id_dest
 * @retval true 存在する
 * @retval false 存在しない
 */
bool RoutingTable::hasEntry(const int id_dest) const {
    return table_.count(id_dest);
}

/*!
 * @brief エントリの更新
 * @param id_dest 宛先デバイスID
 * @param id_nextHop 次ホップデバイスのID
 * @param distance 次ホップデバイスの距離
 * @retval true 更新あり
 * @retval false 更新無し
 */
bool RoutingTable::setEntry(const int id_dest, const int id_nextHop,
                            const int distance) {
    if (table_.count(id_dest)) {
        // 既にエントリが存在する場合は、距離が近ければ更新する
        if (table_.at(id_dest).getDistance() > distance) {
            table_[id_dest].setEntry(id_nextHop, distance);
        } else {
            return false;
        }
    } else {
        // エントリが存在しない場合は、新たに作成する
        table_.emplace(id_dest, Entry(id_nextHop, distance));
    }

    return true;
}

/*!
 * @brief 宛先に対するエントリを無効にする
 * @param id_dest 宛先デバイスID
 */
void RoutingTable::markEntryInvalid(const int id_dest) {
    if (table_.count(id_dest)) {
        table_.at(id_dest).markInvalid();
    }
}

/* 各デバイスのエントリクラス */

/*!
 * @brief コンストラクタ
 * @param id_nextHop 次ホップデバイスのID
 * @param distance 次ホップデバイスの距離
 */
RoutingTable::Entry::Entry(const int id_nextHop, const int distance)
    : id_nextHop_{id_nextHop}, distance_{distance}, isValid_{true} {}

/*!
 * @return 次ホップデバイスのID
 */
int RoutingTable::Entry::getIdNextHop() const { return id_nextHop_; }

/*!
 * @return マルチホップ距離
 */
int RoutingTable::Entry::getDistance() const { return distance_; }

/*!
 * @brief エントリが無効か取得
 * @retval true 有効
 * @retval false 無効
 */
bool RoutingTable::Entry::isValid() const { return isValid_; }

/*!
 * @brief エントリの更新
 * @param id_nextHop 次のホップデバイスのID
 * @param distance 次ポップデバイスの距離
 */
void RoutingTable::Entry::setEntry(const int id_nextHop, const int distance) {
    id_nextHop_ = id_nextHop;
    distance_ = distance;
    isValid_ = true;
}

/*!
 * @brief エントリを無効にする
 */
void RoutingTable::Entry::markInvalid() { isValid_ = false; }
