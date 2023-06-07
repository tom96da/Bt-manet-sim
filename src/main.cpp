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

    auto runDevice = [&](int &progress)
    {
        for (int id_1 = 0; id_1 < num_dev; id_1++)
        {
            for (int id_2 = 0; id_2 < num_dev; id_2++)
            {
                mgr.pairDevices(id_1, id_2);
                mgr.connectDevices(id_1, id_2);
                mgr.disconnectDevices(id_1, id_2);
            }
            // mgr.getDeviceById(id_1).sendHello();
            progress = id_1 + 1;
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
            auto &[x, y] = mgr.getPosition(id);
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
        auto pards = dev.getPairedDeviceId();
        std::cout << dev.getName() << " paired with ";
        for (auto pard : pards)
            std::cout << pard << ", ";
        std::cout << endl;
    };

    auto showConnected = [&](int id)
    {
        auto dev = mgr.getDeviceById(id);
        auto cntds = dev.getConnectedDeviceId();
        std::cout << dev.getName() << " connected with ";
        for (auto cntd : cntds)
            std::cout << cntd << ", ";
        std::cout << endl;
    };

    auto showTotalPacket = [&]()
    {
        std::cout << "total packet: " << pcnt::getTotalPacket() << endl;
    };

    auto doSim = [&](const int frames)
    {
        // newCsv();
        int f = 0;
        // auto pbar_frames = thread(
        //     [&]()
        //     {
        //         auto p = PBar(frames, f);
        //         // p.erase();
        //     });

        for (f = 0; f < frames; f++)
        {
            int progress = 0;
            // auto pbar_dev = thread(
            //     [&]()
            //     {
            //         auto p = PBar(num_dev, progress);
            //         // p.erase();
            //     });

            runDevice(progress);

            // pbar_dev.join();

            // writeCsv();
            // nextPos();
        }
        // pbar_frames.join();
    };

    doSim(1);

    // showPaired(3);
    // showConnected(3);

    mgr.startFlooding(3);
    showTotalPacket();

    return 0;
}
