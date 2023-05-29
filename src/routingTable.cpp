/*!
 * @file olsr.cpp
 * @author tom96da
 * @brief RoutingTable クラスのソースファイル
 * @date 2023-05-25
 */

#include "routingTable.hpp"

/* 各デバイスのエントリクラス */

/*!
 * @brief コンストラクタ
 * @param nextHop_id 次ホップデバイスのID
 * @param distance 次ホップデバイスの距離
 */
RoutingTable::Entry::Entry(int nextHop_id, double distance)
    : nextHop_id_{nextHop_id},
      distance_{distance},
      isValid_{true}
{
}

/*!
 * @brief 次ポップデバイスのIDを取得
 * @return 次ホップデバイスのID
 */
int RoutingTable::Entry::getNextHop() const { return nextHop_id_; }

/*!
 * @brief エントリが無効か取得
 * @retval true 有効
 * @retval false 無効
 */
bool RoutingTable::Entry::isValid() const { return isValid_; }

/*!
 * @brief エントリの更新
 * @param nextHop_id 次のホップデバイスのID
 * @param distance 次ポップデバイスの距離
 */
void RoutingTable::Entry::updateEntry(int nextHop_id, double distance)
{
    nextHop_id_ = nextHop_id;
    distance_ = distance;
    isValid_ = true;
}

/*!
 * @brief エントリを無効にする
 */
void RoutingTable::Entry::markInvalid() { isValid_ = false; }

/* ルーティングテーブルクラス */

/*!
 * @brief 宛先に対する次ホップデバイスIDを取得
 * @param dest_id 宛先デバイスID
 * @return 次ホップデバイスID
 */
int RoutingTable::getNextHop(int dest_id) const
{
    auto it = table_.find(dest_id);
    if (it == table_.end())
        /* ルートが見つからない場合は-1を返す */
        return -1;

    return it->second.getNextHop();
}

/*!
 * @brief エントリ済みデバイスのIDを取得
 * @return エントリ済みデバイスのID
 */
vector<int> RoutingTable::getDestinations() const
{
    vector<int> destinations;
    for (const auto &entry : table_)
        destinations.push_back(entry.first);

    return destinations;
}

/*!
 * @brief エントリの更新
 * @param dest_id 宛先デバイスID
 * @param nextHop_id 次ホップデバイスのID
 * @param distance 次ホップデバイスの距離
 */
void RoutingTable::updateEntry(int dest_id, int nextHop_id, double distance)
{
    if (table_.count(dest_id))
        // 既にエントリが存在する場合は、更新する
        table_[dest_id].updateEntry(nextHop_id, distance);
    else
        // エントリが存在しない場合は、新たに作成する
        table_.emplace(dest_id, Entry(nextHop_id, distance));
}

/*!
 * @brief 宛先に対するエントリを無効にする
 * @param dest_id 宛先デバイスID
 */
void RoutingTable::markEntryInvalid(int dest_id)
{
    if (table_.count(dest_id))
        table_.at(dest_id).markInvalid();
}
