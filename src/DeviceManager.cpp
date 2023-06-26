/*!
 * @file DeviceManager.cpp
 * @author tom96da
 * @brief DeviceManager クラスのメンバ関数
 * @date 2023-05-15
 */

#include "DeviceManager.hpp"

#include <algorithm>
#include <iostream>
#include <utility>

/* デバイスマネージャー クラス */

/*!
 * @brief コンストラクタ
 * @param field_size フィールドサイズ
 * @param sim_mode シミュレーションモード
 */
DeviceManager::DeviceManager(const double field_size,
                             const SimulationMode sim_mode)
    : field_size_{field_size},
      sim_mode_{sim_mode},
      mt_{random_device{}()},
      position_random_{0.0, field_size_},
      move_randn_{0, 0.3},
      bias_random_{-0.4, 0.4},
      willingness_random_{1, 5} {
    switch (sim_mode_) {
        case SIMMODE::EXITING:
            std::cout << "Simulation mode: EXITNG" << std::endl;
            break;
        case SIMMODE::PROPOSAL:
            std::cout << "Simulation mode: PROPOSAL" << std::endl;
            break;
        default:
            std::cout << "Please execute after specifying "
                      << "either EXITING or PROPOSAL for the simulation mode."
                      << std::endl;
            std::exit(EXIT_SUCCESS);
            break;
    }
}

void DeviceManager::ResetManager() {
    Device::resetNumPacket();
    nodes_.clear();
}

/* 接続可能距離 */
double DeviceManager::_max_com_distance_ = MAX_COM_DISTANCE;

/*!
 * @return double 接続可能距離
 */
double DeviceManager::getMaxComDistance() { return _max_com_distance_; }

/*!
 * @brief 管理下のデバイス数 取得
 * @return int 管理化のデバイス数
 */
int DeviceManager::getNumDevices() const { return nodes_.size(); };

/*!
 * @brief IDに合致するデバイスを参照 (不正なIDではないか事前確認)
 * @param id デバイスID
 * @return Device デバイスの参照
 */
DeviceManager::Node &DeviceManager::getDeviceById(const int id) {
    return nodes_.at(id);
}

/*!
 * @brief IDに合致する座標を参照 (不正なIDではないか事前確認)
 * @param id デバイスID
 * @return pair<double, double> デバイスの座標の参照
 */
pair<double, double> &DeviceManager::getPosition(const int id) {
    return nodes_.at(id).getPosition();
}

/*!
 * @brief デバイスを追加する
 * @param num_devices デバイス数
 */
void DeviceManager::addDevices(const int num_devices) {
    int id_next = 0;
    if (getNumDevices() > 0) {
        auto &[id_last, _] = *nodes_.rend();
        id_next = id_last + 1;
    }

    for (int id = id_next; id < id_next + num_devices; id++) {
        nodes_.emplace(id, Node(id, willingness_random_(mt_)));
        nodes_.at(id).setBias(bias_random_(mt_), bias_random_(mt_));
        nodes_.at(id).setPositon(position_random_(mt_), position_random_(mt_));
    }
}

/*!
 * @brief デバイス同士をペアリング登録させる
 * @param id_1 デバイスID1
 * @param id_2 デバイスID2
 */
void DeviceManager::pairDevices(const int id_1, const int id_2) {
    if (getDistance(id_1, id_2) > _max_com_distance_) {
        return;
    }
    if (getDistance(id_1, id_2) < 0) {
        return;
    }
    if (isPaired(id_1, id_2)) {
        return;
    }
    if (isSameDevice(id_1, id_2)) {
        return;
    }

    getDeviceById(id_1).pairing(getDeviceById(id_2));
    getDeviceById(id_2).pairing(getDeviceById(id_1));
}

/*!
 * @brief デバイス同士をペアリング解除させる
 * @param id_1 デバイスID1
 * @param id_2 デバイスID2
 */
void DeviceManager::unpairDevices(const int id_1, const int id_2) {
    if (getDistance(id_1, id_2) < 0) {
        return;
    }
    if (isSameDevice(id_1, id_2)) {
        return;
    }

    getDeviceById(id_1).unpairing(id_2);
    getDeviceById(id_2).unpairing(id_1);
}

/*!
 * @brief デバイス同士を接続する
 * @param d1_id デバイスID1
 * @param d2_id デバイスID2
 */
