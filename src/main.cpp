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
#include <thread>

using namespace std;

int main()
{
    double field_size = 60.0;
    int num_dev = 100;
    SIMMODE sim_mode = SIMMODE::PROPOSAL;

    auto mgr = MGR{field_size, sim_mode};
    mgr.addDevices(num_dev);
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
            auto [x, y] = mgr.getPosition(id);
            fs[id] << x << ", " << y << endl;
        }
    };

    newCsv();
    writeCsv();

    mgr.buildNetwork();
    const auto [_, num_member] = mgr.startFlooding(45);
    if (num_member < num_dev)
    {
        std::cout << "Smome nodes were not included in the network.\n"
                  << "Please retry." << std::endl;
        return 0;
    }

    mgr.clearMemory();
    Device::resetNumPacket();

    mgr.sendHello();
    mgr.makeMPR();
    int num_packet = 0;

    /* make Routing Table */
    {
        int num_done = 0;
        auto pbar = thread(
            [&]()
            {
                auto p = PBar(num_member, num_done);
                p.setTitle("Making routing table");
                p.start();
                // p.erase();
            });
        while (num_done < num_member)
        {
            num_packet = Device::getTotalPacket();
            mgr.sendTable();
            num_done = num_member - mgr.makeTable();
        }
        pbar.join();
    }
    std::cout << "Total packets: " << num_packet << std::endl;

    return 0;
}
