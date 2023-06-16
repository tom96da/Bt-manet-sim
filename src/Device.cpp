#include "Device.hpp"
#include <iostream>
#include <string>
#include <algorithm>

/* Bluetooth デバイスクラス */

/*!
 * @brief コンストラクタ
 * @param id デバイスID
 * @param willingness willingness デフォルト値: 3
 */
Device::Device(const int id, const int willingness)
    : id_(id),
      max_connections_(MAX_CONNECTIONS),
      willingness_{willingness},
      num_packet_made_{0}
{
}

/* 累計パケット数 */
int Device::num_total_packe_ = 0;

/*!
 * @return int 累計パケット数
 */
int Device::getTotalPacket() { return num_total_packe_; }

/*!
 * @return int 新規パケットID
 */
int Device::getNewSequenceNum() { return num_total_packe_++; }

/*!
 * @brief 累計パケット数の出力
 */
void Device::showTotalPacket()
{
    std::cout << "total packet: " << getTotalPacket() << std::endl;
}

/*!
 * @return int デバイスID
 */
int Device::getId() const { return id_; }

/*!
 * @return string デバイス名
 */
string Device::getName() const
{
    return "device[" + string(2 - to_string(getId()).length(), '0') +
           to_string(getId()) + "]";
}

/*!
 * @return int willingness
 */
int Device::getWillingness() const { return willingness_; }

/*!
 * @return int ペアリング済みデバイス数
 */
int Device::getNumPaired() const { return paired_devices_.size(); }

/*!
 * @return int 接続中のデバイス数
 */
int Device::getNumConnected() const { return id_connected_devices_.size(); }

/*!
 * @return set<int> ペアリング済みデバイスのIDリスト
 */
set<int> Device::getPairedDeviceId() const
{
    set<int> paired_devices_id;
    for (const auto &paired_device : paired_devices_)
        paired_devices_id.emplace(paired_device.first);

    return paired_devices_id;
}

/*!
 * @return set<int> 接続中デバイスのIDリスト
 */
set<int> Device::getConnectedDevicesId() const
{
    return id_connected_devices_;
}

/*!
 * @return RoutingTable ルーティングテーブル
 */
Table Device::getTable() const
{
    return table_;
}

/*!
 * @return int 累計パケット生成数
 */
int Device::getNumPacket() const { return num_packet_made_; }

/*!
 * @return int 新規パケットのID
 */
int Device::getNewPacketId() const { return num_packet_made_++; }

/*!
 * @return set<int> MPR集合
 */
set<int> Device::getMPR() const { return MPR_; }

/*!
 * @brief メモリからデータを取得
 * @param data_id データ識別子
 * @return pair<size_t, variant> 該当データ
 */
pair<size_t, Var> Device::loadData(const size_t data_id) const
{

    return memory_.at(data_id).getDataWithId();
}

/*!
 * @brief デバイスがペアリング登録済みか取得
 * @param id_another_device 対象デバイスのID
 * @retval true 登録済み
 * @retval false 未登録
 */
bool Device::isPaired(const int id_another_device) const
{
    return paired_devices_.count(id_another_device);
}

/*!
 * @brief デバイスが接続中か取得
 * @param id_another_device 対象デバイスのID
 * @retval true 接続中
 * @retval false 未接続
 */
bool Device::isConnected(const int id_another_device) const
{
    return id_connected_devices_.count(id_another_device);
}

/*!
 * @brief メモリーにデータを持っているか取得
 * @param data_id データ識別子
 * @retval true 保持
 * @retval false 未保持
 */
bool Device::hasData(const size_t data_id) const
{
    return memory_.count(data_id);
}

/*!
 * @brief デバイスをペアリング登録
 * @param another_device 対象デバイスのオブジェクト
 */
void Device::pairing(Device &another_device)
{
    if (this->isPaired(another_device.getId()))
        return;

    this->paired_devices_.emplace(another_device.getId(), another_device);
}

/*!
 * @brief デバイスのペアリング解除
 * @param id_another_device 相手のデバイスID
 */
void Device::unpairing(const int id_another_device)
{
    if (!isPaired(id_another_device))
        return;

    disconnect(id_another_device);
    paired_devices_.erase(id_another_device);
}

/*!
 * @brief 登録済みのデバイスと接続する
 * @param id_another_device 接続相手のデバイス
 * @retval true 接続成功
 * @retval false 接続失敗
 */
