/*!
 * @file Device.hpp
 * @author tom96da
 * @brief Device クラスの定義
 * @date 2023-05-11
 */

#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
using namespace std;

/* 最大接続数 */
const int MAX_CONNECTIONS = 6;

/* Bluetooth デバイス */
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

public:
    Device(const int id, int max_connections = MAX_CONNECTIONS);

    int getId() const;
    string getName() const;

    int getNumPaired() const;
    int getNumConnected() const;
    vector<int> getPairedDeviceId() const;
    set<int> getConnectedDeviceId() const;

    bool isPaired(const int another_device_id) const;
    bool isConnected(const int another_device_id) const;

    void pairing(Device &another_device);
    void removePairing(const int another_device_id);

    void connect(const int another_device_id);
    void disconnect(const int another_device_id);

    void sendMessage(const int receiver_id, string message);
    void receiveMessage(const int sender_id, string message);

    void hello() const;

private:
    Device *getPairedDevice(const int id);

    bool isSelf(const int another_device_id) const;
};

#endif
