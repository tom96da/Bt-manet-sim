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

using namespace std;

int main()
{
    double field_size = 60.0;
    int num_dev = 100;
    MGR mgr(field_size, num_dev);
    vector<ofstream> fs;

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
            auto &[x, y] = mgr.getPosition(id);
            fs[id] << x << ", " << y << endl;
        }
    };

    auto doSim = [&](const int frames)
    {
        newCsv();
        writeCsv();
        mgr.setDevices();
        mgr.sendHello();
        mgr.makeMPR();
        // mgr.showMPR(0);

        int f = 0;
        auto pbar = thread(
            [&]()
            {
                auto p = PBar(frames, f);
                // p.erase();
            });
        for (f = 0; f < frames; f++)
        {
            mgr.sendTable();
            mgr.makeTable();
            // mgr.updatePositionAll();
        }
        pbar.join();

        // mgr.startFlooding(45);
    };

    doSim(15);

    Device::showTotalPacket();

    return 0;
}
