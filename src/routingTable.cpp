/*!
 * @file olsr.cpp
 * @author tom96da
 * @brief RoutingTable クラスのソースファイル
 * @date 2023-05-25
 */

#include "routingTable.hpp"

/**
 *  RoutingTable::Entry
 */

/*!
 * @brief コンストラクタ
 * @param nextHop 次ホップデバイスのID
 * @param nextHop 次ホップデバイスの距離
 */
RoutingTable::Entry::Entry(int nextHop, double distance)
    : nextHop_{nextHop},
      distance_{distance},
      isValid_{true}
{
}

/*!
 * @brief 次ポップデバイスのIDを取得
 * @return 次ホップデバイスのID
 */
int RoutingTable::Entry::getNextHop() const { return nextHop_; }

/*!
 * @brief エントリの更新
 * @param nextHop 次のホップデバイスのID
 * @param distance 次ポップデバイスの距離
 */
void RoutingTable::Entry::updateEntry(int nextHop, double distance)
{
    nextHop_ = nextHop;
    distance_ = distance;
    isValid_ = true;
}

/*!
 * @brief エントリを無効にする
 */
void RoutingTable::Entry::markInvalid() { isValid_ = false; }

/*!
 * @brief エントリが無効か取得
 * @retval true 有効
 * @retval false 無効
 */
bool RoutingTable::Entry::isValid() const { return isValid_; }

/**
 *  RoutingTable
 */

/*!
 * @brief 宛先に対する次ホップデバイスIDを取得
 * @param destination 宛先デバイスID
 * @return 次ホップデバイスID
 */
int RoutingTable::getNextHop(int destination) const
{
    auto it = table_.find(destination);
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
 * @param destination 宛先デバイスID
 * @param nextHop 次ホップデバイスのID
 * @param distance 次ホップデバイスの距離
 */
void RoutingTable::updateEntry(int destination, int nextHop, double distance)
{
    if (table_.count(destination))
        // 既にエントリが存在する場合は、更新する
        table_[destination].updateEntry(nextHop, distance);
    else
        // エントリが存在しない場合は、新たに作成する
        table_.emplace(destination, Entry(nextHop, distance));
}

/*!
 * @brief 宛先に対するエントリを無効にする
 * @param destination
 */
void RoutingTable::markEntryInvalid(int destination)
{
    if (table_.count(destination))
        table_.at(destination).markInvalid();
}
