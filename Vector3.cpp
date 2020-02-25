//ベクトルの定義
class Vector3
{
    public:
    //三次元
    double x, y, z;
    //コンストラクタ(初期化)
    Vector3(double _x = 0, double _y = 0, double _z = 0)
    {
        x = _x;
        y = _y;
        z = _z;
        }
        //セット
        Vector3 Set(double _x, double _y, double _z)
        {
            x = _x;
            y = _y;
            z = _z;
            return *this;
            }
            }