void DeviceManager::connectDevices(const int id_1, const int id_2) {
    if (getDistance(id_1, id_2) > _max_com_distance_) {
        return;
    }
    if (getDistance(id_1, id_2) < 0) {
        return;
    }
    if (!isPaired(id_1, id_2)) {
        return;
    }
    if (isSameDevice(id_1, id_2)) {
        return;
    }

    if (getDeviceById(id_1).connect(id_2)) {
        if (!getDeviceById(id_2).connect(id_1)) {
            getDeviceById(id_1).disconnect(id_2);
        }
    }
}

/*!
 * @brief デバイス同士の接続を切る
 * @param d1_id デバイスID1
 * @param d2_id デバイスID2
 */
void DeviceManager::disconnectDevices(const int id_1, const int id_2) {
    if (getDistance(id_1, id_2) <= _max_com_distance_) {
        return;
    }
    if (getDistance(id_1, id_2) < 0) {
        return;
    }

    getDeviceById(id_1).disconnect(id_2);
    getDeviceById(id_2).disconnect(id_1);
}

/*!
 * @brief 離れたデバイスとの接続を切る
 * @param id デバイスID
 */
void DeviceManager::disconnectDevices(const int id) {
    for (auto id_cntd : getDeviceById(id).getIdConnectedDevices()) {
        disconnectDevices(id, id_cntd);
    }
}

/*!
 * @brief 座標の更新
 * @param id デバイスID
 */
void DeviceManager::updatePosition(const int id) {
    if (!nodes_.count(id)) {
        return;
    }

    double tmp;
    double dx = move_randn_(mt_);
    double dy = move_randn_(mt_);

    auto &[pos_x, pos_y] = getPosition(id);
    auto &[bias_x, bias_y] = getBias(id);

    tmp = pos_x + dx + bias_x;
    if ((tmp > 0) & (tmp < field_size_)) {
        pos_x = tmp;
    } else {
        pos_x -= (dx + bias_x);
        bias_x = -bias_x;
    }

    tmp = pos_y + dy + bias_y;
    if ((tmp > 0) & (tmp < field_size_)) {
        pos_y = tmp;
    } else {
        pos_y -= (dy + bias_y);
        bias_y = -bias_y;
    }
}

/*!
 * @brief すべてのデバイスの座標の更新
 */
void DeviceManager::updatePositionAll() {
    for (auto id : getDevicesList()) {
        updatePosition(id);
    }
    buildNetwork();
}

/*!
 * @brief すべてのデバイスのメモリをクリアする
 */
void DeviceManager::clearMemory() {
    for (auto id : getDevicesList()) {
        getDeviceById(id).clearMemory();
    }
}

/*!
 * @brief ネットワークを構築する
 */
void DeviceManager::buildNetwork() {
    switch (sim_mode_) {
        case SIMMODE::EXITING:
            buildNetworkRandom();
            break;
        case SIMMODE::PROPOSAL:
            buildNetworkByDistance();
            break;
        default:
            break;
    }
}

/*!
 * @brief ランダムにネットワークを構築する
 */
void DeviceManager::buildNetworkRandom() {
    auto &&list_1 = getDevicesList();

    for (auto const id : list_1) {
        disconnectDevices(id);
    }

    shuffle(list_1.begin(), list_1.end(), mt_);

    auto list_2 = list_1;

    for (const auto id_1 : list_1) {
        for (const auto id_2 : list_2) {
            pairDevices(id_1, id_2);
            connectDevices(id_1, id_2);
        }

        auto itr = find(list_2.begin(), list_2.end(), id_1);
        list_2.erase(itr);
        shuffle(list_2.begin(), list_2.end(), mt_);
    }
}

/*!
 * @brief デバイス間距離が大きい順にネットワークを構築する
 */
void DeviceManager::buildNetworkByDistance() {
    auto &&list = getDevicesList();
    for (auto id : list) {
        disconnectDevices(id);
    }

    for (auto id_1 : list) {
        for (auto id_2 : list) {
            pairDevices(id_1, id_2);
        }
    }

    for (auto id_1 : list) {
        auto &&id_pairs = getDeviceById(id_1).getIdPairedDevices();
        map<double, int, greater<double>> tmp;
        transform(id_pairs.begin(), id_pairs.end(), inserter(tmp, tmp.begin()),
                  [&](const int id_2) -> pair<double, int> {
                      return {getDistance(id_1, id_2), id_2};
                  });
        for (auto [_, id_2] : tmp) {
            connectDevices(id_1, id_2);
        }
    }
}

/*!
 * @brief すべてのデバイスにHelloパケットを送信させる
 */
