/*!
 * @file DeviceManager.cpp
 * @author tom96da
 * @brief DeviceManager クラスのメンバ関数
 * @date 2023-05-15
 */

#include <algorithm>
#include "Device.hpp"
#include "DeviceManager.hpp"

DeviceManager::DeviceManager(int num_devices)
{
    addDevice(num_devices);
}

/*!
 * @brief 管理下のデバイス数 取得
 * @return 管理化のデバイス数
 */
int DeviceManager::getNumDevices() const { return devices_.size(); };

/*!
 * @brief デバイスを取得
 * @param id デバイスID
 * @return IDに合致するデバイス
 */
Device *DeviceManager::getDeviceById(const int id)
{
    auto itr = find_if(devices_.begin(), devices_.end(),
                       [&](const Device devise)
                       {
                           return devise.getId() == id;
                       });
    if (itr == devices_.end())
        return nullptr;

    return &(*itr);
}

/*!
 * @brief デバイスを追加する
 * @param num_devices デバイス数
 */
void DeviceManager::addDevice(int num_devices)
{
    int pre_num_devices = devices_.size();
    for (int id = pre_num_devices; id < pre_num_devices + num_devices; id++)
    {
        devices_.emplace_back(id);
    }
}

/*!
 * @brief デバイス同士をペアリング登録させる
 * @param d1_id デバイス1
 * @param d2_id デバイス2
 */
void DeviceManager::pairDevices(const int d1_id, const int d2_id)
{
    getDeviceById(d1_id)->pairing(*getDeviceById(d2_id));
}

void DeviceManager::conectDevices(const int d1_id, const int d2_id)
{
    getDeviceById(d1_id)->connect(*getDeviceById(d2_id));
}