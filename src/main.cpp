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
#include <future>

using namespace std;

int main()
{
    double field_size = 60.0;
    int num_dev = 100;
    SIMMODE sim_mode = SIMMODE::EXITING;
    auto fs = vector<ofstream>{};

    auto mgr = MGR{field_size, sim_mode};

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

    auto doSim = [&](const int repeat)
    {
        for (int i = 0; i < repeat; ++i)
        {
            bool retry = false;
            do
            {
                mgr.ResetManager();
                mgr.addDevices(num_dev);
                mgr.buildNetwork();
                const auto [_, num_member] = mgr.flooding(45);
                retry = false;

                if (num_member < num_dev)
                    retry = true;
            } while (retry);

            newCsv();
            writeCsv();

            // mgr.clearMemory();
            // Device::resetNumPacket();

            mgr.sendHello();
            mgr.makeMPR();

            /* make Routing Table */
            {
                int num_packet_start = Device::getTotalPacket(),
                    num_packet_end = 0;
                int num_done = 0;
                auto pbar = async(launch::async,
                                  [&]()
                                  {
                                      auto p = PBar(num_dev, num_done);
                                      p.setTitle("Making routing table");
                                      p.start();
                                      p.erase();
                                      return p.getTime();
                                  });
                while (num_done < num_dev)
                {
                    num_packet_end = Device::getTotalPacket();
                    mgr.sendTable();
                    num_done = num_dev - mgr.makeTable();
                }
                auto time = pbar.get();
                std::cout << "packets: " << num_packet_end - num_packet_start
                          << ", time: " << time << "msec" << std::endl;
            }
            // mgr.unicast(0, 99);
        }
    };

    doSim(10);
    std::cout << "complete." << std::endl;

    return 0;
}
