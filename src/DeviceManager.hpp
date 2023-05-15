/*!
 * @file DeviceManager.hpp
 * @author tom96da
 * @brief DeviceManager クラスの定義
 * @date 2023-05-15
 */

#ifndef DEVICEMANAGER_HPP
#define DEVICEMANAGER_HPP

#include <vector>
#include "Device.hpp"
using namespace std;

/* デバイスマネージャー クラス */
class DeviceManager
{
private:
    vector<Device> devices_;

public:
    DeviceManager(int init_num_devices = 0);

    int getNumDevices() const;
    Device *getDeviceById(const int id);

    void addDevice(int num_devices);

    void pairDevices(const int d1_id, const int d2_id);
    void conectDevices(const int d1_id, const int d2_id);
};

/* デバイスマネージャー クラス */
using MGR = DeviceManager;

#endif