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
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
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

	void Matrix3::SetIdentity()
	{
		SetZero();
		for (int i = 0; i < 3; i++) {
			m[i][i] = 1.0f;
		}
	}

	void Matrix3::SetZero()
	{
		memset(m, 0, sizeof(float) * 9);
	}

	Matrix3 Matrix3::operator*(const Matrix3& other) const
	{
		Matrix3 r;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				float sum = 0.0f;
				for (int k = 0; k < 3; k++) {
					sum += m[k][i] * other.m[j][k];
				}
				r.m[j][i] = sum;
			}
		}
		return r;
	}

	Quaternion::Quaternion(const Vector3& axis, float angle)
	{
		float halfAngle = angle * 0.5f;
		w = cos(halfAngle);

		auto xyz = axis * sin(halfAngle);
		x = xyz.x;
		y = xyz.y;
		z = xyz.z;
	}

	Quaternion Quaternion::Inversed() const
	{
		float lengthSqr = x * x + y * y + z * z + w * w;
		if (lengthSqr < 0.001) {
			return Quaternion(0, 0, 0, 1.0f);
		}

		Quaternion result;

		result.x = -x / lengthSqr;
		result.y = -y / lengthSqr;
		result.z = -z / lengthSqr;
		result.w = w / lengthSqr;

		return result;
	}

	Quaternion Quaternion::FromMatrix(const Matrix3& matrix)
	{
		Quaternion result;

		float trace = matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2];

		if (trace > 0) {
			result.w = 0.5f * sqrtf(trace + 1.0f);
			float d = 1.0f / (4.0f * result.w);
			result.x = (matrix.m[2][1] - matrix.m[1][2]) * d;
			result.y = (matrix.m[0][2] - matrix.m[2][0]) * d;
			result.z = (matrix.m[1][0] - matrix.m[0][1]) * d;
		}
		else {
			static size_t nextIndex[3] = {1, 2, 0};
			size_t i = 0;
			if (matrix.m[1][1] > matrix.m[i][i]) {
				i = 1;
			}
			if (matrix.m[2][2] > matrix.m[i][i]) {
				i = 2;
			}

			size_t j = nextIndex[i];
			size_t k = nextIndex[j];
			float root = sqrtf(matrix.m[i][i] - matrix.m[j][j] - matrix.m[k][k] + 1.0f);
			result[i] = 0.5f * root;
			root = 0.5f / root;
			result.w = (matrix.m[k][j] - matrix.m[j][k]) * root;
			result[j] = (matrix.m[j][i] + matrix.m[i][j]) * root;
			result[k] = (matrix.m[k][i] + matrix.m[i][k]) * root;
		}
		return result;
	}

	Vector3 Quaternion::operator*(const Vector3& v) const
	{
		auto qvec = Vector3(x, y, z);
		auto uv = Vector3::Cross(qvec, v);
		auto uuv = Vector3::Cross(qvec, uv);
		uv *= (2.0f * w);
		uuv *= 2.0f;

		return v + uv + uuv;
	}

	Quaternion Quaternion::operator*(const Quaternion& other) const
	{
		Quaternion q;

		q.w = w * other.w - x * other.x - y * other.y - z * other.z;
		q.x = w * other.x + x * other.w + y * other.z - z * other.y;
		q.y = w * other.y + y * other.w + z * other.x - x * other.z;
		q.z = w * other.z + z * other.w + x * other.y - y * other.x;

		return q;
	}
}