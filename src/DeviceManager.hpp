/*!
 * @file DeviceManager.hpp
 * @author tom96da
 * @brief DeviceManager クラスの定義
 * @date 2023-05-15
 */

#ifndef DEVICEMANAGER_HPP
#define DEVICEMANAGER_HPP

#include <map>
#include <random>
#include <vector>

#include "Device.hpp"
using namespace std;

/* 接続可能距離 */
const double MAX_COM_DISTANCE = 10.0;

/* デバイスマネージャー クラス */
class DeviceManager {
   public:
    /* シミュレーションモード列挙型 */
    enum class SimulationMode;

   private:
    /* 接続可能距離 */
    static double _max_com_distance_;

    /* フィールドサイズ */
    const double field_size_;

    /* シミュレーションモード */
    SimulationMode sim_mode_;

    /* ノード クラス */
    class Node;
    /*すべてのデバイス */
    map<int, Node> nodes_;

    /* メルセンヌ・ツイスタ */
    mt19937 mt_;
    /* 一様実数分布 座標 */
    uniform_real_distribution<> position_random_;
    /* 正規分布 移動距離 */
    normal_distribution<> move_randn_;
    /* 一様実数分布 バイアス */
    uniform_real_distribution<> bias_random_;
    /* 一様分布 willingness */
    uniform_int_distribution<> willingness_random_;

    /* 出力モード列挙型 */
    enum class WriteMode;

   public:
    DeviceManager(const double field_size);

    static double getMaxComDistance();

    void setSimMode(const SimulationMode sim_mode);

    int getNumDevices() const;
    Node &getDeviceById(const int id);
    pair<double, double> &getPosition(const int id);
    double getDistance(const int id_1, const int id_2);

    void addDevices(const int num_devices);
    void removeDevice(const int id);
    void deleteDeviceAll();

    void pairDevices(const int id_1, const int id_2);
    void unpairDevices(const int id_1, const int id_2);
    void connectDevices(const int id_1, const int id_2);
    void disconnectDevices(const int id_1, const int id_2);
    void disconnectDevices(const int id);

    void updatePosition(const int id);
    void updatePositionAll();
    void relocatePosition();

    void clearDevice();

    void buildNetwork();
    void buildNetworkRandom();
    void buildNetworkByDistance();
    void resetNetwork();

    void sendHello();
    void sendTable();

    void makeMPR();
    void showMPR(const int id);

    int makeTable();

    pair<size_t, int> flooding(const int id);
    int aggregateDevices(size_t data_id);
    int aggregateDevices(size_t data_id, set<int> &devices_have_data,
                         const WriteMode write_mode);

    void unicast(const int id_source, const int id_dest);

   private:
    vector<int> getDevicesList() const;
    pair<double, double> &getBias(const int id);

    bool isSameDevice(const int id_1, const int id_2) const;
    bool isPaired(const int id_1, const int id_2);
    bool isConnected(const int id_1, const int id_2);
};

/* デバイスマネージャー クラス */
using MGR = DeviceManager;

/* シミュレーションモード */
enum class DeviceManager::SimulationMode {
    NONE,
    EXITING,                  /* 既存手法 */
    PROPOSAL_LONG_CONNECTION, /* 提案手法 遠距離接続 */
    PROPOSAL_LONG_MPR         /* 提案手法 遠距離MPR */
};
using SIMMODE = DeviceManager::SimulationMode;

/* ノード クラス */
class DeviceManager::Node : public Device {
   private:
    /* 移動バイアス */
    pair<double, double> bias_;
    /* 座標 */
    pair<double, double> position_;
    /* マネージャー */
    MGR *manager_;

   public:
    Node(const int id, const int willingness, MGR *manager);

    string getName() override;
    pair<double, double> &getBias();
    pair<double, double> &getPosition();

    void setBias(double bias_x, double bias_y);
    void setPositon(double pos_x, double pos_y);

    void makeMPR() override;
};

/* 出力モード列挙型 */
enum class DeviceManager::WriteMode {
    DEFAULT, /* デフォルト 最小限表示 */
    HIDE,    /* 非表示 */
    SIMPLE,  /* 簡易表示 */
    ARRAY,   /* 配列表示 */
    VISIBLE  /* 詳細表示 */
};

#include "DeviceManager.cpp"

#endif