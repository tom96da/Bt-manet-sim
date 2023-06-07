#include "Device.hpp"
#include <iostream>
#include <string>
#include <algorithm>

/* Bluetooth デバイスクラス */

/*!
 * @brief コンストラクタ
 * @param id デバイスID
 * @param max_connections 最大接続数 デフォルト値: 6
 */
Device::Device(int id, int max_connections)
    : id_(id),
      max_connections_(max_connections),
      num_packet_made_{0}
{
}

/*!
 * @return デバイスID
 */
int Device::getId() const { return id_; }

/*!
 * @return デバイス名
 */
string Device::getName() const { return "device[" + to_string(getId()) + "]"; }

/*!
 * @return ペアリング済みデバイス数
 */
int Device::getNumPaired() const { return paired_devices_.size(); }

/*!
 * @return 接続中のデバイス数
 */
int Device::getNumConnected() const { return connected_devices_.size(); }

/*!
 * @return ペアリング済みデバイスのIDリスト
 */
set<int> Device::getPairedDeviceId() const
{
    set<int> paired_devices_id;
    for (const auto paired_device : paired_devices_)
        paired_devices_id.emplace(paired_device.first);

    return paired_devices_id;
}

/*!
 * @return 接続中デバイスのIDリスト
 */
set<int> Device::getConnectedDeviceId() const
{
    return connected_devices_;
}

/*!
 * @return 累計パケット生成数
 */
int Device::getNumPacket() const { return num_packet_made_; }

/*!
 * @return 新規パケットのID
 */
int Device::getNewPacketId() const { return num_packet_made_++; }

Device::Sell &Device::getSellData(const size_t data_id)
{
    return memory_.at(data_id);
}

/*!
 * @brief メモリからデータを取得
 * @param data_id データ識別子
 * @return 該当データ
 */
pair<size_t, Var> Device::loadData(const size_t data_id) const
{

    return memory_.at(data_id).getData();
}

/*!
 * @brief デバイスがペアリング登録済みか取得
 * @param another_device_id 対象デバイスのID
 * @retval true 登録済み
 * @retval false 未登録
 */
bool Device::isPaired(const int another_device_id) const
{
    return paired_devices_.count(another_device_id);
}

/*!
 * @brief デバイスが接続中か取得
 * @param another_device_id 対象デバイスのID
 * @retval true 接続中
 * @retval false 未接続
 */
bool Device::isConnected(const int another_device_id) const
{
    return connected_devices_.count(another_device_id);
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

    this->paired_devices_.emplace(another_device.getId(), &another_device);
}

/*!
 * @brief デバイスのペアリング解除
 * @param another_device_id 相手のデバイスID
 */
void Device::unpairing(const int another_device_id)
{
    if (!this->isPaired(another_device_id))
        return;

    this->disconnect(another_device_id);
    this->paired_devices_.erase(another_device_id);
}

/*!
 * @brief 登録済みのデバイスと接続する
 * @param another_device 接続相手のデバイス
 * @retval true 接続成功
 * @retval false 接続失敗
 */
bool Device::connect(const int another_device_id)
{
    if (this->isConnected(another_device_id))
        return false;
    if (getNumConnected() >= max_connections_)
        return false;

    this->connected_devices_.emplace(another_device_id);
    return true;
}

/*!
 * @brief 接続中デバイスの切断
 * @param another_device
 */
void Device::disconnect(const int another_device_id)
{
    if (!isConnected(another_device_id))
        return;

    this->connected_devices_.erase(another_device_id);
}

/*!
 * @brief データを保存する
 * @param idata 識別子付きデータ
 * @param flood_flag フラッディングフラグ
 * @param flood_step フラッディングホップ数
 */
void Device::saveData(pair<size_t, Var> idata,
                      bool flood_flag, int flood_step)
{
    size_t data_id = idata.first;
    if (memory_.count(data_id))
        return;

    memory_.emplace(data_id, Sell(getId(), idata,
                                  flood_flag, flood_step));
}

/*!
 * @brief パケットを保存する
 * @param packet パケット
 */
void Device::saveData(const Packet &packet)
{
    pair<size_t, Var> idata = packet.getData();
    size_t data_id = idata.first;
    if (memory_.count(data_id))
        return;

    memory_.emplace(data_id, Sell(packet.getSenderId(), idata,
                                  packet.isFloodFlag(), packet.getFloodStep()));
}

/*!
 * @brief 接続中のデバイスに文字列を送信する
 * @param receiver_id 送信先デバイスのID
 * @param message 送信する文字列
 */
void Device::sendMessage(const int receiver_id, string message)
{
    if (!isConnected(receiver_id))
        return;

    std::cout << "ID_" << this->getId() << " -> " << flush;
    getPairedDevice(receiver_id).receiveMessage(this->getId(), message);
}

/*!
 * @brief 接続中のデバイスから文字列を受信する (保存しない)
 * @param sender_id 送信元デバイスのID
 * @param message 受信する文字列
 */
void Device::receiveMessage(const int sender_id, string message)
{
    std::cout << message << " -> ID_" << this->getId() << endl;
}

/*!
 * @brief データからパケットを生成する
 * @param data 送信するデータ
 * @return 生成したパケット
 */
Packet Device::makePacket(Var data) const
{
    return Packet(getId(), getNewPacketId(),
                  pcnt::getNewSequenceNum(), assignIdToData(data));
}

/*!
 * @brief 識別子付きデータパケットを生成する
 * @param idata 送信する識別子付きデータ
 * @param flood_flag フラッディングフラグ
 * @param flood_step フラッディングホップ数
 * @return 生成したパケット
 */
