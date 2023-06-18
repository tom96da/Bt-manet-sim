/*!
 * @file Device.hpp
 * @author tom96da
 * @brief Device クラスの定義
 * @date 2023-05-11
 */

#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "routingTable.hpp"
#include <string>
#include <map>
#include <set>
#include <variant>

using namespace std;
using Var = variant<int, double, string, set<int>, Table>;

/* 最大接続数 */
const int MAX_CONNECTIONS = 6;

/* Bluetooth デバイスクラス */
class Device
{
private:
    /* 累計パケット数 */
    static int num_total_packe_;

    /* デバイスID */
    const int id_;
    /* 最大接続台数 */
    const int max_connections_;
    /* willingness */
    const int willingness_;

    /* 累計パケット生成数 */
    mutable int num_packet_made_;
    /* ペアリング登録済みデバイス */
    map<int, Device &> paired_devices_;
    /* 接続中デバイス */
    set<int> id_connected_devices_;
    /* MPR集合 */
    set<int> MPR_;
    /* ルーティングテーブル */
    Table table_;

    /* データ属性列挙型クラス */
    enum class DataAttr;

    /* メモリセルクラス */
    class Sell;
    /* メモリ */
    map<size_t, Sell> memory_;

    /* パケットクラス */
    class Packet;

public:
    Device(const int id, const int willingness);

    static int getTotalPacket();
    static int getNewSequenceNum();
    static void resetNumPacket();
    static void showTotalPacket();

    int getId() const;
    string getName() const;
    int getWillingness() const;

    int getNumPaired() const;
    int getNumConnected() const;
    set<int> getPairedDeviceId() const;
    set<int> getConnectedDevicesId() const;
    Table getTable() const;

    int getNumPacket() const;
    int getNewPacketId() const;

    set<int> getMPR() const;

    pair<size_t, Var> readData(const size_t data_id) const;

    bool isPaired(const int id_another_device) const;
    bool isConnected(const int id_another_device) const;
    bool hasData(const size_t data_id) const;

    void pairing(Device &another_device);
    void unpairing(const int id_another_device);
    bool connect(const int id_another_device);
    void disconnect(const int id_another_device);
    void saveData(pair<size_t, Var> data_with_id,
                  const DataAttr data_attr, int flood_step = 0);
    void saveData(const Packet &packet);
    void clearMemory();

    void sendMessage(const int id_receiver, string message);
    void receiveMessage(const int id_sender, string message);

    Packet makePacket(Var data) const;
    Packet makePacket(pair<size_t, Var> data_with_id,
                      const DataAttr data_attr, const int flood_step = 0) const;
    void sendPacket(const int id_receiver, const Packet &packet);
    void receivePacket(const Packet &packet);

    void sendHello();
    void sendTable();

    size_t makeFloodData();
    void flooding(const int flag = 0);

    void makeMPR();
    bool makeTable();

private:
    Device &getPairedDevice(const int id);

    bool isSelf(const int id_another_device) const;

    pair<size_t, Var> assignIdToData(const Var data, const bool is_flooding = false,
                                     size_t data_id = 0) const;
};

enum class Device::DataAttr
{
    NONE,
    WILLINGNESS,
    TOPOLOGY,
    TABLE,
    FLOODING,
    NEXT_FLOOD
};

/* メモリセルクラス */
class Device::Sell
{
private:
    /* 送信元デバイスのID */
    const int id_sender_;

    /* 識別子付きデータ */
    const pair<size_t, Var> data_with_id_;
    /* データ属性 */
    DataAttr data_attr_;

    /* フラッディングホップ数 */
    const int flood_step_;

public:
    Sell(const int id_sender, const pair<size_t, Var> data_with_id,
         const DataAttr data_attr, const int flood_step);

    int getSenderId() const;
    size_t getDataId() const;

    Var getData() const;
    pair<size_t, Var> getDataWithId() const;
    DataAttr getDataAttribute() const;
    int getFloodStep() const;

    void setDaTaAttribute(const DataAttr data_attr);
};

/* パケットクラス */
class Device::Packet
{
private:
    /* 送信元デバイスのID */
    const int id_sender_;
    /* パケットID */
    const int packet_id_;
    /* シーケンスナンバー */
    const int seq_num_;

    /* 識別子付きデータ */
    const pair<size_t, Var> data_with_id_;
    /* データ属性 */
    DataAttr data_attr_;

    /* フラッディングホップ数 */
    const int flood_step_;

public:
    Packet(const int id_sender, const int packet_id,
           const int seq_num, const pair<size_t, Var> data,
           const DataAttr data_attr, const int flood_step = 0);

    int getSenderId() const;
    int getPacketId() const;
    int getSeqNum() const;

    pair<size_t, Var> getDataWithId() const;
    DataAttr getDataAttribute() const;
    int getFloodStep() const;
};

#include "Device.cpp"

#endif // DEVICE_HPP
