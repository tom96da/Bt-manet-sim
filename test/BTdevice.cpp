#include <iostream>
#include <string>
#include <random>

using namespace std;

constexpr int MAX_CONNECTIONS = 6;

/* Bluetooth device class */
class Device
{
public:
    Device(int id, int max_connections = MAX_CONNECTIONS)
        : id_(id),
          max_connections_(max_connections),
          name_([&id]() -> string
                { return "device[" + to_string(id) + "]"; }()),
          velocity(this) {}

    void send(Device &receiver, string message)
    {
        cout << "ID_" << getId() << " -> " << message << " -> ID_" << receiver.getId() << flush;
        receiver.receive(*this, message);
    }
    void receive(Device &sender, string message)
    {
        cout << " : success!!" << endl;
    }

    int getId() const { return id_; }
    string getName() const { return name_; }
    int getMaxConnections() const { return max_connections_; }
    pair<double, double> getVelocity() { return velocity.getVelocity(); }

    class Velocity
    {
    public:
        Velocity(Device *device) : x_(0), y_(0), device_(device) {}

        pair<double, double> getVelocity() { return {x_, y_}; }
        double getX() { return x_; }
        double getY() { return y_; }

    private:
        double x_;
        double y_;

        Device *device_;
    };
    Velocity velocity;

private:
    const int id_;
    const string name_;
    const int max_connections_;

    vector<Device *> pairedDevices;
    vector<Device *> connectedDevices;

    double getRandomVelocity() const
    {
        // -1.0 から 1.0 の間の値をランダムに生成
        return (double)rand() / RAND_MAX * 2.0 - 1.0;
    }
};

/* device manager class */
class DeviceManager
{
public:
    DeviceManager() {}

    void setDevice(int num_devices)
    {
        for (int id = 0; id < num_devices; id++)
        {
            devices_.push_back(Device(id));
        }
    }
    void addDevice(int num_devices)
    {
        int pre_num_devices = devices_.size();
        for (int id = pre_num_devices; id < pre_num_devices + num_devices; id++)
        {
            // devices_.push_back(Device(id));
            devices_.emplace_back(id);
        }
    }
    void communicateDevices(int sender_id, int receiver_id, string message)
    {
        devices_[sender_id].send(devices_[receiver_id], message);
    }

    int getNumDevices() const { return devices_.size(); };
    Device &getDevice(const int id) { return devices_[id]; };

private:
    vector<Device> devices_;
};

int main()
{
    int num_devices = 100;
    DeviceManager manager;

    // string message = "Hello!";

    // devices[0].send(devices[1], message);

    manager.setDevice(num_devices);
    manager.addDevice(3);

    Device &d1 = manager.getDevice(102);
    Device &d2 = manager.getDevice(100);
    d1.send(d2, "Hello!");
    manager.communicateDevices(100, 102, "Hi!");

    return 0;
}
