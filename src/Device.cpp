/*!
 * @file Device.cpp
 * @author tom96da
 * @brief Device クラスのメンバ関数
 * @date 2023-05-11
 */

#include <iostream>
#include "Device.hpp"

/* パケットクラス */

/*!
 * @brief コンストラクタ
 * @tparam T データの型
 * @param sender_id 送信元デバイスのID
 * @param packet_id パケットID
 * @param data 送信データ
 */
template <typename T>
Packet<T>::Packet(const int sender_id, const int packet_id, size_t seq_num, T data)
    : sender_id_{sender_id},
      packet_id_{packet_id},
      seq_num_{seq_num},
      data_{data}
{
}

/*!
 * @return 送信元デバイスのID
 */
template <typename T>
int Packet<T>::getSenderId() const { return sender_id_; }

/*!
 * @return パケットID
 */
template <typename T>
int Packet<T>::getPacketId() const { return packet_id_; }

/*!
 * @return 送信元デバイスのID
 */
template <typename T>
size_t Packet<T>::getSeqNum() const { return seq_num_; }

/*!
 * @tparam T 送信するデータの型
 * @return 送信するデータ
 */
template <typename T>
T Packet<T>::getData() const { return data_; }

/* Bluetooth デバイスクラス */

/*!
 * @brief コンストラクタ
 * @param id デバイスID
 * @param max_connections 最大接続数 デフォルト値: 6
 */
Device::Device(const int id, int max_connections)
    : id_{id},
      max_connections_{max_connections},
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
int Device::getNewPacketId() { return num_packet_made_++; }

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
 * @brief デバイスをペアリング登録
 * @param another_device 対象デバイスのオブジェクト
 */
void Device::pairing(Device &another_device)
{
    if (this->isSelf(another_device.getId()))
        return;
    if (this->isPaired(another_device.getId()))
        return;

    this->paired_devices_.emplace(another_device.getId(), &another_device);
    another_device.pairing(*this);
}

/*!
 * @brief デバイスのペアリング解除
 * @param another_device_id 相手のデバイスID
 */
void Device::removePairing(const int another_device_id)
{
    if (!this->isPaired(another_device_id))
        return;

    this->disconnect(another_device_id);
    this->paired_devices_.erase(another_device_id);
    getPairedDevice(another_device_id)->removePairing(this->getId());
}

/*!
 * @brief 登録済みのデバイスと接続する
 * @param another_device 接続相手のデバイス
 */
void Device::connect(const int another_device_id)
{
    if (!this->isPaired(another_device_id))
        return;
    if (this->isConnected(another_device_id))
        return;
    if (getNumConnected() >= max_connections_)
        return;

    this->connected_devices_.emplace(another_device_id);
    getPairedDevice(another_device_id)->connect(this->getId());
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
    getPairedDevice(another_device_id)->disconnect(this->getId());
}

/*!
 * @brief 接続中のデバイスに文字列を送信する
 * @param receiver_id 送信先デバイスのID
 * @param message 送信する文字列
 */
void Device::sendMessage(const int receiver_id, string message)
{
    cout << "ID_" << this->getId() << " -> " << flush;
    getPairedDevice(receiver_id)->receiveMessage(this->getId(), message);
}

/*!
 * @brief 接続中のデバイスから文字列を受信する
 * @param sender_id 送信元デバイスのID
 * @param message 受信する文字列
 */
void Device::receiveMessage(const int sender_id, string message)
{
    cout << message << " -> ID_" << this->getId() << endl;
    memory_.emplace_back(message);
}

/*!
 * @brief 接続中のデバイスにパケットを送信する
 * @tparam T パケットが含むデータの型
 * @param receiver_id 送信先デバイスのID
 * @param packet 送信するパケット
 */
// template <typename T>
// void Device::sendPacket(const int receiver_id, const Packet<T> packet)
// {
//     getPairedDevice(receiver_id)->receivePacket(this->getId(), packet);
// }
// void Device::sendPacket(const int receiver_id, const Packet packet)
// {
//     getPairedDevice(receiver_id)->receivePacket(this->getId(), packet);
// }

/*!
 * @brief 接続中のデバイスからパケットを受信する
 * @tparam T パケットが含むデータの型受信する
 * @param sender_id 送信元デバイスのID
 * @param packet 受信するパケット
 */
// template <typename T>
// void Device::receivePacket(const int sender_id, const Packet<T> packet)
// {
//     memory_.emplace_back(packet.getData());
// }
// void Device::receivePacket(const int sender_id, const Packet packet)
// {
//     memory_.emplace_back(packet.getData());
// }

/*!
 * @brief パケットを生成する
 * @tparam T 送信するデータの型
 * @param data 送信するデータ
 * @return 生成したパケット
 */
template <typename T>
Packet<T> Device::makePacket(const T data)
{
    return Packet<T>(getId(), getNewPacketId(), data);
}
template class Packet<string>;
template class Packet<int>;
template class Packet<double>;

/* hello を出力 */
void Device::hello() const
{
    cout << "device[" << getId() << "]: hello!" << endl;
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
 * @brief ペアリング済みのデバイスオブジェクトを取得
 * @param id 対象デバイスのID
 * @return デバイスオブジェクト
 */
Device *Device::getPairedDevice(const int id)
{
    if (!paired_devices_.count(id))
        return nullptr;

    return paired_devices_.at(id);
}
