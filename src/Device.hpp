/*!
 * @file Device.hpp
 * @author tom96da
 * @brief Device クラスの定義
 * @date 2023-05-11
 */

#ifndef DEVICE_HPP
#define DEVICE_HPP

class Device
{
private:
    const int id_;

public:
    Device(int id);

    int getId() const;
};

#endif