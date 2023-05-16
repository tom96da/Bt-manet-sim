/*!
 * @file DeviceManager.hpp
 * @author tom96da
 * @brief DeviceManager クラスの定義
 * @date 2023-05-15
 */

#ifndef DEVICEMANAGER_HPP
#define DEVICEMANAGER_HPP

#include <vector>
#include <random>
#include "Device.hpp"
using namespace std;

/* デバイスマネージャー クラス */
class DeviceManager
{
private:
    vector<Device> devices_;
    vector<pair<double, double>> positions_;

    /* メルセンヌ・ツイスタ */
    mt19937 mt_;
    /* 一様実数分布 座標 */
    uniform_real_distribution<> position_random_;
    /* 一様実数分布 移動角度 */
    uniform_real_distribution<> move_random_;
    /* 正規分布 移動距離 */
    normal_distribution<> move_randn_;

public:
    DeviceManager(int init_num_devices = 0);

    int getNumDevices() const;
    Device *getDeviceById(const int id);

    // pair<double, double> *getPositon(int id);
    void updatePisition(int id);
    pair<double, double> *getPositon(int id);

    void addDevices(int num_devices);

    void pairDevices(const int d1_id, const int d2_id);
    void conectDevices(const int d1_id, const int d2_id);
};

/* デバイスマネージャー クラス */
using MGR = DeviceManager;

#endif