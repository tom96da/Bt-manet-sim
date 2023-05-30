/*!
 * @file main.cpp
 * @author tom96da
 * @brief Bluetooth MANET のシミュレーション
 * @date 2023-05-11
 */

#include "DeviceManager.hpp"
#include "Device.hpp"
#include "pbar.hpp"
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

int main()
{
    double field_size = 60.0;
    int num_dev = 100;
    MGR mgr(field_size, num_dev);
    vector<ofstream> fs;

    auto runDevice = [&](const int base_id)
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
            mgr.updatePisition(id);
    };

    auto showPaired = [&](int id)
    {
        auto dev = mgr.getDeviceById(id);
        auto pards = dev->getPairedDeviceId();
        cout << dev->getName() << " paired with ";
        for (auto pard : pards)
            cout << pard << ", ";
        cout << endl;
    };

    auto showConnected = [&](int id)
    {
        auto dev = mgr.getDeviceById(id);
        auto cntds = dev->getConnectedDeviceId();
        cout << dev->getName() << " connected with ";
        for (auto cntd : cntds)
            cout << cntd << ", ";
        cout << endl;
    };

    auto doSim = [&](const int frames)
    {
        newCsv();

        for (int f = 0; f < frames; f++)
        {
            int id = 0;
            auto pbar = thread(
                [&]()
                {
                    auto p = PBar(num_dev, id);
                    // p.erase();
                });
            for (id = 0; id < num_dev; id++)
            {
                runDevice(id);
            }
            pbar.join();
            writeCsv();
            nextPos();
        }
    };

    doSim(1);

    showPaired(0);
    showConnected(0);
    auto dev0 = mgr.getDeviceById(0);
    auto cntds = dev0->getConnectedDeviceId();
    for (auto cntd : cntds)
    {
        dev0->sendPacket(cntd, dev0->makePacket(3.14));
    }

    return 0;
}
