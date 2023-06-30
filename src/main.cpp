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
    const double field_size = 60.0;
    /* ノード数 */
    const int num_node = 100;
    /* 座標を記録するファイル */
    auto fs = vector<ofstream>{};
    /* 試行回数 */
    const int num_repeat = 2;
    /* 結果 */
    vector<tuple<int, double, int64_t>> result_exiting, result_proposal_1,
        result_proposal_2;

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

    auto doSim = [&](const int num_repeat) {
        auto pbar = PBar();
        auto &pb_repeat = pbar.add();
        pb_repeat.set_title("Simulation progress");
        auto &pb_proposal_2 = pbar.add();
        auto &pb_proposal_1 = pbar.add();
        auto &pb_exiting = pbar.add();

        int count_repeat = 0;
        pb_repeat.start(num_repeat, count_repeat);
        newCsv();

        for (; count_repeat < num_repeat;) {
            mgr.setSimMode(SIMMODE::EXITING);

            // 孤立しないネットワークを構築する
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

            /* 完成するまでテーブルを更新する */
            auto makingTableUntilcomplete =
                [&mgr, num_node](
                    std::vector<std::tuple<int, double, int64_t>> &result,
                    ProgressBar::BarBody &pb, string pb_title) {
                    int num_packet_start = Device::getTotalPacket();
                    int num_packet_end = 0;
                    int num_done = 0;
                    int num_update = 0;
                    pb.set_title(pb_title);
                    pb.start(num_node, num_done);

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

                    pb.close();
                    result.emplace_back(num_packet_end - num_packet_start,
                                        num_update,
                                        static_cast<int>(pb.time()));
                };

            {  // 既存手法
                mgr.sendHello();
                mgr.makeMPR();
                // mgr.showMPR(0);

                makingTableUntilcomplete(result_exiting, pb_exiting, "EXITING");
                // pb_make_table.erase();

                // mgr.getDeviceById(0).calculateTableFrequency();
            }

            mgr.clearDevice();

            {  // 提案手法 遠距離選択MPR
                mgr.setSimMode(SIMMODE::PROPOSAL_LONG_MPR);

                mgr.sendHello();
                mgr.makeMPR();
                // mgr.showMPR(0);

                makingTableUntilcomplete(result_proposal_1, pb_proposal_1,
                                         "LONG_MPR");

                // pb_make_table.erase();
            }

            mgr.resetNetwork();

            {  // 提案手法 遠距離選択接続
                mgr.setSimMode(SIMMODE::PROPOSAL_LONG_CONNECTION);
                mgr.buildNetwork();
                const auto [_, num_member] = mgr.flooding(45);
                if (num_member != num_node) {
                    // 孤立するノードがあれば上の2つの結果を消してループに戻る
                    result_exiting.pop_back();
                    result_proposal_2.pop_back();
                    continue;
                }

                mgr.sendHello();
                mgr.makeMPR();
                // mgr.showMPR(0);

                makingTableUntilcomplete(result_proposal_2, pb_proposal_2,
                                         "LONG_CONNECTION");
                // pb_make_table.erase();
            }

            ++count_repeat;
        }
        pb_repeat.close();
    };

    doSim(num_repeat);

    auto average_exiting = average(result_exiting);
    showAverage("EXITING", average_exiting);
    auto average_proposal_1 = average(result_proposal_1);
    showAverage("PROPOSAL", average_proposal_1);
    auto average_proposal_2 = average(result_proposal_2);
    showAverage("PROPOSAL", average_proposal_2);

    return 0;
}
