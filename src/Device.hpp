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
#include <vector>
#include <any>
using namespace std;

/* 最大接続数 */
const int MAX_CONNECTIONS = 6;

/* パケットクラスの前方宣言 */
template <typename T>
class Packet;

/* Bluetooth デバイスクラス */
class Device
{
private:
    /* デバイスID */
    const int id_;
    /* 最大接続台数 */
    const int max_connections_;

    /* ペアリング登録済みデバイス */
    map<int, Device *> paired_devices_;
    /* 接続中デバイス */
    set<int> connected_devices_;
    /* メモリー */
    vector<any> memory_;
    /* 累計パケット生成数 */
    mutable int num_packet_made_;

public:
    Device(int id, int max_connections = MAX_CONNECTIONS);

    int getId() const;
    string getName() const;

    int getNumPaired() const;
    int getNumConnected() const;
    set<int> getPairedDeviceId() const;
    set<int> getConnectedDeviceId() const;
    int getNumPacket() const;
    int getNewPacketId() const;

    bool isPaired(const int another_device_id) const;
    bool isConnected(const int another_device_id) const;

    void pairing(Device &another_device);
    void removePairing(const int another_device_id);
    void connect(const int another_device_id);
    void disconnect(const int another_device_id);

    void sendMessage(const int receiver_id, string message);
    void receiveMessage(const int sender_id, string message);

    template <typename T>
    Packet<T> makePacket(T data) const;
    template <typename T>
    void sendPacket(const int receiver_id, const Packet<T> &packet);
    template <typename T>
    void receivePacket(const Packet<T> &packet);

    void hello() const;

private:
    Device *getPairedDevice(const int id);

    bool isSelf(const int another_device_id) const;
};

/* パケットクラス */
template <typename T>
class Packet
{
private:
    /* 送信元デバイスのID */
    const int sender_id_;
    /* パケットID */
    const int packet_id_;
    /* シーケンスナンバー */
    const int seq_num_;
    /* 送信するデータ */
    const T data_;

public:
    Packet(const int sender_id, const int packet_id, int seq_num, T data);

    int getSenderId() const;
    int getPacketId() const;
    int getSeqNum() const;

    T getData() const;
};

#include "Device.cpp"

#endif // DEVICE_HPP