void DeviceManager::sendHello() {
    for (auto id : getDevicesList()) {
        getDeviceById(id).sendHello();
    }
}

/*!
 * @brief すべてのデバイスにルーティングテーブルを送信させる
 */
void DeviceManager::sendTable() {
    for (auto id : getDevicesList()) {
        getDeviceById(id).sendTable();
    }
}

/*!
 * @brief すべてのデバイスにMPR集合を作成させる
 */
void DeviceManager::makeMPR() {
    for (auto id : getDevicesList()) {
        getDeviceById(id).makeMPR();
    }
}

/*!
 * @brief MPR集合を出力する
 * @param id デバイスID
 */
void DeviceManager::showMPR(const int id) {
    WriteMode write_mode = WriteMode::SIMPLE;

    getDeviceById(id).showMPR(write_mode);
}

/*!
 * @brief すべてのデバイスにルーティングテーブルを作成させる
 * @return int 更新ありデバイス数
 */
int DeviceManager::makeTable() {
    int result = 0;

    for (auto id : getDevicesList()) {
        result += static_cast<int>(getDeviceById(id).makeTable());
    }

    return result;
}

/*!
 * @brief フラッディングを開始する
 * @param id 開始デバイスのID
 * @return データ識別子
 */
pair<size_t, int> DeviceManager::flooding(const int id) {
    WriteMode write_mode = WriteMode::HIDE;

    if (!nodes_.count(id)) {
        return {0, 0};
    }
    auto &device_starter = getDeviceById(id);

    int num_devices_have_data = 0, num_step = 0;

    device_starter.flooding(-1);
    set<int> devices_have_data;

    size_t data_id = device_starter.makeFloodData();
    switch (write_mode) {
        case WriteMode::VISIBLE:
            std::cout << "start from : ";
            break;
        case WriteMode::ARRAY:
            std::cout << "[";
            break;
        default:
            break;
    }

    while (num_devices_have_data !=
           aggregateDevices(data_id, devices_have_data, write_mode)) {
        num_devices_have_data = devices_have_data.size();

        for (auto &[_, device] : nodes_) {
            device.flooding();
        }

        num_step++;
        device_starter.flooding(1);

        switch (write_mode) {
            case WriteMode::VISIBLE:
                std::cout << "\e[2D \n" << num_step << " hop : ";
                break;
            case WriteMode::ARRAY:
                std::cout << "\e[2D],\n"
                          << "[";
                break;
            default:
                break;
        }
    }
    if (write_mode != WriteMode::HIDE) {
        std::cout << "\r" << devices_have_data.size() << " devices have data"
                  << endl;
    }

    return {data_id, devices_have_data.size()};
}

/*!
 * @brief データを持っているか集計する
 * @param data_id データ識別子
 * @return int データ識別子が合致するデータを保持するデバイス数
 */
int DeviceManager::aggregateDevices(size_t data_id) {
    int cnt = 0;

    for (auto &[_, device] : nodes_) {
        if (device.hasData(data_id)) {
            cnt++;
        }
    }

    return cnt;
}

/*!
 * @brief データを持っているか集計する
 * @param data_id データ識別子
 * @param devices_have_data 集計コンテナ
 * @param write_mode 出力モード
 * @return int データ識別子が合致するデータを保持するデバイス数
 */
int DeviceManager::aggregateDevices(size_t data_id, set<int> &devices_have_data,
                                    const WriteMode write_mode) {
    for (auto &[id, device] : nodes_) {
        if (devices_have_data.count(id)) {
            continue;
        }

        if (device.hasData(data_id)) {
            devices_have_data.emplace(id);
            if (static_cast<int>(write_mode) > 1) {
                std::cout << id << ", ";
            }
        }
    }

    return devices_have_data.size();
}

/*!
 * @brief
 * @param id_source 発信元デバイスID
 * @param id_dest 宛先デバイスのID
 */
void DeviceManager::unicast(const int id_source, const int id_dest) {
    WriteMode write_mode = WriteMode::SIMPLE;

    if (getDistance(id_source, id_dest) < 0) {
        return;
    }

    int num_hop = 0;
    auto [id_nextHop, distance] =
        getDeviceById(id_source).startUnicast(id_dest);
    if (write_mode != WriteMode::HIDE) {
        std::cout << "Unicast distance: " << distance << "\n"
                  << id_source << " -> " << id_nextHop;
    }
    num_hop++;

    for (int flag = 1; flag > 0;) {
        tie(id_nextHop, flag) = getDeviceById(id_nextHop).hopping();
        if (flag == 0) {
            break;
        }

        if (write_mode != WriteMode::HIDE) {
            std::cout << " -> " << id_nextHop;
        }
        num_hop++;
    }
    if (write_mode != WriteMode::HIDE) {
        std::cout << "\n" << num_hop << " times hop." << std::endl;
    }
}

