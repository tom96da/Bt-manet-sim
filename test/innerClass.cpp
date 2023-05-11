/*!
 * @file innerClass.cpp
 * @author tom96da
 * @brief アウタークラスとインナークラスの相互アクセス
 * @date 2023-05-04
 */

#include <iostream>

using namespace std;

class OuterClass
{
public:
    OuterClass(int x, int y) : x_(x), inner(y, this) {}

    void OuterFunction()
    {
        cout << "OuterFunction: x = " << x_ << endl;
    }

    class InnerClass
    {
    public:
        InnerClass(int y, OuterClass *outer) : y_(y), outer_(outer) {}

        void innerFunction()
        {
            cout << "InnerFunction: y = " << y_ << endl;
            cout << "InnerFunction: x = " << outer_->x_ << endl;
        }

    private:
        int y_;

        OuterClass *outer_;
    };

    InnerClass inner;

private:
    int x_;
};

int main(void)
{
    int x = 3,
        y = 2;

    auto myclass = OuterClass(x, y);

    myclass.OuterFunction();
    myclass.inner.innerFunction();

    return 0;
}