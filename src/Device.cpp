/*!
 * @file Device.cpp
 * @author tom96da
 * @brief Device クラスのメンバ関数
 * @date 2023-05-11
 */

#include "Device.hpp"

Device::Device(int id)
    : id_(id)
{
}

int Device::getId() const { return id_; }
