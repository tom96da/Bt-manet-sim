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
    double field_size = 60.0;
    int num_dev = 100;
    MGR mgr(field_size, num_dev);
    vector<ofstream> fs;

    auto manageDistance = [&](const int base_id)
    {
        for (int id = 0; id < num_dev; id++)
        {
            mgr.pairDevices(base_id, id);
            mgr.connectDevices(base_id, id);
            mgr.disconnectDevices(base_id, id);
        }
    };

    auto newCsv = [&]()
    {
        for (int id = 0; id < num_dev; id++)
        {
            string fname = "../tmp/dev_pos" + to_string(id) + ".csv";
            fs.emplace_back(fname);
            fs[id] << "x,y" << endl;
        }
    };

    auto writeCsv = [&]()
    {
        for (int id = 0; id < num_dev; id++)
        {
            auto [x, y] = *mgr.getPositon(id);
            fs[id] << x << ", " << y << endl;
        }
    };

    auto nextPos = [&]()
    {
        for (int id = 0; id < num_dev; id++)
        {
            mgr.updatePisition(id);
        }
    };

    auto doSim = [&](const int frames)
    {
        newCsv();

        for (int f = 0; f < frames; f++)
        {
            for (int id = 0; id < num_dev; id++)
            {
                manageDistance(id);
            }

            writeCsv();
            nextPos();
        }
    };

    doSim(1);

    for (int id = 0; id < num_dev; id++)
    {
        auto dev = mgr.getDeviceById(id);
        auto cntds = dev->getConnectedDeviceId();
        cout << "device" << dev->getId() << " connected with ";
        for (auto cntd : cntds)
        {
            cout << cntd << " ";
        }
        cout << endl;
    }

    return 0;
}
