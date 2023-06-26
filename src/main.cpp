/*!
 * @file main.cpp
 * @author tom96da
 * @brief Bluetooth MANET のシミュレーション
 * @date 2023-05-11
 */

#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

#include "Device.hpp"
#include "DeviceManager.hpp"
#include "pbar.hpp"

using namespace std;

int main() {
    double field_size = 60.0;
    int num_dev = 100;
    auto fs = vector<ofstream>{};

    const int num_repeat = 1;
    vector<tuple<int, double, int64_t>> result;

    auto mgr = MGR{field_size};

    // auto newCsv = [&]() {
    //     for (int id = 0; id < num_dev; id++) {
    //         string fname = "../tmp/dev_pos" + to_string(id) + ".csv";
    //         fs.emplace_back(fname);
    //         fs[id] << "x,y" << endl;
    //     }
    // };

    // auto writeCsv = [&]() {
    //     for (int id = 0; id < num_dev; id++) {
    //         auto [x, y] = mgr.getPosition(id);
    //         fs[id] << x << ", " << y << endl;
    //     }
    // };

    auto doSim = [&](const int repeat) {
        mgr.setSimMode(SIMMODE::EXITING);

        auto pbar = PBar();
        auto &pb_make_table = pbar.add();
        pb_make_table.set_title("Making routing table");
        // newCsv();

        for (int i = 0; i < repeat; ++i) {
            while (true) {
                mgr.deleteDeviceAll();
                mgr.addDevices(num_dev);
                mgr.buildNetwork();
                const auto [_, num_member] = mgr.flooding(45);

                if (num_member == num_dev) {
                    break;
                }
            }

            // writeCsv();

            mgr.sendHello();
            mgr.makeMPR();

            /* make Routing Table */
            {
                int num_packet_start = Device::getTotalPacket();
                int num_packet_end = 0;
                int num_done = 0;
                int num_update = 0;
                pb_make_table.start(num_dev, num_done);

                while (true) {
                    mgr.sendTable();
                    num_done = num_dev - mgr.makeTable();
                    if (num_done < num_dev) {
                        num_packet_end = Device::getTotalPacket();
                        ++num_update;
                    } else {
                        break;
                    }
                }

                pb_make_table.close();
                result.emplace_back(num_packet_end - num_packet_start,
                                    num_update,
                                    static_cast<int>(pb_make_table.time()));
            }
            mgr.resetNetwork();
        }
        pb_make_table.erase();
    };

    doSim(num_repeat);
    std::cout << num_repeat << " tiomes complete." << std::endl;

    auto average = [&]() -> tuple<int, double, int64_t> {
        auto sum = reduce(
            result.begin(), result.end(), tuple<int, double, int64_t>{0, 0, 0},
            [&](const auto &acc,
                const auto &elem) -> tuple<int, double, int64_t> {
                return {get<0>(acc) + get<0>(elem), get<1>(acc) + get<1>(elem),
                        get<2>(acc) + get<2>(elem)};
            });
        return {get<0>(sum) / num_repeat, get<1>(sum) / num_repeat,
                get<2>(sum) / num_repeat};
    }();

    std::cout << "average packets: " << get<0>(average)
              << ", update: " << get<1>(average)
              << ", time: " << get<2>(average) << " msec" << std::endl;

    return 0;
}
