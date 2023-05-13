/*!
 * @file Device.cpp
 * @author tom96da
 * @brief Device クラスのメンバ関数
 * @date 2023-05-11
 */

#include <iostream>
#include <algorithm>
// #include <numeric>
#include "Device.hpp"

// コンストラクタ
Device::Device(int id, int max_connections)
    : id_(id),
      max_connections_(max_connections),
      name_([&]() -> string
            { return "device[" + to_string(id) + "]"; }())
{
}

// デバイスID 取得
int Device::getId() const { return id_; }
// デバイス名 取得
string Device::getName() const { return name_; }

// デバイス速度 更新
void Device::setVelocity()
{
    velocity_ = {0.0, 0.0};
}
// デバイス速度 取得
pair<double, double> Device::getVelocity() { return velocity_; }

// ペアリング済みデバイス数 取得
int Device::getNumPaired() const
{
    return paired_devices_.size();
}
// ペアリング済みデバイスID 取得
vector<int> Device::getPairedDeviceId() const
{
    vector<int> paired_devices_id;
    for (const Device *paired_device : paired_devices_)
        paired_devices_id.emplace_back(paired_device->getId());

    return paired_devices_id;
}
/*!
 * @brief デバイスがペアリング登録済みか取得
 * @param otherDevice 対象のデバイス
 * @retval true 登録済み
 * @retval false 未登録
 */
bool Device::isPaired(const Device &otherDevice) const
{
    auto itr = find_if(paired_devices_.begin(), paired_devices_.end(),
                       [&](const Device *paired_device)
                       { return paired_device->getId() == otherDevice.getId(); });

    if (itr == paired_devices_.end())
        return false;
    else
        return true;
}
// bool Device::isPaired(const Device &otherDevice) const
// {
//     // auto f = [](const Device &d1, const Device &d2) -> bool
//     // {
//     //     for (const Device *paired_device : d1)
//     //     {
//     //     }
//     // };
// }

void Device::hello() const
{
    cout << getName() << " hello!" << endl;
}
/*!
 * @brief デバイスをペアリング登録
 * @param otherDevice ペアリング相手のデバイス
 */
void Device::pairing(Device &otherDevice)
{
    if (!isPaired(otherDevice))
    {
        paired_devices_.emplace_back(&otherDevice);
        otherDevice.paired_devices_.emplace_back(this);
    }
}
void Device::connect(Device &otherDevice)
{
    // if (isPaired(otherDevice))
    //     connected_devices_.emplace_back((otherDevice).getId());

    // auto *device = reduce(paired_devices_.begin(), paired_devices_.end(),
    //                       static_cast<Device *>(nullptr),
    //                       [&otherDevice](Device &a, Device &b)
    //                       {
    //                           return b.getId() == otherDevice.getId() ? b : a;
    //                       });
    // connected_devices_.emplace_back(device->getId());
}

/*!
 * @brief 接続中デバイスに文字列を送信する
 * @param receiver 送信先デバイス
 * @param message 送信する文字列
 */
void Device::sendMessage(Device &receiver, string message)
{
    cout << "ID_" << getId() << " -> " << message << " -> " << flush;
    receiver.receiveMessage(*this, message);
}
void Device::receiveMessage(Device &sender, string message)
{
    cout << "ID_" << getId() << endl;
}
