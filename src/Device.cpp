/*!
 * @file Device.cpp
 * @author tom96da
 * @brief Device クラスのメンバ関数
 * @date 2023-05-11
 */

#include <iostream>
#include <algorithm>
#include "Device.hpp"

/* コンストラクタ */
Device::Device(const int id, int max_connections)
    : id_{id},
      max_connections_{max_connections}
{
}

/* デバイスID 取得 */
int Device::getId() const { return id_; }

/* デバイスIDのポインタ 取得 */
const int *Device::getIdPtr() const { return &id_; }

/* ペアリング済みデバイス数 取得 */
int Device::getNumPaired() const
{
    return paired_devices_.size();
}

/* ペアリング済みデバイスID 取得 */
vector<int> Device::getPairedDeviceId() const
{
    vector<int> paired_devices_id;
    for (const Device *paired_device : paired_devices_)
        paired_devices_id.emplace_back(paired_device->getId());

    return paired_devices_id;
}

/* 接続中のデバイス数を取得 */
int Device::getNumConnected() const { return connected_devices_.size(); }

/* 接続中のデバイス数のIDを取得 */
vector<int> Device::getConnectedDeviceId() const
{
    vector<int> connected_devices_id;
    for (const int *connected_id : connected_devices_)
        connected_devices_id.emplace_back(*connected_id);

    return connected_devices_id;
}

void Device::hello() const
{
    cout << "device[" << getId() << "] hello!" << endl;
}

/* デバイスが自身が取得 */
bool Device::isSelf(const Device &another_device) const
{
    return this->getId() == another_device.getId();
}

/*!
 * @brief デバイスがペアリング登録済みか取得
 * @param another_device 対象のデバイス
 * @retval true 登録済み
 * @retval false 未登録
 */
bool Device::isPaired(const Device &another_device) const
{
    if (isSelf(another_device))
        return false;

    auto itr =
        find_if(this->paired_devices_.begin(),
                this->paired_devices_.end(),
                [&](const Device *paired_device)
                {
                    return paired_device->getId() == another_device.getId();
                });
    if (itr == paired_devices_.end())
        return false;

    return true;
}

/*!
 * @brief デバイスをペアリング登録
 * @param another_device ペアリング相手のデバイス
 */
void Device::pairing(Device &another_device)
{
    if (this->isSelf(another_device))
        return;
    if (this->isPaired(another_device))
        return;

    this->paired_devices_.emplace_back(&another_device);
    another_device.pairing(*this);
}

/*!
 * @brief デバイスのペアリング解除
 * @param another_device
 */
void Device::removePairing(Device &another_device)
{
    if (this->isSelf(another_device))
        return;
    if (!this->isPaired(another_device))
        return;

    auto itr =
        find_if(this->paired_devices_.begin(),
                this->paired_devices_.end(),
                [&](const Device *paired_device)
                {
                    return paired_device->getId() == another_device.getId();
                });
    if (itr == paired_devices_.end())
        return;

    this->disconnect(another_device);
    this->paired_devices_.erase(itr);
    another_device.removePairing(*this);
}

/*!
 * @brief デバイスが接続中か取得
 * @param another_device
 * @retval true 接続中
 * @retval false 未接続
 */
bool Device::isConnected(const Device &another_device) const
{
    if (isSelf(another_device))
        return false;
    if (!this->isPaired(another_device))
        return false;

    auto itr =
        find_if(this->connected_devices_.begin(),
                this->connected_devices_.end(),
                [&](const int *connected_device_id)
                {
                    return *connected_device_id == another_device.getId();
                });
    if (itr == connected_devices_.end())
        return false;

    return true;
}

/*!
 * @brief 登録済みのデバイスと接続する
 * @param another_device 接続相手のデバイス
 */
void Device::connect(Device &another_device)
{
    if (this->isSelf(another_device))
        return;
    if (!this->isPaired(another_device))
        return;
    if (this->isConnected(another_device))
        return;
    if (getNumConnected() >= max_connections_)
        return;

    this->connected_devices_.emplace_back(another_device.getIdPtr());
    another_device.connect(*this);
}

/*!
 * @brief 接続中デバイスの切断
 * @param another_device
 */
void Device::disconnect(Device &another_device)
{
    if (isSelf(another_device))
        return;
    if (!isPaired(another_device))
        return;

    auto itr =
        find_if(this->connected_devices_.begin(),
                this->connected_devices_.end(),
                [&](const int *connected_device_id)
                {
                    return *connected_device_id == another_device.getId();
                });
    if (itr == connected_devices_.end())
        return;

    this->connected_devices_.erase(itr);
    another_device.disconnect(*this);
}

/*!
 * @brief 接続中デバイスに文字列を送信する
 * @param receiver 送信先デバイス
 * @param message 送信する文字列
 */
void Device::sendMessage(Device &receiver, string message)
{
    cout << "ID_" << this->getId() << " -> " << flush;
    receiver.receiveMessage(*this, message);
}

/*!
 * @brief 接続中デバイスから文字列を受信する
 * @param sender 送信元デバイス
 * @param message 受信する文字列
 */
void Device::receiveMessage(Device &sender, string message)
{
    cout << message << " -> ID_" << this->getId() << endl;
}

Device *Device::getPairedDevice(const int id)
{
    auto itr = find_if(paired_devices_.begin(), paired_devices_.end(),
                       [&](Device *device)
                       {
                           return device->getId() == id;
                       });
    if (itr == paired_devices_.end())
        return nullptr;

    return *itr;
}
