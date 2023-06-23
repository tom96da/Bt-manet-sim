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
#include <vector>

using namespace std;

int main()
{
    double field_size = 60.0;
    int num_dev = 100;
    SIMMODE sim_mode = SIMMODE::EXITING;
    auto fs = vector<ofstream>{};

    const int num_repeat = 20;
    vector<pair<int, int>> result;

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
        auto pbar = PBar();
        auto &pb_make_table = pbar.add();
        pb_make_table.set_title("Making routing table");
        newCsv();

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

            writeCsv();

            mgr.sendHello();
            mgr.makeMPR();

            /* make Routing Table */
            {
                int num_packet_start = Device::getTotalPacket(),
                    num_packet_end = 0,
                    num_done = 0;
                pb_make_table.start(num_dev, num_done);

                while (num_done < num_dev)
                {
                    num_packet_end = Device::getTotalPacket();
                    mgr.sendTable();
                    num_done = num_dev - mgr.makeTable();
                }

                pb_make_table.close();
                result.emplace_back(num_packet_end - num_packet_start,
                                    static_cast<int>(pb_make_table.time()));
            }
        }
        pb_make_table.erase();
    };

    doSim(num_repeat);
    std::cout << num_repeat << " tiomes complete." << std::endl;

    auto average = [&]() -> std::pair<int, int64_t>
    {
        auto sum = reduce(
            result.begin(), result.end(), std::pair<int, int64_t>{0, 0},
            [&](const auto &acc, const auto &elem) -> std::pair<int, int64_t>
            {
                return {acc.first + elem.first, acc.second + elem.second};
            });
        return {sum.first / num_repeat, sum.second / num_repeat};
    }();

    std::cout << "average packets: " << average.first
              << ", time: " << average.second << " msec" << std::endl;

    return 0;
}
