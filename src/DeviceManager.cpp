/*!
 * @file DeviceManager.cpp
 * @author tom96da
 * @brief DeviceManager クラスのメンバ関数
 * @date 2023-05-15
 */

#include <iostream>
#include "DeviceManager.hpp"

/* 接続可能距離 */
double DeviceManager::max_com_distance_ = MAX_COM_DISTANCE;

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
int DeviceManager::getNumDevices() const { return nodes_.size(); };

/*!
 * @brief IDに合致するデバイスを取得
 * @param id デバイスID
 * @return デバイスのポインタ
 */
Device &DeviceManager::getDeviceById(const int id)
{
    if (nodes_.count(id))
        return nodes_.at(id).getDevice();

    static Device null_device(-1);
    return null_device;
}

/*!
 * @brief IDに合致する座標を取得
 * @param id デバイスID
 * @return デバイスの座標のポインタ
 */
pair<double, double> &DeviceManager::getPosition(const int id)
{
    if (nodes_.count(id))
        return nodes_.at(id).getPosition();

    static pair<double, double> null_pos;
    return null_pos;
}

/*!
 * @brief デバイスを追加する
 * @param num_devices デバイス数
 */
void DeviceManager::addDevices(int num_devices)
{
    int pre_num_devices = nodes_.size();
    for (int id = pre_num_devices; id < pre_num_devices + num_devices; id++)
    {
        nodes_.emplace(id, Node(id));
        nodes_.at(id).setBias(bias_random_(mt_), bias_random_(mt_));
        nodes_.at(id).setPositon(position_random_(mt_), position_random_(mt_));
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
    if (isPaired(id_1, id_2))
        return;
    if (isSameDevice(id_1, id_2))
        return;

    getDeviceById(id_1).pairing(getDeviceById(id_2));
    getDeviceById(id_2).pairing(getDeviceById(id_1));
}

void DeviceManager::unpairDevices(const int id_1, const int id_2)
{
    if (isSameDevice(id_1, id_2))
        return;

    getDeviceById(id_1).unpairing(id_2);
    getDeviceById(id_2).unpairing(id_1);
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
    if (!isPaired(id_1, id_2))
        return;
    if (isSameDevice(id_1, id_2))
        return;

    if (getDeviceById(id_1).connect(id_2))
        if (!getDeviceById(id_2).connect(id_1))
            getDeviceById(id_1).disconnect(id_2);
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

    getDeviceById(id_1).disconnect(id_2);
    getDeviceById(id_2).disconnect(id_1);
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

    auto &[pos_x, pos_y] = getPosition(id);
    auto &[bias_x, bias_y] = getBias(id);

    tmp = pos_x + dx + bias_x;
    if ((tmp > 0) & (tmp < field_size_))
        pos_x = tmp;
    else
    {
        pos_x -= (dx + bias_x);
        bias_x = -bias_x;
    }

    tmp = pos_y + dy + bias_y;
    if ((tmp > 0) & (tmp < field_size_))
        pos_y = tmp;
    else
    {
        pos_y -= (dy + bias_y);
        bias_y = -bias_y;
    }
}

/*!
 * @brief フラッディングを開始する
 * @param id 開始デバイスのID
 * @return データ識別子
 */
void DeviceManager::startFlooding(const int id)
{
    for (auto &node : nodes_)
        node.second.getDevice().resetFloodSteps();

    size_t data_id = getDeviceById(id).makeFloodData();
    int num_devices_have_data = showDevicesGetData(data_id);
    do
    {
        num_devices_have_data = getNumDevicesHaveData(data_id);

        for (auto &node : nodes_)
            node.second.getDevice().flooding();
    } while (num_devices_have_data != showDevicesGetData(data_id));

    cout << getNumDevicesHaveData(data_id) << " devices have data" << endl;
}

/*!
 * @param data_id データ識別子
 * @return データ識別子が合致するデータを保持するデバイス数
 */
int DeviceManager::getNumDevicesHaveData(size_t data_id)
{
    for (auto &node : nodes_)
    {
        auto &device = node.second.getDevice();
        int id = device.getId();

        if (devices_have_data_.count(id))
            continue;

        if (device.hasData(data_id))
            devices_have_data_.emplace(id);
    }

    return devices_have_data_.size();
}

/*!
 * @param data_id データ識別子
 * @return データ識別子が合致するデータを保持するデバイス数
 */
int DeviceManager::showDevicesGetData(size_t data_id)
{
    cout << "get data : ";
    for (auto &node : nodes_)
    {
        auto &device = node.second.getDevice();
        int id = device.getId();

        if (devices_have_data_.count(id))
            continue;

        if (device.hasData(data_id))
        {
            devices_have_data_.emplace(id);
            cout << id << ", ";
        }
    }
    cout << endl;

    return devices_have_data_.size();
}

/*!
 * @brief IDに合致するバイアスを取得
 * @param id デバイスID
 * @return デバイスのバイアスのポインタ
 */
pair<double, double> &DeviceManager::getBias(const int id)
{
    if (nodes_.count(id))
        return nodes_.at(id).getBias();

    static pair<double, double> null_bias;
    return null_bias;
}

/*!
 * @brief デバイス間距離の取得
 * @param id_1 デバイスID1
 * @param id_2 デバイスID2
 * @return　デバイス間距離
 */
double DeviceManager::getDistance(const int id_1, const int id_2)
{
    auto &[pos_1_x, pos_1_y] = getPosition(id_1);
    auto &[pos_2_x, pos_2_y] = getPosition(id_2);

    return hypot(pos_1_x - pos_2_x, pos_1_y - pos_2_y);
}

/*!
 * @brief デバイスIDが一致するか取得
 * @param id_1 対象デバイスのID-1
 * @param id_2 対象デバイスのID-2
 * @retval true 一致
 * @retval false 不一致
 */
bool DeviceManager::isSameDevice(const int id_1, const int id_2) const
{
    return id_1 == id_2;
}

/*!
 * @brief デバイスがペアリング登録済みか取得
 * @param id_1 対象デバイスのID-1
 * @param id_2 対象デバイスのID-2
 * @retval true 登録済み
 * @retval false 未登録
 */
bool DeviceManager::isPaired(const int id_1, const int id_2)
{
    return getDeviceById(id_1).isPaired(id_2) &&
           getDeviceById(id_2).isPaired(id_1);
}

/*!
 * @brief デバイスが接続中か取得
 * @param id_1 対象デバイスのID-1
 * @param id_2 対象デバイスのID-2
 * @retval true 接続中
 * @retval false 未接続
 */
bool DeviceManager::isConnected(const int id_1, const int id_2)
{
    return getDeviceById(id_1).isConnected(id_2) &&
           getDeviceById(id_2).isConnected(id_1);
}

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
Device &DeviceManager::Node::getDevice() { return device_; }

/*!
 * @return 移動バイアス
 */
pair<double, double> &DeviceManager::Node::getBias() { return bias_; }

/*!
 * @return 座標
 */
pair<double, double> &DeviceManager::Node::getPosition() { return position_; }

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