bool Device::connect(const int id_another_device)
{
    if (isConnected(id_another_device))
        return false;
    if (getNumConnected() >= max_connections_)
        return false;

    id_connected_devices_.emplace(id_another_device);
    return true;
}

/*!
 * @brief 接続中デバイスの切断
 * @param id_another_device
 */
void Device::disconnect(const int id_another_device)
{
    if (!isConnected(id_another_device))
        return;

    id_connected_devices_.erase(id_another_device);
}

/*!
 * @brief データを保存する
 * @param data_with_id 識別子付きデータ
 * @param data_attr データ属性
 * @param flood_step フラッディングホップ数
 */
void Device::saveData(pair<size_t, Var> data_with_id,
                      const DataAttr data_attr, int flood_step)
{
    size_t data_id = data_with_id.first;
    if (memory_.count(data_id))
        return;

    memory_.emplace(data_id, Sell(getId(), data_with_id, data_attr, flood_step));
}

/*!
 * @brief パケットを保存する
 * @param packet パケット
 */
void Device::saveData(const Packet &packet)
{
    pair<size_t, Var> data_with_id = packet.getDataWithId();
    size_t data_id = data_with_id.first;
    if (memory_.count(data_id))
        return;

    memory_.emplace(data_id,
                    Sell(packet.getSenderId(), data_with_id, packet.getDataAttribute(),
                         packet.getFloodStep()));
}

/*!
 * @brief 接続中のデバイスに文字列を送信する
 * @param id_receiver 送信先デバイスのID
 * @param message 送信する文字列
 */
void Device::sendMessage(const int id_receiver, string message)
{
    if (!isConnected(id_receiver))
        return;

    std::cout << "ID_" << this->getId() << " -> " << flush;
    getPairedDevice(id_receiver).receiveMessage(this->getId(), message);
}

/*!
 * @brief 接続中のデバイスから文字列を受信する (保存しない)
 * @param id_sender 送信元デバイスのID
 * @param message 受信する文字列
 */
void Device::receiveMessage(const int id_sender, string message)
{
    std::cout << message << " -> ID_" << this->getId() << endl;
}

/*!
 * @brief データからパケットを生成する
 * @param data 送信するデータ
 * @return Packet 生成したパケット
 */
Device::Packet Device::makePacket(Var data) const
{
    return Packet(getId(), getNewPacketId(), getNewSequenceNum(),
                  assignIdToData(data), DataAttr::NONE);
}

/*!
 * @brief 識別子付きデータパケットを生成する
 * @param data_with_id 送信する識別子付きデータ
 * @param data_attr データ属性
 * @param flood_step ホップ数 デフォルト値: 0
 * @return Packet 生成したパケット
 */
Device::Packet Device::makePacket(pair<size_t, Var> data_with_id,
                                  const DataAttr data_attr, const int flood_step) const
{
    return Packet(getId(), getNewPacketId(), getNewSequenceNum(),
                  data_with_id, data_attr, flood_step);
}

/*!
 * @brief 接続中のデバイスにパケットを送信する
 * @param id_receiver 送信先デバイスのID
 * @param packet 送信するパケット
 */
void Device::sendPacket(const int id_receiver, const Packet &packet)
{
    if (isConnected(id_receiver))
        getPairedDevice(id_receiver).receivePacket(packet);
}

/*!
 * @brief 接続中のデバイスからパケットを受信する
 * @param packet 受信するパケット
 */
void Device::receivePacket(const Packet &packet)
{
    saveData(packet);
}

/*!
 * @brief 接続中のデバイスにhello!を送信
 */
void Device::sendHello()
{
    auto id_cntds = getConnectedDevicesId();

    for (auto &id_cntd : id_cntds)
    {
        sendPacket(id_cntd, makePacket(assignIdToData(getWillingness()),
                                       DataAttr::WILLINGNESS));
        sendPacket(id_cntd, makePacket(assignIdToData(id_cntds),
                                       DataAttr::TOPOLOGY));
    }
}

/*!
 * @brief 接続中のデバイスにルーティングテーブルを送信
 */
void Device::sendTable()
{
    for (auto &id_cntd : getConnectedDevicesId())
    {
        sendPacket(id_cntd, makePacket(assignIdToData(getTable()), DataAttr::TABLE));
    }
}

/*!
 * @brief フラッディングするデータを作成
 * @return size_t 作成したデータの識別子
 */
