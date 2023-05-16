/*!
 * @file DeviceManager.cpp
 * @author tom96da
 * @brief DeviceManager クラスのメンバ関数
 * @date 2023-05-15
 */

#include <algorithm>
#include "Device.hpp"
#include "DeviceManager.hpp"

DeviceManager::DeviceManager(int init_num_devices)
    : mt_{random_device{}()},
      position_random_(0.0, 100),
      move_random_(0.0, 2 * M_PI),
      move_randn_(2.0, 0.7)
{
    addDevices(init_num_devices);
}

/*!
 * @brief 管理下のデバイス数 取得
 * @return 管理化のデバイス数
 */
int DeviceManager::getNumDevices() const { return devices_.size(); };

/*!
 * @brief IDに合致するデバイスを取得
 * @param id デバイスID
 * @return デバイスのポインタ
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
 * @brief IDに合致する座標を取得
 * @param id デバイスID
 * @return デバイスの座標のポインタ
 */
pair<double, double> *DeviceManager::getPositon(int id)
{
    auto itr = find_if(devices_.begin(), devices_.end(),
                       [&](const Device devise)
                       {
                           return devise.getId() == id;
                       });
    if (itr == devices_.end())
        return nullptr;

    return &positions_[distance(devices_.begin(), itr)];
}

/*!
 * @brief 座標の更新
 * @param id デバイスID
 */
void DeviceManager::updatePisition(int id)
{
    double radius = move_randn_(mt_);
    double theta = move_random_(mt_);

    auto position = getPositon(id);
    position->first += radius * cos(theta);
    position->second += radius * sin(theta);
}

/*!
 * @brief デバイスを追加する
 * @param num_devices デバイス数
 */

void DeviceManager::addDevices(int num_devices)
{
    int pre_num_devices = devices_.size();
    for (int id = pre_num_devices; id < pre_num_devices + num_devices; id++)
    {
        devices_.emplace_back(id);
        positions_.emplace_back(position_random_(mt_), position_random_(mt_));
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

/*!
 * @brief デバイス同士を接続する
 * @param d1_id デバイス1
 * @param d2_id デバイス2
 */
void DeviceManager::conectDevices(const int d1_id, const int d2_id)
{
    getDeviceById(d1_id)->connect(*getDeviceById(d2_id));
}
