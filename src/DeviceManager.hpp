/*!
 * @file DeviceManager.hpp
 * @author tom96da
 * @brief DeviceManager クラスの定義
 * @date 2023-05-15
 */

#ifndef DEVICEMANAGER_HPP
#define DEVICEMANAGER_HPP

// #include <vector>
#include <random>
#include <map>
#include "Device.hpp"
using namespace std;

/* 接続可能距離 */
const double MAX_COM_DISTANCE = 10.0;

/* デバイスマネージャー クラス */
class DeviceManager
{
private:
    static double max_com_distance_;

    const double field_size_;

    /* ノード クラス */
    class Node
    {
    private:
        /* デバイス */
        Device device_;
        /* 移動バイアス */
        pair<double, double> bias_;
        /* 座標 */
        pair<double, double> position_;

    public:
        Node(const int id);

        Device *getDevice();
        pair<double, double> *getBias();
        pair<double, double> *getPosition();

        void setBias(double bias_x, double bias_y);
        void setPositon(double pos_x, double pos_y);
    };

    /*すべてのデバイス */
    map<int, Node> devices_;

    /* メルセンヌ・ツイスタ */
    mt19937 mt_;
    /* 一様実数分布 座標 */
    uniform_real_distribution<> position_random_;
    /* 一様実数分布 移動角度 */
    uniform_real_distribution<> move_random_;
    /* 正規分布 移動距離 */
    normal_distribution<> move_randn_;
    /* 一様実数分布 バイアス */
    uniform_real_distribution<> bias_random_;

public:
    DeviceManager(double field_size, int init_num_devices = 0);

    int getNumDevices() const;
    Device *getDeviceById(const int id);

    pair<double, double> *getPositon(const int id);
    pair<double, double> *getBias(const int id);
    void updatePisition(const int id);
    double getDistance(const int id_1, const int id_2);

    void addDevices(int num_devices);

    void pairDevices(const int id_1, const int id_2);
    void removePairingDevice(const int id_1, const int id_2);
    void connectDevices(const int id_1, const int id_2);
    void disconnectDevices(const int id_1, const int id_2);

    static double getMaxComDistance() { return max_com_distance_; }
};

/* デバイスマネージャー クラス */
using MGR = DeviceManager;

#endif