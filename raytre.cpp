
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#define WIDTH 400       //横のピクセル数
#define HEIGHT 300      //縦のピクセル数
#define PI 3.14159      //円周率
#define SAMPLING_SIZE 5 //サンプリング数

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
    //四則演算
    Vector3 operator+(const Vector3 &b) const { return Vector3(x + b.x, y + b.y, z + b.z); }
    Vector3 operator-(const Vector3 &b) const { return Vector3(x - b.x, y - b.y, z - b.z); }
    Vector3 operator-() const { return Vector3(-x, -y, -z); }
    Vector3 operator*(double b) const { return Vector3(x * b, y * b, z * b); }
    //正規化
    Vector3 &Norm() { return *this = *this * (1 / sqrt(x * x + y * y + z * z)); }
    //内積
    double Dot(const Vector3 &b) const { return x * b.x + y * b.y + z * b.z; }
    //外積
    Vector3 Cross(const Vector3 &b) { return Vector3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }
    //長さ
    float Length() { return sqrt(x * x + y * y + z * z); }
    //表示
    void Print() { printf("( %.3f, %.3f, %.3f)¥n", x, y, z); }
};
//反射情報を保存
class HitRecord
{
public:
    float solveT;
    //光線の解
    Vector3 position;
    //反射した座標位置
    Vector3 normal;
    //法線ベクトル
};
HitRecord hitRecord;

//カラークラス
class Color
{
public:
    int r, g, b;
    Color() {} //クラスでの宣言で必要
    //基本はこちらを使用
    Color(int _r, int _g, int _b)
    {
        r = _r;
        g = _g;
        b = _b;
    }
    //セット
    void Set(int _r, int _g, int _b)
    {
        r = _r;
        g = _g;
        b = _b;
    }
    Color VectorToColor(Vector3 v)
    {
        v.Norm();
        //虹色に色付けする
        Vector3 colorVector = (v + Vector3(1, 1, 1)) * 0.5;
        r = (int)(255 * colorVector.x);
        g = (int)(255 * colorVector.y);
        b = (int)(255 * colorVector.z);
        return Color(r, g, b);
    }
    //加算
    Color operator+(const Color &left) const
    {
        int R = r + left.r;
        int G = g + left.g;
        int B = b + left.b;
        if (R > 255)
            R = 255;
        if (G > 255)
            G = 255;
        if (B > 255)
            B = 255;
        return Color(R, G, B);
    }
    //減算
    Color operator-(const Color &left)
    {
        int R = r - left.r;
        int G = g - left.g;
        int B = b - left.b;
        if (R < 0)
            R = 0;
        if (G < 0)
            G = 0;
        if (B < 0)
            B = 0;
        return Color(R, G, B);
    }
    Color operator*(const float B) const
    {
        return Color((int)(B * r), (int)(B * g), (int)(B * b));
    }
    Color operator*(const Color &left) const
    {
        return Color((int)(r * left.r / 255), (int)(g * left.g / 255), (int)(b * left.b / 255));
    }
    Color operator/(const double t) const
    {
        return Color((int)(r / t), (int)(g / t), (int)(b / t));
    }
};

class Ray
{
public:
    //変数
    Vector3 origin;
    //中心座標
    Vector3 direction;
    //方向
    //初期化
    Ray(Vector3 o, Vector3 d)
    {
        origin = o;
        direction = d;
    }
    //セット
    void Set(Vector3 o, Vector3 d)
    {
        origin = o;
        direction = d;
    }
};

class Camera
{
private:
    Vector3 X, Y, Z;
    //カメラ基準の方向ベクトル
    Vector3 screenOrigin;
    //スクリーンの原点
public:
    Vector3 eye;
    //視点
    Vector3 lookAt;
    //方向
    double angle;
    //アングル
    //初期化
    void Set(Vector3 _eye, Vector3 _lookAt, double _angle)
    {
        eye = _eye;
        lookAt = _lookAt;
        angle = _angle;
        SetScreenOrigin();
    }
    void SetScreenOrigin()
    {
        //スクリーンまでの奥行きを求める
        double depth = (HEIGHT / 2) / tan(angle * (PI / 180));
        //スクリーン中心の座標を求める
        Vector3 screenCenter = eye + lookAt.Norm() * depth;
        //図のX,Y,Zベクトルを求める
        Z = -lookAt.Norm();
        X = -Vector3(0, 1, 0).Cross(Z).Norm(); //上向きベクトルとZの外積を求めて正規化
        Y = Z.Cross(X).Norm();                 //ZとXの外積を求めて正規化
        //スクリーン上での原点の座標を求める
        screenOrigin = screenCenter - Y * (HEIGHT / 2) - X * (WIDTH / 2);
    }
    //uv座標の(i,j)の場所の光線を取得する
    Ray GetScreenRay(int i, int j)
    {
        //-0.5~0.5の乱数を取得する
        float randA = drand48() - 0.5;
        float randB = drand48() - 0.5;
        //光線の場所を求める
        //Vector3 rayPos= screenOrigin+ X * i+ Y * j;
        Vector3 rayPos = screenOrigin + X * (i + randA) + Y * (j + randB);
        //光線をリターンする
        return Ray(eye, (rayPos - eye).Norm());
    }
};
//球体
class Sphere
{
public:
    //変数
    Vector3 center;
    //中心座標
    float radius; //半径
    //初期化
    Sphere() {} //クラス内での宣言で必要
    //セット
    void Set(Vector3 _center, float _radius)
    {
        center = _center;
        radius = _radius;
    }