size_t Device::makeFloodData()
{
    pair<size_t, Var> data_with_id =
        assignIdToData(static_cast<string>("hello!"), true);
    saveData(data_with_id, DataAttr::FLOODING, 0);

    return data_with_id.first;
}

/*!
 * @brief フラッディングを行う
 * @param flag 制御フラグ 0 -ホップ, 1 -次ステップへ, -1 -初期化,
 * デフォルト値: 0
 */
void Device::flooding(const int flag)
{
    /* 現在のホップ数 */
    static int _step_new = 0;
    if (flag == 1)
    {
        /* flag = 1 なら次のホップへ */
        _step_new++;
        return;
    }
    if (flag == -1)
    {
        /* flag = -1 なら初期化 */
        _step_new = 0;
        return;
    }

    auto itr =
        find_if(memory_.rbegin(), memory_.rend(),
                [](pair<size_t, Device::Sell> sell)
                { return sell.second.getDataAttribute() == DataAttr::FLOODING; });
    if (itr == memory_.rend())
        return;

    auto &data_in_sell = itr->second;
    if (data_in_sell.getFloodStep() > _step_new)
        return;

    for (auto id_cntd : getConnectedDevicesId())
        if (id_cntd != data_in_sell.getSenderId())
            sendPacket(id_cntd,
                       makePacket(data_in_sell.getDataWithId(), DataAttr::FLOODING,
                                  _step_new + 1));

    data_in_sell.setDaTaAttribute(DataAttr::NONE);
}

/*!
 * @brief MPR集合を作成する
 */
void Device::makeMPR()
{
    MPR_.clear();
    /* 隣接 <willingness, id> (降順) */
    multimap<int, int, greater<int>> neighbors;
    /* 2ホップ隣接 <tow hop neighbor, MPR> */
    map<int, int> tow_hop_neighbors;

    while (true)
    {
        auto itr =
            find_if(memory_.rbegin(), memory_.rend(),
                    [](pair<size_t, Device::Sell> sell)
                    { return sell.second.getDataAttribute() == DataAttr::WILLINGNESS; });
        if (itr == memory_.rend())
            break;

        auto &data_in_sell = itr->second;
        auto id_cntd = data_in_sell.getSenderId();
        auto willingness = get<int>(data_in_sell.getData());

        neighbors.emplace(willingness, id_cntd);
        table_.setEntry(id_cntd, id_cntd, 1);

        data_in_sell.setDaTaAttribute(DataAttr::NONE);
    }

    for (auto &neighbor : neighbors)
    {
        auto id_neighbor = neighbor.second;

        auto itr =
            find_if(memory_.rbegin(), memory_.rend(),
                    [&](pair<size_t, Device::Sell> sell)
                    {
                        auto is_topology =
                            sell.second.getDataAttribute() == DataAttr::TOPOLOGY;
                        auto is_id = sell.second.getSenderId() == id_neighbor;
                        return is_topology & is_id;
                    });
        if (itr == memory_.rend())
            break;

        auto &data_in_sell = itr->second;
        auto neighbor_cntds = get<set<int>>(data_in_sell.getData());
        for (auto id_tow_hop_neighbor : neighbor_cntds)
        {
            if (isSelf(id_tow_hop_neighbor))
                continue;
            if (isConnected(id_tow_hop_neighbor))
                continue;
            if (tow_hop_neighbors.count(id_tow_hop_neighbor))
                continue;

            tow_hop_neighbors.emplace(id_tow_hop_neighbor, id_neighbor);
            table_.setEntry(id_tow_hop_neighbor, id_neighbor, 2);
        }
    }

    for (auto &two_hop_neighbor : tow_hop_neighbors)
        MPR_.emplace(two_hop_neighbor.second);
}

/*!
 * @brief ルーティングテーブルを作成する
 */
void Device::makeTable()
{
    while (true)
    {
        auto itr =
            find_if(memory_.rbegin(), memory_.rend(),
                    [](pair<size_t, Device::Sell> sell)
                    { return sell.second.getDataAttribute() == DataAttr::TABLE; });
        if (itr == memory_.rend())
            break;

        auto &data_in_sell = itr->second;
        auto id_sender = data_in_sell.getSenderId();
        auto &&table_neighbor = get<Table>(data_in_sell.getData()).getTable();

        for (auto &entry : table_neighbor)
        {
            auto id_dest = entry.first;
            auto nexthop = id_sender;
            auto distance = entry.second.getDistance() + 1;

            if (id_dest != getId())
                table_.setEntry(id_dest, nexthop, distance);
        }

        data_in_sell.setDaTaAttribute(DataAttr::NONE);
    }
}

