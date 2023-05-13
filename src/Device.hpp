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
using namespace std;

const int MAX_CONNECTIONS = 6;

// Bluetooth デバイス
class Device
{
private:
    const int id_;
    const string name_;
    const int max_connections_;

    pair<double, double> velocity_;

    vector<Device *> paired_devices_;
    vector<int *> connected_devices_;

public:
    Device(int id, int max_connections = MAX_CONNECTIONS);

    int getId() const;
    string getName() const;

    void setVelocity();
    pair<double, double> getVelocity();

    int getNumPaired() const;
    vector<int> getPairedDeviceId() const;
    bool isPaired(const Device &otherDevice) const;

    vector<int> getConnectedDeviceId();

    void hello() const;
    void pairing(Device &otherDevice);
    void removePairing(); //**
    void connect(Device &otherDevice);
    void disconnect(); //**
    void sendMessage(Device &receiver, string message);
    void receiveMessage(Device &sender, string message);
};

#endif