    //交差判定
    bool IsHit(Ray ray)
    {
        Vector3 oc = ray.origin - center;
        //二次方程式の定数a,b,cを求める
        float a = (ray.direction).Dot(ray.direction);
        float b = oc.Dot(ray.direction);
        float c = oc.Dot(oc) - radius * radius;
        //判別式Dを求める
        float discriminant = b * b - a * c;
        //交点が存在する場合
        if (discriminant > 0)
        {
            float solveT1;
            solveT1 = (-b - sqrt(discriminant)) / a;
            //解1(手前側)//交点が前方に存在する場合
            if (solveT1 > 0.01)
            {
                //反射情報を保存する
                hitRecord.solveT = solveT1;
                return true;
            }
        }
        return false;
    }
};
class World
{
public:
    //カメラ
    Camera camera;
    //球体オブジェクト
    Sphere ball_1, ball_2, ball_3, terrain;
    Sphere objects[4];
    //初期化
    World()
    {
        //カメラを設定
        camera.Set(Vector3(0, 0, 0), Vector3(0, 0, 1), 45); //座標、視線方向、仰角
        //オブジェクトを設定
        ball_1.Set(Vector3(-2, 0, 3), 1);
        ball_2.Set(Vector3(0, 0, 3), 1);
        ball_3.Set(Vector3(2, 0, 3), 1);
        terrain.Set(Vector3(0, -1001, 0), 1000);
        //オブジェクトを登録
        objects[0] = ball_1;
        objects[1] = ball_2;
        objects[2] = ball_3;
        objects[3] = terrain;
    }
    //光線を飛ばして色を取得する
    Color GetColor(Ray inputRay)
    {
        //交差か否か？
        bool isHit = false;
        float closeT = 10000; //最短距離の解
        int index;            //最短の物体のインデックス
        //すべてのオブジェクトについて衝突を検索
        for (int n = 0; n < 4; n++)
        {
            //交点が存在する場合
            if (objects[n].IsHit(inputRay))
            {
                //交差にチェック
                isHit = true;
                //最短の物体であれば
                if (hitRecord.solveT < closeT)
                {
                    closeT = hitRecord.solveT;
                    //解を更新
                    index = n;
                    //最短の物体のインデックスを保存
                }
            }
        }
        //交差がある場合はその色を取得
        if (isHit)
        {
            // 最短の球を取得
            Sphere closeSphere = objects[index];
            //反射情報を保存
            hitRecord.solveT = closeT;
            hitRecord.position = inputRay.origin + inputRay.direction * closeT;
            hitRecord.normal = (hitRecord.position - closeSphere.center).Norm();
        }

        // 出力のタイプで分ける
        int imageType = 1;
        //法線ベクトルを出力するとき
        if (imageType == 0)
        {
            if (isHit)
            {
                return Color().VectorToColor(hitRecord.normal);
            }
            return Color().VectorToColor(inputRay.direction);
            //光線の方向の色
        }
        //深度を出力するとき
        else if (imageType == 1)
        {
            if (isHit)
            {
                int depth = (int)((pow((3.0 / 5.0), hitRecord.solveT)) * 255);
                return Color(depth, depth, depth);
                //ぶつかると真っ白
            }
            return Color(0, 0, 0);
            //黒色
        }
        //輪郭を出力するとき
        else if (imageType == 2)
        {
            if (isHit)
            {
                //入射ベクトルを整える
                Vector3 I = -inputRay.direction.Norm();
                if (I.Dot(hitRecord.normal) < (0.2 / sqrt(sqrt(objects[index].radius))))
                {
                    return Color(255, 255, 255);
                }
            }
            return Color(0, 0, 0); //黒色
        }

        return Color(150, 200, 255); //空の色
        //return Color(150, 200, 255);
        return Color().VectorToColor(inputRay.direction); //光線の角度
    }

    //画像生成
    void GetImage(char *path)
    {
        // カラー画像の書き込み
        FILE *fp;
        fp = fopen(path, "wb");                //■write binaryモードで画像ファイルを開く
        fprintf(fp, "P3¥n");                   //■ファイルの識別符号を書き込む
        fprintf(fp, "%d %d¥n", WIDTH, HEIGHT); //■画像サイズを書き込む
        fprintf(fp, "%d¥n", 255);              //■最大輝度値を書き込む

        //■画像データの書き込み
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                //カラーを格納する変数を作成
                int red = 0, green = 0, blue = 0;
                for (int n = 0; n < SAMPLING_SIZE; n++)
                {
                    //カメラからの光線を取得する
                    Ray cameraRay = camera.GetScreenRay(x, y);
                    //光線を飛ばして色を取得し加算
                    Color c = GetColor(cameraRay);
                    //取得した色を書き込む
                    red += c.r;
                    green += c.g;
                    blue += c.b;
                }
                //サンプリングの平均を取得
                red = (int)(red / SAMPLING_SIZE);
                green = (int)(green / SAMPLING_SIZE);
                blue = (int)(blue / SAMPLING_SIZE);
                //ファイルに書き込み
                fprintf(fp, "%d %d %d ", red, green, blue);
            }
        }
        fclose(fp);
    }
};

int main()
{
    World world;
    char imagePath[] = "./test6.ppm";
    world.GetImage(imagePath);
    return 0;
}
