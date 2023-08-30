/*!
 * @file DeviceManager.cpp
 * @author tom96da
 * @brief DeviceManager クラスのメンバ関数
 * @date 2023-05-15
 */

#include "DeviceManager.hpp"

#include <algorithm>
#include <cmath>
#include <execution>
#include <future>
#include <iomanip>
#include <iostream>
#include <numbers>
#include <utility>

/* デバイスマネージャー クラス */

/*!
 * @brief コンストラクタ
 * @param field_size フィールドサイズ
 * @param sim_mode シミュレーションモード
 */
DeviceManager::DeviceManager(const double field_size)
    : field_size_{field_size},
      mt_{random_device{}()},
      position_random_{0.0, field_size_},
      move_randn_{0, 0.3},
      bias_random_{-0.4, 0.4},
      willingness_random_{1, 5} {}

/* 最大接続距離 */
double DeviceManager::_max_com_distance_ = MAX_COM_DISTANCE;

/*!
 * @return double 接続可能距離
 */
double DeviceManager::getMaxComDistance() { return _max_com_distance_; }

/*!
 * @brief シミュレーションモードを設定する
 * @param sim_mode
 */
void DeviceManager::setSimMode(const SimulationMode sim_mode) {
    /* シミュレーションモード */
    sim_mode_ = sim_mode;

    switch (sim_mode_) {
        case SIMMODE::CONVENTIONAL:
            /* 従来手法 */
            Device::setSimMode(Device::SimulationMode::CONVENTIONAL);
            break;
        case SIMMODE::PROPOSAL_LONG_CONNECTION:
            /* 提案手法 遠距離接続 */
            Device::setSimMode(
                Device::SimulationMode::PROPOSAL_LONG_CONNECTION);
            break;
        case SIMMODE::PROPOSAL_LONG_MPR:
            /* 提案手法 遠距離MPR */
            Device::setSimMode(Device::SimulationMode::PROPOSAL_LONG_MPR);
            break;
        default:
            std::cout
                << "Please execute after specifying "
                << "either CONVENTIONAL or PROPOSAL_LONG_CONNECTION for the "
                   "simulation mode."
                << std::endl;
            std::exit(EXIT_SUCCESS);
            break;
    }
}

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
 * @return デバイスIDのリスト
 */
