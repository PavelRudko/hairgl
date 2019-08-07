#ifndef HAIRGL_MATH_H
#define HAIRGL_MATH_H

namespace HairGL
{
    constexpr float PI = 3.1415926535f;
    constexpr float RadToDeg = 180.0f / PI;
    constexpr float DegToRad = PI / 180.0f;

    struct Vector3;

    struct Vector4
    {
        union 
        {
            struct
            {
                float m[4];
            };
            struct
            {
                float x, y, z, w;
            };
        };

        Vector4(float x, float y, float z, float w = 1.0f) :
            x(x), 
            y(y),
            z(z),
            w(w)
        {
        }

        Vector4() :
            x(0.0f),
            y(0.0f),
            z(0.0f),
            w(1.0f)
        {
        }

        float operator[](unsigned int i) const 
        {
            return m[i];
        }

        float& operator[](unsigned int i)
        {
            return m[i];
        }

        Vector4 operator+(const Vector4& other) const;
        Vector4 operator-(const Vector4& other) const;
        Vector4 operator*(float value) const;
        Vector4 operator/(float value) const;

        Vector4& operator+=(const Vector4& other);
        Vector4& operator-=(const Vector4& other);
        Vector4& operator*=(float value);
        Vector4& operator/=(float value);

        float Length() const;
        float Length2() const;
        Vector4 Normalized() const;
        Vector4& Normalize();
        Vector3 XYZ() const;
    };

    struct Vector3
    {
        union
        {
            struct
            {
                float m[3];
            };
            struct
            {
                float x, y, z;
            };
        };

        Vector3(float x, float y, float z) :
            x(x),
            y(y),
            z(z)
        {
        }

        Vector3() :
            x(0.0f),
            y(0.0f),
            z(0.0f)
        {
        }

        float operator[](unsigned int i) const
        {
            return m[i];
        }

        float& operator[](unsigned int i)
        {
            return m[i];
        }

        Vector3 operator+(const Vector3& other) const;
        Vector3 operator-(const Vector3& other) const;
        Vector3 operator*(float value) const;
        Vector3 operator/(float value) const;

        Vector3& operator+=(const Vector3& other);
        Vector3& operator-=(const Vector3& other);
        Vector3& operator*=(float value);
        Vector3& operator/=(float value);

        float Length() const;
        float Length2() const;
        Vector3 Normalized() const;
        Vector3& Normalize();

        static Vector3 Cross(const Vector3& a, const Vector3& b);
        static float Dot(const Vector3& a, const Vector3& b);
    };

    struct Matrix4
    {
        Vector4 m[4];

        Matrix4()
        {
            SetZero();
        }

        void SetIdentity();
        void SetZero();

        static Matrix4 Perspective(float fovy, float aspect, float zNear, float zFar);
        static Matrix4 LookAt(const HairGL::Vector3& eye, const HairGL::Vector3& at, const HairGL::Vector3& up);
        static Matrix4 Translation(float x, float y, float z);
        static Matrix4 RotateX(float angle);
        static Matrix4 RotateY(float angle);
        static Matrix4 RotateZ(float angle);

        Matrix4 EuclidianInversed() const;
        
        Matrix4 operator*(const Matrix4& other) const;
        Vector4 operator*(const Vector4& v) const;
    };
}

#endif