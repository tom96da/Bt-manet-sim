/*!
 * @file main.cpp
 * @author tom96da
 * @brief Bluetooth MANET のシミュレーション
 * @date 2023-05-11
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Device.hpp"
#include "DeviceManager.hpp"

using namespace std;

int main(void)
{
    int num = 100;
    MGR mgr(num);
    vector<ofstream> fs;

    auto makeCSV = [&]()
    {
        for (int id = 0; id < num; id++)
        {
            string fname = "../tmp/dev_pos" + to_string(id) + ".csv";
            fs.emplace_back(fname);
            fs[id] << "x,y" << endl;
        }

        for (int i = 0; i < 100; i++)
        {
            for (int id = 0; id < num; id++)
            {
                auto [x, y] = *mgr.getPositon(id);
                fs[id] << x << ", " << y << endl;
                mgr.updatePisition(id);
            }
        }
    };

    mgr.pairDevices(0, 1);
    mgr.pairDevices(1, 2);
    auto d0 = mgr.getDeviceById(0);
    cout << d0->getNumPaired() << endl;
    auto d1 = d0->getPairedDevice(1);
    cout << d1->getNumPaired() << endl;

    // mgr.addDevices(num);
    // mgr.pairDevices(0, 1);
    // mgr.pairDevices(0, 2);
    // mgr.pairDevices(0, 3);
    // mgr.conectDevices(0, 1);

    // vector<Device> d;
    // cout << mgr.getNumDevices() << endl;
    // for (int id = 0; id < num; id++)
    //     d.push_back(*(mgr.getDeviceById(id)));

    // vector<int> ids = d[0].getPairedDeviceId();
    // for (int id : ids)
    // {
    //     cout << id << " ";
    // }
    // cout << endl;
    // ids = d[0].getConnectedDeviceId();
    // for (int id : ids)
    // {
    //     cout << id << " ";
    // }
    // cout << endl;

    return 0;
}