/*!
 * @brief ペアリング済みのデバイスの参照を取得
 * @param id 対象デバイスのID
 * @return Device デバイスオブジェクトの参照
 */
Device &Device::getPairedDevice(const int id)
{
    return paired_devices_.at(id);
}

/*!
 * @brief デバイスが自身かどうか取得
 * @param id_another_device 対象デバイスのID
 * @retval true 自身
 * @retval false 自身でない
 */
bool Device::isSelf(const int id_another_device) const
{
    return this->getId() == id_another_device;
}

/*!
 * @brief データにデータ識別子を付与する
 * @param data データ
 * @return pair<size_t, variant> 識別子付きデータ
 */
pair<size_t, Var> Device::assignIdToData(const Var data,
                                         const bool is_flooding,
                                         size_t data_id) const
{
    if (data_id == 0)
    {
        string s_id = to_string(getId()),
               s_packet = to_string(getNumPacket());
        if (!is_flooding)
            data_id = stoul(
                "2" + string(3 - s_id.length(), '0') + s_id +
                string(6 - s_packet.length(), '0') + s_packet);
        else
            data_id = stoul(
                "3" + string(3 - s_id.length(), '0') + s_id +
                string(6 - s_packet.length(), '0') + s_packet);
    }

    return {data_id, data};
}

/* メモリセルクラス */

/*!
 * @brief コンストラクタ
 * @param id_sender 送信元デバイスのID
 * @param data 送信データ
 * @param data_attr データ属性
 * @param flood_step ホップ数 デフォルト値: 0
 */
Device::Sell::Sell(const int id_sender, const pair<size_t, Var> data_with_id,
                   const DataAttr data_attr, const int flood_step)
    : id_sender_{id_sender},
      data_with_id_{data_with_id},
      data_attr_{data_attr},
      flood_step_{flood_step}
{
}

/*!
 * @return int 送信元デバイスのID
 */
int Device::Sell::getSenderId() const { return id_sender_; }

/*!
 * @return size_t データ識別子
 */
size_t Device::Sell::getDataId() const { return data_with_id_.first; }

/*!
 * @return variant 識別子無しデータ
 */
Var Device::Sell::getData() const { return data_with_id_.second; }

/*!
 * @return pair<size_t, variant> 識別子付きデータ
 */
pair<size_t, Var> Device::Sell::getDataWithId() const { return data_with_id_; }

/*!
 * @return Device::DataAttr データ属性
 */
Device::DataAttr Device::Sell::getDataAttribute() const { return data_attr_; }

/*!
 * @return int フラッディングホップ数
 */
int Device::Sell::getFloodStep() const { return flood_step_; }

/*!
 * @brief データ属性を変更する
 * @param data_attr 与えるデータ属性
 */
void Device::Sell::setDaTaAttribute(const DataAttr data_attr)
{
    data_attr_ = data_attr;
}

/* パケットクラス */

/*!
 * @brief コンストラクタ
 * @param id_sender 送信元デバイスのID
 * @param packet_id パケットID
 * @param seq_num シーケンスナンバー
 * @param data 送信データ
 * @param data_attr データ属性
 * @param flood_step ホップ数 デフォルト値: 0
 */
Device::Packet::Packet(const int id_sender, const int packet_id,
                       const int seq_num, const pair<size_t, Var> data,
                       const DataAttr data_attr, const int flood_step)
    : id_sender_{id_sender},
      packet_id_{packet_id},
      seq_num_{seq_num},
      data_with_id_{data},
      data_attr_{data_attr},
      flood_step_{flood_step}
{
}

/*!
 * @return int 送信元デバイスのID
 */
int Device::Packet::getSenderId() const { return id_sender_; }

/*!
 * @return int パケットID
 */
int Device::Packet::getPacketId() const { return packet_id_; }

/*!
 * @return int 送信元デバイスのID
 */
int Device::Packet::getSeqNum() const { return seq_num_; }

/*!
 * @return pair<size_t, variant> 識別子付き送信データ
 */
pair<size_t, Var> Device::Packet::getDataWithId() const { return data_with_id_; }

/*!
 * @return Device::DataAttr データ属性
 */
Device::DataAttr Device::Packet::getDataAttribute() const { return data_attr_; }

/*!
 * @return int フラッディングステップ数
 */
int Device::Packet::getFloodStep() const { return flood_step_; }
