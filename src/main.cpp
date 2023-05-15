/*!
 * @file main.cpp
 * @author tom96da
 * @brief Bluetooth MANET のシミュレーション
 * @date 2023-05-11
 */

#include <iostream>
#include <vector>
#include "Device.hpp"
#include "DeviceManager.hpp"

using namespace std;

int main(void)
{
    int num = 100;
    MGR mgr;

    mgr.addDevice(num);
    mgr.pairDevices(0, 1);
    mgr.pairDevices(0, 2);
    mgr.pairDevices(0, 3);
    mgr.conectDevices(0, 1);

    vector<Device> d;
    cout << mgr.getNumDevices() << endl;
    for (int id = 0; id < num; id++)
        d.push_back(*(mgr.getDeviceById(id)));

    vector<int> ids = d[0].getPairedDeviceId();
    for (int id : ids)
    {
        cout << id << " ";
    }
    cout << endl;
    ids = d[0].getConnectedDeviceId();
    for (int id : ids)
    {
        cout << id << " ";
    }
    cout << endl;

    return 0;
}