Packet Device::makePacket(pair<size_t, Var> idata,
                          const bool flood_flag, const int flood_step) const
{
    return Packet(getId(), getNewPacketId(), pcnt::getNewSequenceNum(),
                  idata, flood_flag, flood_step);
}

/*!
 * @brief 接続中のデバイスにパケットを送信する
 * @param receiver_id 送信先デバイスのID
 * @param packet 送信するパケット
 */
void Device::sendPacket(const int receiver_id, const Packet &packet)
{
    if (isConnected(receiver_id))
        getPairedDevice(receiver_id).receivePacket(packet);
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
 * @brief フラッディングするデータを作成
 * @return 作成したデータの識別子
 */
size_t Device::makeFloodData()
{
    pair<size_t, Var> idata = assignIdToData(static_cast<string>("hello!"), true);
    saveData(idata, true, 0);

    return idata.first;
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

    auto itr = find_if(memory_.rbegin(), memory_.rend(),
                       [](pair<size_t, Device::Sell> sell)
                       {
                           return sell.second.isFloodFlag();
                       });
    if (itr == memory_.rend())
        return;

    auto &data_in_sell = itr->second;
    if (data_in_sell.getFloodStep() > _step_new)
        return;

    for (auto cntd : getConnectedDeviceId())
        if (cntd != data_in_sell.getSenderId())
            sendPacket(cntd, makePacket(data_in_sell.getData(), true, _step_new + 1));

    data_in_sell.markFlagInvalid();
}

/*!
 * @brief 接続中のデバイスにhello!を送信
 */
void Device::sendHello()
{
    for (auto cntd : getConnectedDeviceId())
        sendPacket(cntd, makePacket(static_cast<string>("hello!")));
}

/*!
 * @brief ペアリング済みのデバイスの参照を取得
 * @param id 対象デバイスのID
 * @return デバイスオブジェクトの参照
 */
Device &Device::getPairedDevice(const int id)
{
    return *paired_devices_.at(id);
}

/*!
 * @brief デバイスが自身かどうか取得
 * @param another_device_id 対象デバイスのID
 * @retval true 自身
 * @retval false 自身でない
 */
bool Device::isSelf(const int another_device_id) const
{
    return this->getId() == another_device_id;
}

/*!
 * @brief データにデータ識別子を付与する
 * @param data データ
 * @return 識別子が付与されたデータ
 */
pair<size_t, Var> Device::assignIdToData(const Var data,
                                         const bool flood_flag,
                                         size_t data_id) const
{
    if (data_id == 0)
    {
        string s_id = to_string(getId()),
               s_packet = to_string(getNumPacket());
        if (!flood_flag)
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
 * @param sender_id 送信元デバイスのID
 * @param data 送信データ
 * @param flood_flag フラッディングフラグ デフォルト値: false
 * @param flood_step ホップ数 デフォルト値: 0
 */
Device::Sell::Sell(const int sender_id, const pair<size_t, Var> idata,
                   const bool flood_flag, const int flood_step)
    : sender_id_{sender_id},
      idata_{idata},
      flood_step_{flood_step},
      flood_flag_{flood_flag}
{
}

/*!
 * @return 送信元デバイスのID
 */
int Device::Sell::getSenderId() const { return sender_id_; }

/*!
 * @return データ識別子
 */
size_t Device::Sell::getDataId() const { return idata_.first; }

/*!
 * @return 識別子付きデータ
 */
pair<size_t, Var> Device::Sell::getData() const { return idata_; }

/*!
 * @return フラッディングホップ数
 */
int Device::Sell::getFloodStep() const { return flood_step_; }

/*!
 * @return フラッディングフラグ
 */
bool Device::Sell::isFloodFlag() const { return flood_flag_; }

/*!
 * @brief フラッディングフラグを立てる
 */
void Device::Sell::markFlagValid() { flood_flag_ = true; }

/*!
 * @brief フラッディングフラグを下ろす
 */
void Device::Sell::markFlagInvalid() { flood_flag_ = false; }

/* パケットクラス */

/*!
 * @brief コンストラクタ
 * @param sender_id 送信元デバイスのID
 * @param packet_id パケットID
 * @param seq_num シーケンスナンバー
 * @param data 送信データ
 * @param flood_flag フラッディングフラグ デフォルト値: false
 */
Packet::Packet(const int sender_id, const int packet_id,
               const int seq_num, const pair<size_t, Var> data,
               const bool flood_flag, const int flood_step)
    : sender_id_{sender_id},
      packet_id_{packet_id},
      seq_num_{seq_num},
      idata_{data},
      flood_flag_{flood_flag},
      flood_step_{flood_step}
{
}

/*!
 * @return 送信元デバイスのID
 */
int Packet::getSenderId() const { return sender_id_; }

/*!
 * @return パケットID
 */
int Packet::getPacketId() const { return packet_id_; }

/*!
 * @return 送信元デバイスのID
 */
int Packet::getSeqNum() const { return seq_num_; }

/*!
 * @return 送信データ
 */
pair<size_t, Var> Packet::getData() const { return idata_; }

/*!
 * @return フラッディングフラグ
 */
bool Packet::isFloodFlag() const { return flood_flag_; }

/*!
 * @return フラッディングステップ数
 */
int Packet::getFloodStep() const { return flood_step_; }

/* パケットカウンタ */
namespace PacketCounter
{
    int num_total_packe = 0;

    /*!
     * @return 累計パケット数
     */
    int getTotalPacket() { return num_total_packe; }

    /*!
     * @return 新規パケットID
     */
    int getNewSequenceNum() { return num_total_packe++; }
}
