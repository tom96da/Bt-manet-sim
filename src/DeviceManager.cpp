/*!
 * @file DeviceManager.cpp
 * @author tom96da
 * @brief DeviceManager クラスのメンバ関数
 * @date 2023-05-15
 */

#include "DeviceManager.hpp"
#include <iostream>
#include <algorithm>

/* 接続可能距離 */
double DeviceManager::max_com_distance_ = MAX_COM_DISTANCE;

/* デバイスマネージャー クラス */

/*!
 * @brief コンストラクタ
 * @param field_size フィールドサイズ
 * @param init_num_devices 初期デバイス数 デフォルト値: 0
 */
DeviceManager::DeviceManager(double field_size, int init_num_devices)
    : field_size_{field_size},
      mt_{random_device{}()},
      position_random_{0.0, field_size_},
      move_random_{0.0, 2 * M_PI},
      move_randn_{0, 0.3},
      bias_random_{-0.4, 0.4},
      willingness_random_{1, 5}
{
    addDevices(init_num_devices);
}

/*!
 * @brief 管理下のデバイス数 取得
 * @return 管理化のデバイス数
 */
int DeviceManager::getNumDevices() const { return nodes_.size(); };

/*!
 * @brief IDに合致するデバイスを参照 (不正なIDではないか事前確認)
 * @param id デバイスID
 * @return デバイスの参照
 */
Device &DeviceManager::getDeviceById(const int id)
{
    return nodes_.at(id).getDevice();
}

/*!
 * @brief IDに合致する座標を参照 (不正なIDではないか事前確認)
 * @param id デバイスID
 * @return デバイスの座標の参照
 */
pair<double, double> &DeviceManager::getPosition(const int id)
{
    return nodes_.at(id).getPosition();
}

/*!
 * @brief デバイスを追加する
 * @param num_devices デバイス数
 */
void DeviceManager::addDevices(int num_devices)
{
    int device_id_next = 0;
    if (!nodes_.empty())
    {
        device_id_next = nodes_.rbegin()->first + 1;
    }

    for (int id = device_id_next; id < device_id_next + num_devices; id++)
    {
        nodes_.emplace(id, Node(id, willingness_random_(mt_)));
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
    if (getDistance(id_1, id_2) < 0)
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
    if (getDistance(id_1, id_2) < 0)
        return;
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
    if (getDistance(id_1, id_2) < 0)
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
    if (getDistance(id_1, id_2) < 0)
        return;

    getDeviceById(id_1).disconnect(id_2);
    getDeviceById(id_2).disconnect(id_1);
}

/*!
 * @brief 離れたデバイスとの接続を切る
 * @param id デバイスID
 */
void DeviceManager::disconnectDevices(const int id)
{
    for (auto &cntd : getDeviceById(id).getConnectedDeviceId())
        disconnectDevices(id, cntd);
}

/*!
 * @brief 座標の更新
 * @param id デバイスID
 */
void DeviceManager::updatePosition(const int id)
{
    if (!nodes_.count(id))
        return;

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
 * @brief すべてのデバイスの座標の更新
 */
void DeviceManager::updatePositionAll()
{
    for (auto id : getDevicesList())
        updatePosition(id);
    setDevices();
}

/*!
 * @brief デバイスをペアリングおよび接続する
 */
void DeviceManager::setDevices()
{
    vector<int> &&list_1{getDevicesList()};

    for (auto const id : list_1)
        disconnectDevices(id);

    shuffle(list_1.begin(), list_1.end(), mt_);

    auto list_2 = list_1;

    for (const auto id_1 : list_1)
    {
        for (const auto id_2 : list_2)
        {
            pairDevices(id_1, id_2);
            connectDevices(id_1, id_2);
        }

        auto itr = find(list_2.begin(), list_2.end(), id_1);
        list_2.erase(itr);
        shuffle(list_2.begin(), list_2.end(), mt_);
    }
}

/*!
 * @brief Helloパケットを送信する
 */
void DeviceManager::sendHello()
{
    for (auto &&id : getDevicesList())
        getDeviceById(id).sendHello();
}

void DeviceManager::makeMPR()
{
    for (auto &&id : getDevicesList())
        getDeviceById(id).makeMPR();
}

/*!
 * @brief フラッディングを開始する
 * @param id 開始デバイスのID
 * @return データ識別子
 */
void DeviceManager::startFlooding(const int id)
{
    if (!nodes_.count(id))
        return;
    auto &device_starter = getDeviceById(id);

    int num_devices_have_data = 0,
        num_step = 0;

    device_starter.flooding(-1);
    set<int> devices_have_data;

    size_t data_id = device_starter.makeFloodData();
    // std::cout << "start from : ";

    while (num_devices_have_data !=
           aggregateDevicesGetData(data_id, devices_have_data))
    {
        num_devices_have_data = devices_have_data.size();

        for (auto &node : nodes_)
            node.second.getDevice().flooding();

        num_step++;
        device_starter.flooding(1);
        // std::cout << num_step << " hop ; get data : ";
    }

    std::cout << devices_have_data.size() << " devices have data" << endl;
}

/*!
 * @param data_id データ識別子
 * @param devices_have_data 集計コンテナ
 * @return データ識別子が合致するデータを保持するデバイス数
 */
int DeviceManager::aggregateDevicesGetData(size_t data_id,
                                           set<int> &devices_have_data)
{

    for (auto &node : nodes_)
    {
        auto &device = node.second.getDevice();
        int id = device.getId();

        if (devices_have_data.count(id))
            continue;

        if (device.hasData(data_id))
        {
            devices_have_data.emplace(id);
            // std::cout << id << ", ";
        }
    }
    // std::cout << endl;

    return devices_have_data.size();
}

/*!
 * @return デバイスIDのリスト
 */
vector<int> DeviceManager::getDevicesList() const
{
    vector<int> list;
    for (auto &node : nodes_)
        list.emplace_back(node.first);

    return list;
}

/*!
 * @brief IDに合致するバイアスを参照 (不正なIDではないか事前確認)
 * @param id デバイスID
 * @return デバイスのバイアスの参照
 */
pair<double, double> &DeviceManager::getBias(const int id)
{
    return nodes_.at(id).getBias();
}

/*!
 * @brief デバイス間距離の取得
 * @param id_1 デバイスID1
 * @param id_2 デバイスID2
 * @retval 0< デバイス間距離
 * @retval <0 デバイスIDが不正
 */
double DeviceManager::getDistance(const int id_1, const int id_2)
{
    if (!nodes_.count(id_1))
        return -1.0;
    if (!nodes_.count(id_2))
        return -1.0;

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
 * @param willingness willingness
 */
DeviceManager::Node::Node(const int id, const int willingness)
    : device_{id, willingness},
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
