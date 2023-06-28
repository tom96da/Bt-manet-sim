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
    /* フィールドサイズ */
    double field_size = 60.0;
    /* ノード数 */
    int num_node = 100;
    /* 座標を記録するファイル */
    auto fs = vector<ofstream>{};
    /* 試行回数 */
    const int num_repeat = 1;
    /* 結果 */
    vector<tuple<int, double, int64_t>> result_exiting, result_proposal;

    /*　マネージャー */
    auto mgr = MGR{field_size};

    /* ファイル作成 */
    auto newCsv = [&]() {
        for (int id = 0; id < num_node; id++) {
            string fname = "../tmp/dev_pos" + to_string(id) + ".csv";
            fs.emplace_back(fname);
            fs[id] << "x,y" << endl;
        }
    };
    /* ファイルに座標書き込み */
    auto writeCsv = [&]() {
        for (int id = 0; id < num_node; id++) {
            auto [x, y] = mgr.getPosition(id);
            fs[id] << x << ", " << y << endl;
        }
    };
    /* 結果から平均を得る */
    auto average = [&](vector<tuple<int, double, int64_t>> result)
        -> tuple<int, double, int64_t> {
        auto sum = reduce(
            result.begin(), result.end(), tuple<int, double, int64_t>{0, 0, 0},
            [&](const auto &acc,
                const auto &elem) -> tuple<int, double, int64_t> {
                return {get<0>(acc) + get<0>(elem), get<1>(acc) + get<1>(elem),
                        get<2>(acc) + get<2>(elem)};
            });
        return {get<0>(sum) / num_repeat, get<1>(sum) / num_repeat,
                get<2>(sum) / num_repeat};
    };
    /* 平均を表示 */
    auto showAverage = [](string s, tuple<int, double, int64_t> average) {
        std::cout << s << " average packets: " << get<0>(average)
                  << ", update: " << get<1>(average)
                  << ", time: " << get<2>(average) << " msec" << std::endl;
    };

    auto doSim = [&](const int repeat) {
        auto pbar = PBar();
        auto &pb_make_table = pbar.add();
        newCsv();

        for (int i = 0; i < repeat; ++i) {
            mgr.setSimMode(SIMMODE::EXITING);
            string pb_title = ("EXITING");
            pb_make_table.set_title(pb_title);

            while (true) {
                // 孤立するノードがないネットワークができるまで繰り返す
                mgr.deleteDeviceAll();
                mgr.addDevices(num_node);
                mgr.buildNetwork();
                const auto [_, num_member] = mgr.flooding(45);
                if (num_member == num_node) {
                    // 孤立するノードがなければ抜ける
                    break;
                }
            }

            writeCsv();

            mgr.sendHello();
            mgr.makeMPR();
            mgr.showMPR(0);

            /* make Routing Table */
            {
                int num_packet_start = Device::getTotalPacket();
                int num_packet_end = 0;
                int num_done = 0;
                int num_update = 0;
                pb_make_table.start(num_node, num_done);

                while (true) {
                    mgr.sendTable();
                    num_done = num_node - mgr.makeTable();
                    if (num_done < num_node) {
                        num_packet_end = Device::getTotalPacket();
                        ++num_update;
                    } else {
                        break;
                    }
                }

                pb_make_table.close();
                result_exiting.emplace_back(
                    num_packet_end - num_packet_start, num_update,
                    static_cast<int>(pb_make_table.time()));
            }

            pb_make_table.erase();
            mgr.getDeviceById(0).calculateTableFrequency();
            mgr.clearDevice();
            // mgr.resetNetwork();

            mgr.setSimMode(SIMMODE::PROPOSAL_LONG_MPR);
            // mgr.buildNetwork();
            pb_title = ("PROPOSAL_LONG_MPR");
            pb_make_table.set_title(pb_title);

            mgr.sendHello();
            mgr.makeMPR();
            mgr.showMPR(0);

            /* make Routing Table */
            {
                int num_packet_start = Device::getTotalPacket();
                int num_packet_end = 0;
                int num_done = 0;
                int num_update = 0;
                pb_make_table.start(num_node, num_done);

                while (true) {
                    mgr.sendTable();
                    num_done = num_node - mgr.makeTable();
                    if (num_done < num_node) {
                        num_packet_end = Device::getTotalPacket();
                        ++num_update;
                    } else {
                        break;
                    }
                }

                pb_make_table.close();
                result_proposal.emplace_back(
                    num_packet_end - num_packet_start, num_update,
                    static_cast<int>(pb_make_table.time()));
            }
        }
        pb_make_table.erase();
        mgr.getDeviceById(0).calculateTableFrequency();
    };

    doSim(num_repeat);

    std::cout << num_repeat << " tiomes complete." << std::endl;

    auto average_exiting = average(result_exiting);
    showAverage("EXITING", average_exiting);
    auto average_proposal = average(result_proposal);
    showAverage("PROPOSAL", average_proposal);

    return 0;
}