/*!
 * @return デバイスIDのリスト
 */
vector<int> DeviceManager::getDevicesList() const {
    vector<int> list;
    for (auto &[id, _] : nodes_) {
        list.emplace_back(id);
    }

    return list;
}

/*!
 * @brief IDに合致するバイアスを参照 (不正なIDではないか事前確認)
 * @param id デバイスID
 * @return デバイスのバイアスの参照
 */
pair<double, double> &DeviceManager::getBias(const int id) {
    return nodes_.at(id).getBias();
}

/*!
 * @brief デバイス間距離の取得
 * @param id_1 デバイスID1
 * @param id_2 デバイスID2
 * @retval 0< デバイス間距離
 * @retval <0 デバイスIDが不正
 */
double DeviceManager::getDistance(const int id_1, const int id_2) {
    if (!nodes_.count(id_1)) {
        return -1.0;
    }
    if (!nodes_.count(id_2)) {
        return -1.0;
    }

    auto &&[pos_1_x, pos_1_y] = getPosition(id_1);
    auto &&[pos_2_x, pos_2_y] = getPosition(id_2);

    return hypot(pos_1_x - pos_2_x, pos_1_y - pos_2_y);
}

/*!
 * @brief デバイスIDが一致するか取得
 * @param id_1 対象デバイスのID-1
 * @param id_2 対象デバイスのID-2
 * @retval true 一致
 * @retval false 不一致
 */
bool DeviceManager::isSameDevice(const int id_1, const int id_2) const {
    return id_1 == id_2;
}

/*!
 * @brief デバイスがペアリング登録済みか取得
 * @param id_1 対象デバイスのID-1
 * @param id_2 対象デバイスのID-2
 * @retval true 登録済み
 * @retval false 未登録
 */
bool DeviceManager::isPaired(const int id_1, const int id_2) {
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
bool DeviceManager::isConnected(const int id_1, const int id_2) {
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
    : Device{id, willingness}, bias_{0.0, 0.0}, position_{0.0, 0.0} {}

/*!
 * @return 移動バイアス
 */
pair<double, double> &DeviceManager::Node::getBias() { return bias_; }

/*!
 * @return 座標
 */
pair<double, double> &DeviceManager::Node::getPosition() { return position_; }

/*!
 * @brief バイアスを設定
 * @param bias_x x成分
 * @param bias_y y成分
 */
void DeviceManager::Node::setBias(double bias_x, double bias_y) {
    bias_ = {bias_x, bias_y};
}

/*!
 * @brief 座標を設定
 * @param pos_x x座標
 * @param pos_y y座標
 */
void DeviceManager::Node::setPositon(double pos_x, double pos_y) {
    position_ = {pos_x, pos_y};
}

/*!
 * @brief MPR集合を出力する
 * @param write_mode 出力モード
 */
void DeviceManager::Node::showMPR(const WriteMode write_mode) {
    auto id_cntds = getIdConnectedDevices();
    auto id_MPR = getMPR();

    std::cout << getName();
    switch (write_mode) {
        case WriteMode::VISIBLE:
            std::cout << "  connected with ";
            break;
        case WriteMode::SIMPLE:
            std::cout << " --> ";
        default:
            break;
    }

    for (auto id_cntd : id_cntds) {
        std::cout << id_cntd << [&]() {
            if (id_MPR.count(id_cntd) && write_mode == WriteMode::SIMPLE) {
                return "*";
            }

            return "";
        }() << ", ";
    }
    std::cout << "\e[2D " << std::endl;

    if (write_mode == WriteMode::VISIBLE) {
        for (auto id_cntd : id_cntds) {
            auto dev__ = getPairedDevice(id_cntd);
            std::cout << dev__.getName() << [&] {
                if (id_MPR.count(id_cntd)) {
                    return "*";
                }

                return " ";
            }() << " connected with ";
            for (auto cntd__ : dev__.getIdConnectedDevices()) {
                std::cout << cntd__ << ", ";
            }
            std::cout << "\e[2D " << std::endl;
        }
    }
}
