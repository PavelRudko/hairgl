#include <hairgl/HairGL.h>
#include <math.h>
#include <memory>

namespace HairGL
{
    Vector4 Vector4::operator+(const Vector4& other) const
    {
        return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    Vector4 Vector4::operator-(const Vector4& other) const
    {
        return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    Vector4 Vector4::operator*(float value) const
    {
        return Vector4(x * value, y * value, z * value, w * value);
    }

    Vector4 Vector4::operator/(float value) const
    {
        return Vector4(x / value, y / value, z / value, w / value);
    }

    Vector4& Vector4::operator+=(const Vector4& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    Vector4& Vector4::operator-=(const Vector4& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    Vector4& Vector4::operator*=(float value)
    {
        x *= value;
        y *= value;
        z *= value;
        w *= value;
        return *this;
    }

    Vector4& Vector4::operator/=(float value)
    {
        x /= value;
        y /= value;
        z /= value;
        w /= value;
        return *this;
    }

    float Vector4::Length() const
    {
        return sqrtf(Length2());
    }

    float Vector4::Length2() const
    {
        return x * x + y * y + z * z + w * w;
    }

    Vector4 Vector4::Normalized() const
    {
        float l = Length();
        return Vector4(x / l, y / l, z / l, w / l);
    }

    Vector4& Vector4::Normalize()
    {
        float l = Length();
        x /= l;
        y /= l;
        z /= l;
        w /= l;
        return *this;
    }

    Vector3 Vector4::XYZ() const
    {
        return Vector3(x, y, z);
    }

    Vector3 Vector3::operator+(const Vector3& other) const
    {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 Vector3::operator-(const Vector3& other) const
    {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 Vector3::operator*(float value) const
    {
        return Vector3(x * value, y * value, z * value);
    }

    Vector3 Vector3::operator/(float value) const
    {
        return Vector3(x / value, y / value, z / value);
    }

    Vector3& Vector3::operator+=(const Vector3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vector3& Vector3::operator-=(const Vector3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vector3& Vector3::operator*=(float value)
    {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }

    Vector3& Vector3::operator/=(float value)
    {
        x /= value;
        y /= value;
        z /= value;
        return *this;
    }

    float Vector3::Length() const
    {
        return sqrtf(Length2());
    }

    float Vector3::Length2() const
    {
        return x * x + y * y + z * z;
    }

    Vector3 Vector3::Normalized() const
    {
        float l = Length();
        return Vector3(x / l, y / l, z / l);
    }

    Vector3& Vector3::Normalize()
    {
        float l = Length();
        x /= l;
        y /= l;
        z /= l;
        return *this;
    }

    void Matrix4::SetIdentity()
    {
        SetZero();
        for (int i = 0; i < 4; i++) {
            m[i][i] = 1.0f;
        }
    }

    void Matrix4::SetZero()
    {
        memset(m, 0, sizeof(float) * 16);
    }

    Matrix4 Matrix4::Perspective(float fovy, float aspect, float zNear, float zFar)
    {
        float tf = tan(fovy / 2.0f);
        float nfDiff = zNear - zFar;
        Matrix4 p;
        p.m[0][0] = 1.0f / (aspect * tf);
        p.m[1][1] = 1.0f / tf;
        p.m[2][2] = (zFar + zNear) / nfDiff;
        p.m[2][3] = -1.0f;
        p.m[3][2] = 2.0f * zFar * zNear / nfDiff;
        return p;
    }

    Matrix4 Matrix4::LookAt(const HairGL::Vector3& eye, const HairGL::Vector3& at, const HairGL::Vector3& up)
    {
        auto zAxis = (at - eye).Normalized();
        auto xAxis = Vector3::Cross(zAxis, up).Normalized();
        auto yAxis = Vector3::Cross(xAxis, zAxis).Normalized();

        Matrix4 l;
        l.SetIdentity();
        for (int i = 0; i < 3; i++) {
            l.m[i][0] = xAxis[i];
            l.m[i][1] = yAxis[i];
            l.m[i][2] = -zAxis[i];
        }

        l.m[3][0] = -Vector3::Dot(xAxis, eye);
        l.m[3][1] = -Vector3::Dot(yAxis, eye);
        l.m[3][2] = Vector3::Dot(zAxis, eye);

        return l;
    }

    Matrix4 Matrix4::Translation(float x, float y, float z)
    {
        Matrix4 t;
        t.SetIdentity();
        t.m[3][0] = x;
        t.m[3][1] = y;
        t.m[3][2] = z;
        return t;
    }

    Matrix4 Matrix4::RotateX(float angle)
    {
        float c = cos(angle);
        float s = sin(angle);

        Matrix4 ry;
        ry.SetIdentity();
        ry.m[1][1] = c;
        ry.m[2][1] = -s;
        ry.m[1][2] = s;
        ry.m[2][2] = c;
        return ry;
    }

    Matrix4 Matrix4::RotateY(float angle)
    {
        float c = cos(angle);
        float s = sin(angle);

        Matrix4 ry;
        ry.SetIdentity();
        ry.m[0][0] = c;
        ry.m[0][2] = -s;
        ry.m[2][0] = s;
        ry.m[2][2] = c;
        return ry;
    }

    Matrix4 Matrix4::RotateZ(float angle)
    {
        float c = cos(angle);
        float s = sin(angle);

        Matrix4 ry;
        ry.SetIdentity();
        ry.m[0][0] = c;
        ry.m[1][0] = -s;
        ry.m[0][1] = s;
        ry.m[1][1] = c;
        return ry;
    }

    Matrix4 Matrix4::EuclidianInversed() const
    {
        Matrix4 rInv;
        rInv.SetIdentity();
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                rInv.m[i][j] = m[j][i];
            }
        }
        
        Matrix4 tInv;
        tInv.SetIdentity();
        for (int i = 0; i < 3; i++) {
            tInv.m[3][i] = -m[3][i];
        }

        return rInv * tInv;
    }

    Matrix4 Matrix4::operator*(const Matrix4& other) const
    {
        Matrix4 r;
        for (int i = 0; i < 4; i++) { //rows
            for (int j = 0; j < 4; j++) { //columns
                float sum = 0.0f;
                for (int k = 0; k < 4; k++) {
                    sum += m[k][i] * other.m[j][k];
                }
                r.m[j][i] = sum;
            }
        }
        return r;
    }

    Vector4 Matrix4::operator*(const Vector4& v) const
    {
        return Vector4(
            m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3],
            m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3],
            m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3],
            m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3] * v[3]
        );
    }

    Vector3 Vector3::Cross(const Vector3& a, const Vector3& b)
    {
        Vector3 c;
        c.x = a.y * b.z - a.z * b.y;
        c.y = a.z * b.x - a.x * b.z;
        c.z = a.x * b.y - a.y * b.x;
        return c;
    }

    float Vector3::Dot(const Vector3& a, const Vector3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
}