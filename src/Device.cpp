/*!
 * @file Device.cpp
 * @author tom96da
 * @brief Device クラスのメンバ関数
 * @date 2023-05-11
 */

#include <iostream>
#include "Device.hpp"

Device::Device(int id, int max_connections)
    : id_(id),
      max_connections_(max_connections),
      name_([&id]() -> string
            { return "device[" + to_string(id) + "]"; }())
{
}

int Device::getId() const { return id_; }
string Device::getName() const { return name_; }

void Device::hello() const
{
    cout << getName() << " hello!" << endl;
}
pair<double, double> Device::getVelocity()
{
    return velocity_;
}
void Device::setVelocity()
{
    velocity_ = {0.0, 0.0};
}
