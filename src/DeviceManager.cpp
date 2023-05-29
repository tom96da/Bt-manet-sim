/*!
 * @file DeviceManager.cpp
 * @author tom96da
 * @brief DeviceManager クラスのメンバ関数
 * @date 2023-05-15
 */

#include <iostream>
#include "Device.hpp"
#include "DeviceManager.hpp"

/* 接続可能距離 */
double DeviceManager::max_com_distance_ = MAX_COM_DISTANCE;

/* ノード クラス */

/*!
 * @brief コンストラクタ
 * @param id デバイスID
 */
DeviceManager::Node::Node(const int id)
    : device_{id},
      bias_{0.0, 0.0},
      position_{0.0, 0.0}
{
}

/*!
 * @return デバイスオブジェクト
 */
Device *DeviceManager::Node::getDevice() { return &device_; }

/*!
 * @return 移動バイアス
 */
pair<double, double> *DeviceManager::Node::getBias() { return &bias_; }

/*!
 * @return 座標
 */
pair<double, double> *DeviceManager::Node::getPosition() { return &position_; }

/* バイアスを設定 */
void DeviceManager::Node::setBias(double bias_x, double bias_y)
{
    bias_ = {bias_x, bias_y};
}

/* 座標を更新 */
void DeviceManager::Node::setPositon(double pos_x, double pos_y)
{
    position_ = {pos_x, pos_y};
}

/* デバイスマネージャー クラス */

/*!
 * @brief コンストラクタ
 * @param field_size フィールドサイズ
 * @param init_num_devices デバイス数 デフォルト値: 0
 */
DeviceManager::DeviceManager(double field_size, int init_num_devices)
    : field_size_{field_size},
      mt_{random_device{}()},
      position_random_(0.0, field_size_),
      move_random_(0.0, 2 * M_PI),
      move_randn_(0, 0.3),
      bias_random_(-0.4, 0.4)
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
    if (devices_.count(id))
        return devices_.at(id).getDevice();

    return nullptr;
}

/*!
 * @brief IDに合致する座標を取得
 * @param id デバイスID
 * @return デバイスの座標のポインタ
 */
pair<double, double> *DeviceManager::getPositon(const int id)
{
    if (devices_.count(id))
        return devices_.at(id).getPosition();

    return nullptr;
}

/*!
 * @brief IDに合致するバイアスを取得
 * @param id デバイスID
 * @return デバイスのバイアスのポインタ
 */
pair<double, double> *DeviceManager::getBias(const int id)
{
    if (devices_.count(id))
        return devices_.at(id).getBias();

    return nullptr;
}

/*!
 * @brief 座標の更新
 * @param id デバイスID
 */
void DeviceManager::updatePisition(const int id)
{
    double tmp;
    double dx = move_randn_(mt_);
    double dy = move_randn_(mt_);

    auto position = getPositon(id);
    auto bias = getBias(id);

    tmp = position->first + dx + bias->first;
    if (tmp > 0 & tmp < field_size_)
        position->first = tmp;
    else
    {
        position->first -= (dx + bias->first);
        bias->first = -bias->first;
    }

    tmp = position->second + dy + bias->second;
    if (tmp > 0 & tmp < field_size_)
        position->second = tmp;
    else
    {
        position->second -= (dy + bias->second);
        bias->second = -bias->second;
    }
}

/*!
 * @brief デバイス間距離の取得
 * @param id_1 デバイスID1
 * @param id_2 デバイスID2
 * @return　デバイス間距離
 */
double DeviceManager::getDistance(const int id_1, const int id_2)
{
    auto pos_1 = getPositon(id_1);
    auto pos_2 = getPositon(id_2);

    return hypot(pos_1->first - pos_2->first, pos_1->second - pos_2->second);
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
        devices_.emplace(id, Node(id));
        devices_.at(id).setBias(bias_random_(mt_), bias_random_(mt_));
        devices_.at(id).setPositon(position_random_(mt_), position_random_(mt_));
    }
}

/*!
 * @brief デバイス同士をペアリング登録させる
 * @param d1_id デバイスID1
 * @param d2_id デバイスID2
 */
void DeviceManager::pairDevices(const int id_1, const int id_2)
{
    if (getDistance(id_1, id_2) > max_com_distance_)
        return;

    getDeviceById(id_1)->pairing(*getDeviceById(id_2));
}

/*!
 * @brief デバイス同士を接続する
 * @param d1_id デバイスID1
 * @param d2_id デバイスID2
 */
void DeviceManager::connectDevices(const int id_1, const int id_2)
{
    if (getDistance(id_1, id_2) > max_com_distance_)
        return;

    getDeviceById(id_1)->connect(id_2);
}

/*!
 * @brief デバイス同士の接続を切る
 * @param d1_id デバイスID1
 * @param d2_id デバイスID2
 */
void DeviceManager::disconnectDevices(const int id_1, const int id_2)
{
    if (getDistance(id_1, id_2) <= max_com_distance_)
        return;

    getDeviceById(id_1)->disconnect(id_2);
}