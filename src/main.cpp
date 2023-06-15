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

    auto showMPR = [&](int id)
    {
        auto dev = mgr.getDeviceById(id);
        auto cntds = dev.getConnectedDeviceId();
        std::cout << dev.getName() << " connected with ";
        for (auto cntd : cntds)
            std::cout << cntd << ", ";
        std::cout << std::endl;
        auto MPR = dev.getMPR();
        for (auto &&cntd : cntds)
        {
            auto dev__ = mgr.getDeviceById(cntd);
            std::cout << dev__.getName()
                      << [&]
            {
                if (MPR.count(cntd))
                    return "*";

                return " ";
            }() << " connected with ";
            for (auto &&cntd__ : dev__.getConnectedDeviceId())
                std::cout << cntd__ << ", ";
            std::cout << std::endl;
        }
    };

    auto doSim = [&](const int frames)
    {
        newCsv();
        writeCsv();
        mgr.setDevices();
        mgr.sendHello();
        mgr.makeMPR();

        int f = 0;
        auto pbar = thread(
            [&]()
            {
                auto p = PBar(frames, f);
                p.erase();
            });
        for (f = 0; f < frames; f++)
        {
            mgr.sendTable();
            mgr.makeTable();
            // mgr.updatePositionAll();
        }
        // mgr.startFlooding(45);
        pbar.join();
    };

    doSim(1);
    // showMPR(0);

    Device::showTotalPacket();

    return 0;
}