vector<int> DeviceManager::getDevicesList() const {
    /* リスト */
    vector<int> list;
    for (auto &[id, _] : nodes_) {
        /* 順にリストに挿入する */
        list.emplace_back(id);
    }

    return list;
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
 * @brief デバイス間距離の取得
 * @param id_1 デバイスID1
 * @param id_2 デバイスID2
 * @retval 0< デバイス間距離
 * @retval <0 デバイスIDが不正
 */
double DeviceManager::getDistance(const int id_1, const int id_2) {
    if (!nodes_.count(id_1)) {
        /* デバイスが存在しなければ終了 */
        return -1.0;
    }
    if (!nodes_.count(id_2)) {
        /* デバイスが存在しなければ終了 */
        return -1.0;
    }

    auto &&[pos_1_x, pos_1_y] = getPosition(id_1);
    auto &&[pos_2_x, pos_2_y] = getPosition(id_2);

    return hypot(pos_1_x - pos_2_x, pos_1_y - pos_2_y);
}

/*!
 * @brief デバイスを追加する
 * @param num_devices デバイス数
 */
void DeviceManager::addDevices(const int num_devices) {
    /* 追加する先頭のデバイスID */
    int id_next = 0;
    if (getNumDevices() > 0) {
        auto &[id_last, _] = *nodes_.rend();
        id_next = id_last + 1;
    }

    for (int id = id_next; id < id_next + num_devices; id++) {
        /* 順にデバイスを生成する */
        nodes_.emplace(id, Node(id, willingness_random_(mt_), this));
        nodes_.at(id).setBias(bias_random_(mt_), bias_random_(mt_));
        nodes_.at(id).setPositon(position_random_(mt_), position_random_(mt_));
    }
}

/*!
 * @brief すべてのデバイスを削除する
 */
void DeviceManager::deleteDeviceAll() {
    Device::resetNumPacket();
    nodes_.clear();
}

/*!
 * @brief デバイス同士をペアリング登録させる
 * @param id_1 デバイスID1
 * @param id_2 デバイスID2
 */
void DeviceManager::pairDevices(const int id_1, const int id_2) {
    if (getDistance(id_1, id_2) > _max_com_distance_) {
        /* 距離が最大接続距離より離れていたら終了 */
        return;
    }
    if (getDistance(id_1, id_2) < 0) {
        /* デバイスが存在しなければ終了 */
        return;
    }
    if (isPaired(id_1, id_2)) {
        /* ペアリング済みなら終了 */
        return;
    }
    if (isSameDevice(id_1, id_2)) {
        /* 同一デバイスなら終了 */
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
        /* デバイスが存在しなければ終了 */
        return;
    }
    if (isSameDevice(id_1, id_2)) {
        /* 同一デバイスなら終了 */
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
        /* 距離が最大接続距離より離れていたら終了 */
        return;
    }
    if (getDistance(id_1, id_2) < 0) {
        /* デバイスが存在しなければ終了 */
        return;
    }
    if (!isPaired(id_1, id_2)) {
        /* ペアリング未登録なら終了 */
        return;
    }
    if (isSameDevice(id_1, id_2)) {
        /* 同一デバイスなら終了 */
        return;
    }

    if (getDeviceById(id_1).connect(id_2)) {
        /* デバイス1にデバイス2を接続出来たら */
        if (!getDeviceById(id_2).connect(id_1)) {
            /* デバイス2にデバイス1を接続できなかったら */
            getDeviceById(id_1).disconnect(id_2);
        }
    }
}

/*!
 * @brief 距離が離れたデバイス同士の接続を切る
 * @param d1_id デバイスID1
 * @param d2_id デバイスID2
 */
void DeviceManager::disconnectDevices(const int id_1, const int id_2) {
    if (getDistance(id_1, id_2) <= _max_com_distance_) {
        /* 距離が最大接続距離より小さければ終了 */
        return;
    }
    if (getDistance(id_1, id_2) < 0) {
        /* デバイスが存在しなければ終了 */
        return;
    }

    getDeviceById(id_1).disconnect(id_2);
    getDeviceById(id_2).disconnect(id_1);
}

/*!
 * @brief 距離が離れたデバイスとの接続をすべて切る
 * @param id デバイスID
 */
void DeviceManager::disconnectDevices(const int id) {
    for (auto id_cntd : getDeviceById(id).getIdConnectedDevices()) {
        /* 順に接続を切る */
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
    /* x軸方向変位 */
    double dx = move_randn_(mt_);
    /* y軸方向変位 */
    double dy = move_randn_(mt_);
    /* 座標 */
    auto &[pos_x, pos_y] = getPosition(id);
    /* 移動バイアス */
    auto &[bias_x, bias_y] = getBias(id);

    tmp = pos_x + dx + bias_x;
    if ((tmp > 0) & (tmp < field_size_)) {
        /* エリア外に出なければ */
        pos_x = tmp;
    } else {
        /* エリア外に出てしまうならば */
        pos_x -= (dx + bias_x);
        bias_x = -bias_x;
    }

    tmp = pos_y + dy + bias_y;
    if ((tmp > 0) & (tmp < field_size_)) {
        /* エリア外に出なければ */
        pos_y = tmp;
    } else {
        /* エリア外に出てしまうならば */
        pos_y -= (dy + bias_y);
        bias_y = -bias_y;
    }
}

/*!
 * @brief すべてのデバイスの座標の更新
 */
void DeviceManager::updatePositionAll() {
    for (auto id : getDevicesList()) {
        /* 順に差票を更新する */
        updatePosition(id);
    }
    buildNetwork();
}

/*!
 * @brief すべてのデバイスの記録をクリアする
 */
void DeviceManager::clearDevice() {
    for (auto id : getDevicesList()) {
        /* 順にクリアする */
        auto &device = getDeviceById(id);
        device.clearMPR();
        device.clearTable();
        device.clearMemory();
    }
}

/*!
 * @brief ネットワークを構築する
 */
void DeviceManager::buildNetwork() {
    switch (sim_mode_) {
        case SIMMODE::CONVENTIONAL:
        case SIMMODE::PROPOSAL_LONG_MPR:
            /* 従来手法 */
            buildNetworkRandom();
            break;
        case SIMMODE::PROPOSAL_LONG_CONNECTION:
            /* 提案手法 遠距離接続 */
            buildNetworkByDistance();
            break;
        default:
            std::cout
                << "Please execute after specifying "
                << "either CONVENTIONAL or PROPOSAL_LONG_CONNECTION for the "
                   "simulation mode."
                << std::endl;
            std::exit(EXIT_SUCCESS);
            break;
    }
}

/*!
 * @brief ランダムにネットワークを構築する
 */
void DeviceManager::buildNetworkRandom() {
    /* デバイスIDリスト */
    auto &&list_1 = getDevicesList();

    for (auto const id : list_1) {
        /* 順に距離の離れた接続を切る */
        disconnectDevices(id);
    }

    shuffle(list_1.begin(), list_1.end(), mt_);

    auto list_2 = list_1;

    for (const auto id_1 : list_1) {
        for (const auto id_2 : list_2) {
            /* 順に接続する */
            pairDevices(id_1, id_2);
            connectDevices(id_1, id_2);
        }

        /* 接続が完了したデバイスをリストから除外する */
        auto itr = find(list_2.begin(), list_2.end(), id_1);
        list_2.erase(itr);
        shuffle(list_2.begin(), list_2.end(), mt_);
    }
}

/*!
 * @brief デバイス間距離が大きい順にネットワークを構築する
 */
void DeviceManager::buildNetworkByDistance() {
    /* デバイスIDリスト */
    auto &&list = getDevicesList();
    for (auto id : list) {
        /* 順に距離の離れた接続を切る */
        disconnectDevices(id);
    }

    for (auto id_1 : list) {
        for (auto id_2 : list) {
            /* 順にペアリングする */
            pairDevices(id_1, id_2);
        }
    }

    for (auto id_1 : list) {
        /* ペアリング済みのデバイスを遠い順にリスト化する */
        auto &&id_pairs = getDeviceById(id_1).getIdPairedDevices();
        map<double, int, greater<double>> tmp;
        transform(id_pairs.begin(), id_pairs.end(), inserter(tmp, tmp.begin()),
                  [&](const int id_2) -> pair<double, int> {
                      return {getDistance(id_1, id_2), id_2};
                  });
        for (auto [_, id_2] : tmp) {
            /* 遠い順に接続する */
            connectDevices(id_1, id_2);
        }
    }
}

/*!
 * @brief ネットワークをリセットする
 */
void DeviceManager::resetNetwork() {
    for (auto id_1 : getDevicesList()) {
        /* 順にクリアする */
        auto &device = getDeviceById(id_1);
        device.clearMPR();
        device.clearTable();
        device.clearMemory();
        for (auto &&id_2 : device.getIdPairedDevices()) {
            /* 順にペアリング解除する */
            unpairDevices(id_1, id_2);
        }
    }
}

/*!
 * @brief すべてのデバイスにHelloパケットを送信させる
 */
void DeviceManager::sendHello() {
    for (auto id : getDevicesList()) {
        /* 順に送信する */
        getDeviceById(id).sendHello();
    }
}

/*!
 * @brief すべてのデバイスにルーティングテーブルを送信させる
 */
void DeviceManager::sendTable() {
    for (auto id : getDevicesList()) {
        /* 順に送信する */
        getDeviceById(id).sendTable();
    }
}

/*!
 * @brief すべてのデバイスにMPR集合を作成させる
 */
void DeviceManager::makeMPR() {
    for (auto id : getDevicesList()) {
        /* 順に作成させる */
        getDeviceById(id).makeMPR();
    }
}

/*!
 * @brief MPR集合を出力する
 * @param id デバイスID
 */
void DeviceManager::showMPR(const int id) {
    /* 出力モード */
    WriteMode write_mode = WriteMode::ARRAY;

    auto device_target = getDeviceById(id);
    auto id_cncts = device_target.getIdConnectedDevices();
    auto id_MPRs = device_target.getMPR();

    switch (write_mode) {
        case WriteMode::VISIBLE:
            std::cout << device_target.getName();
            std::cout << "  connected with ";
            break;
        case WriteMode::SIMPLE:
            std::cout << device_target.getName();
            std::cout << " ──> ";
            break;
        case WriteMode::ARRAY:
            std::cout << "target = " << id << std::endl;
        default:
            break;
    }

    if (write_mode == WriteMode::ARRAY) {
        std::cout << "neighbors = [";
    }

    for (auto id_cnct : id_cncts) {
        if (write_mode == WriteMode::SIMPLE && id_MPRs.count(id_cnct)) {
            continue;
        }
        if (write_mode == WriteMode::ARRAY && id_MPRs.count(id_cnct)) {
            continue;
        }
        std::cout << id_cnct << ", ";
    }
    std::cout << "\e[2D ";
    if (write_mode == WriteMode::ARRAY) {
        std::cout << "]";
    }
    std::cout << std::endl;

    if (write_mode == WriteMode::ARRAY) {
        std::cout << "MPRs = [";
        for (auto id_MPR : id_MPRs) {
            std::cout << id_MPR << ", ";
        }
        std::cout << "\e[2D]" << std::endl;
        std::cout << "tow_hop = [";
    }

    for (auto id_cnct : id_cncts) {
        if (write_mode == WriteMode::SIMPLE && (id_MPRs.count(id_cnct) == 0)) {
            continue;
        }
        if (write_mode == WriteMode::ARRAY && (id_MPRs.count(id_cnct) == 0)) {
            continue;
        }

        auto dev_cnct = getDeviceById(id_cnct);
        switch (write_mode) {
            case WriteMode::VISIBLE:
                std::cout << dev_cnct.getName() << [&] {
                    if (id_MPRs.count(id_cnct)) {
                        return "*";
                    }

                    return " ";
                }() << " connected with ";
                break;
            case WriteMode::SIMPLE:
                std::cout << string(device_target.getName().size(), ' ')
                          << " └─> " << id_cnct << " ──> ";
                break;
            case WriteMode::ARRAY:
                std::cout << "[";
                break;
            default:
                break;
        }
        for (auto cnct__ : dev_cnct.getIdConnectedDevices()) {
            if (write_mode == WriteMode::SIMPLE &&
                device_target.getTable().getIdNextHop(cnct__) != id_cnct) {
                continue;
            }
            if (write_mode == WriteMode::ARRAY &&
                device_target.getTable().getIdNextHop(cnct__) != id_cnct) {
                continue;
            }
            std::cout << cnct__ << ", ";
        }
        std::cout << "\e[2D ";
        if (write_mode == WriteMode::ARRAY) {
            std::cout << "],";
        }
        std::cout << std::endl;
    }
    if (write_mode == WriteMode::ARRAY) {
        std::cout << "\e[2D]" << std::endl;
    }
}

int DeviceManager::getCentralDevice() {
    auto list = getDevicesList();
    double center = field_size_ / 2;

    for (auto id : list) {
        const auto [pos_x, pos_y] = getPosition(id);
        double location = hypot(pos_x - center, pos_y - center);
        if (location < 5) {
            return id;
        }
    }

    return 0;
}

/*!
 * @brief すべてのデバイスにルーティングテーブルを作成させる
 * @return int 更新ありデバイス数
 */
int DeviceManager::makeTable() {
    int result = 0;

    for (auto id : getDevicesList()) {
        /* 順に作成させる */
        result += static_cast<int>(getDeviceById(id).makeTable());
    }

    return result;
}

/*!
 * @brief すべてのノードの度数分布を集計する
 * @return vector<map<int, double>> 領域別平均度数分布
 */
vector<map<int, double>> DeviceManager::calculateTableFrequency() {
    /* 度数分布 */
    map<int, double> frequency_central, frequency_middle, frequency_edge;
    /* 分布するデバイス数 */
    int num_central = 0, num_middle = 0, num_edge = 0;

    /* 中心点座標 */
    double center = field_size_ / 2;
    /* 各領域面積 */
    double area_zone = pow(field_size_, 2) / 3;
    /* 中央部半径 */
    double radius_central = sqrt(area_zone / numbers::pi);
    /* 中間部半径 */
    double radius_middle = sqrt(2 * area_zone / numbers::pi);

    /* 度数分布を加算する */
    auto mergeFrequency = [](map<int, double> &frequency_acc,
                             map<int, int> frequency_elem) {
        for (const auto &[num_hop, num_device] : frequency_elem) {
            frequency_acc[num_hop] += num_device;
        }
    };

    for (auto id : getDevicesList()) {
        /* 順に集計する */
        auto frequency_device = getDeviceById(id).calculateTableFrequency();

        /* 座標 */
        const auto [pos_x, pos_y] = getPosition(id);
        /* 中心点からの距離 */
        double location = hypot(pos_x - center, pos_y - center);

        if (location < radius_central) {
            /* 中央部にあれば */
            ++num_central;
            mergeFrequency(frequency_central, frequency_device);
        } else if (location < radius_middle) {
            /* 中間部にあれば */
            ++num_middle;
            mergeFrequency(frequency_middle, frequency_device);
        } else {
            /* 周縁部にあれば */
            ++num_edge;
            mergeFrequency(frequency_edge, frequency_device);
        }
    }

    /* 度数分布を除算する */
    auto divideFrequency = [](map<int, double> &frequency_acc,
                              const int divisor) {
        for (auto [num_hop, _] : frequency_acc) {
            frequency_acc[num_hop] /= divisor;
        }
    };

    divideFrequency(frequency_central, num_central);
    divideFrequency(frequency_middle, num_middle);
    divideFrequency(frequency_edge, num_edge);

    return {frequency_central, frequency_middle, frequency_edge};
}

/*!
 * @brief フラッディングを開始する
 * @param id 開始デバイスのID
 * @return pair<size_t, int> データ識別子, データ到達台数
 */
pair<size_t, int> DeviceManager::flooding(const int id) {
    /* 出力モード */
    WriteMode write_mode = WriteMode::HIDE;

    if (!nodes_.count(id)) {
        /* デバイスが存在しなければ終了 */
        return {0, 0};
    }

    auto &device_starter = getDeviceById(id);
    int num_devices_have_data = 0, num_step = 0;

    device_starter.flooding(-1);

    /* データが到達したデバイスリスト */
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
        /* フラッディングが止まるまで繰り返す */
        num_devices_have_data = devices_have_data.size();

        for (auto &[_, device] : nodes_) {
            /* 順にフラッディングをさせる */
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
        /* 順にデータを持っているか確認する */
        if (devices_have_data.count(id)) {
            /* すでにカウント済みならスルー */
            continue;
        }

        if (device.hasData(data_id)) {
            /* データを持っていればリストに挿入 */
            devices_have_data.emplace(id);
            if (static_cast<int>(write_mode) > 1) {
                std::cout << id << ", ";
            }
        }
    }

    return devices_have_data.size();
}

/*!
 * @brief ルーティングテーブルに従って通信する
 * @param id_source 発信元デバイスID
 * @param id_dest 宛先デバイスのID
 */
void DeviceManager::unicast(const int id_source, const int id_dest) {
    /* 出力モード */
    WriteMode write_mode = WriteMode::SIMPLE;

    if (getDistance(id_source, id_dest) < 0) {
        /* デバイスが存在しなければ終了 */
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
        /* 宛先に届くまで繰り返す */
        tie(id_nextHop, flag) = getDeviceById(id_nextHop).hopping();
        if (flag == 0) {
            /* 宛先に届けばループから離脱 */
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
 * @brief IDに合致するバイアスを参照 (不正なIDではないか事前確認)
 * @param id デバイスID
 * @return デバイスのバイアスの参照
 */
pair<double, double> &DeviceManager::getBias(const int id) {
    return nodes_.at(id).getBias();
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
DeviceManager::Node::Node(const int id, const int willingness, MGR *manager)
    : Device{id, willingness},
      bias_{0.0, 0.0},
      position_{0.0, 0.0},
      manager_{manager} {}

/*!
 * @brief デバイス名を取得(オーバーライド)
 * @return デバイス名
 */
string DeviceManager::Node::getName() {
    const int digit = to_string(manager_->getNumDevices() - 1).size();
    return "device[" + string(digit - to_string(getId()).length(), '0') +
           to_string(getId()) + "]";
}

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
 * @brief MPR集合を作成する(オーバーライド)
 */
void DeviceManager::Node::makeMPR() {
    /* シミュレーションモード */
    auto sim_mode = sim_mode_;

    /* 隣接ノード構造体 */
    struct Neighbor {
        int id;
        int willingness;
        double distance;
    };

    MPR_.clear();
    tow_hop_neighbors_.clear();
    /* 隣接ノード <id, willingness, distance> (降順) */
    vector<Neighbor> neighbors;

    while (true) {
        /* メモリ末尾から willingness 属性のデータを走査する */
        auto itr = find_if(memory_.rbegin(), memory_.rend(),
                           [](pair<size_t, Device::Sell> &&sell) {
                               auto &[_, data_in_sell] = sell;
                               return data_in_sell.getDataAttribute() ==
                                      DataAttr::WILLINGNESS;
                           });
        if (itr == memory_.rend()) {
            /* 見つからなければループを離脱 */
            break;
        }

        /* 見つかったデータの保存場所 */
        auto &[_, data_in_sell] = *itr;
        /* 隣接ノードのデバイスID */
        auto id_neighbor = data_in_sell.getIdSender();
        auto willingness = get<int>(data_in_sell.getData());

        neighbors.emplace_back(id_neighbor, willingness,
                               manager_->getDistance(getId(), id_neighbor));
        table_.setEntry(id_neighbor, id_neighbor, 1);

        data_in_sell.setDaTaAttribute(DataAttr::NONE);
    }

    /* neighbors を指標の降順にソート */
    sort(neighbors.begin(), neighbors.end(),
         [&](Neighbor &left, Neighbor &right) {
             switch (sim_mode) {
                 case Device::SimulationMode::CONVENTIONAL:
                 case Device::SimulationMode::PROPOSAL_LONG_CONNECTION:
                     /* 従来手法, 提案手法 遠距離接続 */
                     return left.willingness > right.willingness;
                 case Device::SimulationMode::PROPOSAL_LONG_MPR:
                     /* 提案手法 遠距離MPR 没案 */
                     return left.distance > right.distance;
                 default:
                     return false;
             }
         });

    for (auto [id_neighbor, _, __] : neighbors) {
        /* 隣接ノードのトポロジー情報から2ホップ隣接ノードをリスト化する */
        /* メモリ末尾からトポロジー情報を走査する */
        auto itr = find_if(memory_.rbegin(), memory_.rend(),
                           [&](pair<size_t, Device::Sell> &&sell) {
                               auto &[_, data_in_sell] = sell;
                               return data_in_sell.getDataAttribute() ==
                                          DataAttr::TOPOLOGY &&
                                      data_in_sell.getIdSender() == id_neighbor;
                           });
        if (itr == memory_.rend()) {
            /* 見つからなければループを離脱 */
            break;
        }
        /* 見つかったデータの保存場所 */
        auto &[___, data_in_sell] = *itr;

        for (auto id_neighbor_cncts = get<set<int>>(data_in_sell.getData());
             auto id_tow_hop_neighbor : id_neighbor_cncts) {
            /* 隣接ノードが接続中のノードを順に2ホップ隣接のリストに挿入する */
            if (isSelf(id_tow_hop_neighbor)) {
                /* 地震ならスルー */
                continue;
            }
            if (isConnected(id_tow_hop_neighbor)) {
                /* 他の隣接ノードならスルー */
                continue;
            }
            if (tow_hop_neighbors_.count(id_tow_hop_neighbor)) {
                /* すでにリストにあればスルー */
                continue;
            }

            tow_hop_neighbors_.emplace(id_tow_hop_neighbor, id_neighbor);
            table_.setEntry(id_tow_hop_neighbor, id_neighbor, 2);
        }
    }

    for (auto &[_, MPR] : tow_hop_neighbors_) {
        /* 順にMPRを記録する */
        MPR_.emplace(MPR);
    }
}
