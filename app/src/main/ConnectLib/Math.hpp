//
// Created by cheerwizard on 01.12.24.
//

#ifndef CONNECT_MATH_HPP
#define CONNECT_MATH_HPP

constexpr float operator ""_PI(unsigned long long x) {
    return x * 3.14159265359f;
}

constexpr float operator ""_E(unsigned long long x) {
    return x * 2.71828f;
}

#define RADIANS(x) ((x) * 1_PI / 180.0f)

constexpr float operator ""_RAD(unsigned long long x) {
    return RADIANS(x);
}

inline float clamp(float a, float b, float x) {
    return std::min(std::max(x, b), a);
}

inline float lerp(float a, float b, float x) {
    return x * (b - a) + a;
}

inline float step(float a, float x) {
    return clamp(0, 1, x - a);
}

inline float smoothstep(float a, float b, float x) {
    float t = clamp(0, 1, (x - a) / (b - a));
    float y = -2 * t * t * t + 3 * t * t;
    return y;
}

struct Degree {
    float a;

    explicit Degree(float a = 0) : a(a) {}

    operator float() const { return a; }
};

struct Radians {
    float a;

    explicit Radians(float a = 0) : a(a) {}
    explicit Radians(const Degree& d) : a(RADIANS(d)) {}

    operator float() const { return a; }
};

template<typename T>
struct Vec2 {

    union {
        struct {
            T xy[2];
        };
        struct {
            T rg[2];
        };
        struct {
            T x, y;
        };
        struct {
            T r, g;
        };
    };

    Vec2() = default;

    Vec2(T x, T y) : x(x), y(y) {}

    Vec2(const Vec2<T>& v) = default;

    inline T& operator [](int i) {
        return *(&x + i);
    }

    inline const T& operator [](int i) const {
        return *(&x + i);
    }

    inline friend Vec2 operator +(const Vec2& v1, const Vec2& v2) {
        return { v1.x + v2.x, v1.y + v2.y };
    }

    inline friend Vec2 operator -(const Vec2& v1, const Vec2& v2) {
        return { v1.x - v2.x, v1.y - v2.y };
    }

    inline friend Vec2 operator *(const Vec2& v1, const Vec2& v2) {
        return { v1.x * v2.x, v1.y * v2.y };
    }

    inline friend Vec2 operator /(const Vec2& v1, const Vec2& v2) {
        return { v1.x / v2.x, v1.y / v2.y };
    }

    inline friend Vec2 operator +(const Vec2& v1, const T& s) {
        return { v1.x + s, v1.y + s };
    }

    inline friend Vec2 operator -(const Vec2& v1, const T& s) {
        return { v1.x - s, v1.y - s };
    }

    inline friend Vec2 operator *(const Vec2& v1, const T& s) {
        return { v1.x * s, v1.y * s };
    }

    inline friend Vec2 operator /(const Vec2& v1, const T& s) {
        return { v1.x / s, v1.y / s };
    }

    inline friend Vec2 operator +(const T& s, const Vec2& v2) {
        return { s + v2.x, s + v2.y };
    }

    inline friend Vec2 operator -(const T& s, const Vec2& v2) {
        return { s - v2.x, s - v2.y };
    }

    inline friend Vec2 operator *(const T& s, const Vec2& v2) {
        return { s * v2.x, s * v2.y };
    }

    inline friend Vec2 operator /(const T& s, const Vec2& v2) {
        return { s / v2.x, s / v2.y };
    }

    inline friend Vec2 operator ^(const Vec2& v, const T& p) {
        return { Pow(v.x, p), Pow(v.y, p) };
    }

    inline friend Vec2 operator -(const Vec2& v) {
        return { -v.x, -v.y };
    }
};

template<typename T>
inline T length(const Vec2<T>& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

template<typename T>
inline Vec2<T> normalize(const Vec2<T>& v) {
    T l = length(v);
    return { v.x / l, v.y / l };
}

template<typename T>
inline T dot(const Vec2<T>& v1, const Vec2<T>& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

template<typename T>
inline T cross(const Vec2<T>& v1, const Vec2<T>& v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

template<typename T>
struct Vec3 {

    union {
        struct {
            T xyz[3];
        };
        struct {
            T rgb[3];
        };
        struct {
            T x, y, z;
        };
        struct {
            T r, g, b;
        };
    };

    Vec3() = default;

    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

    Vec3(const Vec2<T>& v) : x(v.x), y(v.y) {}

    Vec3(const Vec3<T>& v) = default;

    inline T& operator [](int i) {
        return *(&x + i);
    }

    inline const T& operator [](int i) const {
        return *(&x + i);
    }

    inline friend Vec3 operator +(const Vec3& v1, const Vec3& v2) {
        return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
    }

    inline friend Vec3 operator -(const Vec3& v1, const Vec3& v2) {
        return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
    }

    inline friend Vec3 operator *(const Vec3& v1, const Vec3& v2) {
        return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
    }

    inline friend Vec3 operator /(const Vec3& v1, const Vec3& v2) {
        return { v1.x / v2.x, v1.y / v2.y, v1.z / v2.z };
    }

    inline friend Vec3 operator +(const Vec3& v1, const T& s) {
        return { v1.x + s, v1.y + s, v1.z + s };
    }

    inline friend Vec3 operator -(const Vec3& v1, const T& s) {
        return { v1.x - s, v1.y - s, v1.z - s };
    }

    inline friend Vec3 operator *(const Vec3& v1, const T& s) {
        return { v1.x * s, v1.y * s, v1.z * s };
    }

    inline friend Vec3 operator /(const Vec3& v1, const T& s) {
        return { v1.x / s, v1.y / s, v1.z / s };
    }

    inline friend Vec3 operator +(const T& s, const Vec3& v2) {
        return { s + v2.x, s + v2.y, s + v2.z };
    }

    inline friend Vec3 operator -(const T& s, const Vec3& v2) {
        return { s - v2.x, s - v2.y, s - v2.z };
    }

    inline friend Vec3 operator *(const T& s, const Vec3& v2) {
        return { s * v2.x, s * v2.y, s * v2.z };
    }

    inline friend Vec3 operator /(const T& s, const Vec3& v2) {
        return { s / v2.x, s / v2.y, s / v2.z };
    }

    inline friend Vec3 operator ^(const Vec3& v, const T& p) {
        return { Pow(v.x, p), Pow(v.y, p), Pow(v.z, p) };
    }

    inline friend Vec3 operator -(const Vec3& v) {
        return { -v.x, -v.y, -v.z };
    }

    inline Vec2<T> xy() const {
        return { x, y };
    }
};

template<typename T>
inline T length(const Vec3<T>& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

template<typename T>
inline Vec3<T> normalize(const Vec3<T>& v) {
    T l = length(v);
    return { v.x / l, v.y / l, v.z / l };
}

template<typename T>
inline T dot(const Vec3<T>& v1, const Vec3<T>& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template<typename T>
inline Vec3<T> cross(const Vec3<T>& v1, const Vec3<T>& v2) {
    return {
            v1.y * v2.z - v1.z * v2.y,
            v1.z * v2.x - v1.x * v2.z,
            v1.x * v2.y - v1.y * v2.x,
    };
}

template<typename T>
struct Vec4 {
    union {
        struct {
            T xyzw[4];
        };
        struct {
            T rgba[4];
        };
        struct {
            T x, y, z, w;
        };
        struct {
            T r, g, b, a;
        };
    };

    Vec4() = default;

    Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

    Vec4(const Vec2<T>& v) : x(v.x), y(v.y) {}

    Vec4(const Vec3<T>& v) : x(v.x), y(v.y), z(v.z) {}

    Vec4(const Vec4<T>& v) = default;

    inline T& operator [](int i) {
        return *(&x + i);
    }

    inline const T& operator [](int i) const {
        return *(&x + i);
    }

    inline friend Vec4 operator +(const Vec4& v1, const Vec4& v2) {
        return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w };
    }

    inline friend Vec4 operator -(const Vec4 v1, const Vec4& v2) {
        return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w };
    }

    inline friend Vec4 operator *(const Vec4& v1, const Vec4& v2) {
        return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w };
    }

    inline friend Vec4 operator /(const Vec4& v1, const Vec4& v2) {
        return { v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w };
    }

    inline friend Vec4 operator +(const Vec4& v1, const T& s) {
        return { v1.x + s, v1.y + s, v1.z + s, v1.w + s };
    }

    inline friend Vec4 operator -(const Vec4& v1, const T& s) {
        return { v1.x - s, v1.y - s, v1.z - s, v1.w - s };
    }

    inline friend Vec4 operator *(const Vec4& v1, const T& s) {
        return { v1.x * s, v1.y * s, v1.z * s, v1.w * s };
    }

    inline friend Vec4 operator /(const Vec4& v1, const T& s) {
        return { v1.x / s, v1.y / s, v1.z / s, v1.w / s };
    }

    inline friend Vec4 operator +(const T& s, const Vec4& v2) {
        return { s + v2.x, s + v2.y, s + v2.z, s + v2.w };
    }

    inline friend Vec4 operator -(const T& s, const Vec4& v2) {
        return { s - v2.x, s - v2.y, s - v2.z, s - v2.w };
    }

    inline friend Vec4 operator *(const T& s, const Vec4& v2) {
        return { s * v2.x, s * v2.y, s * v2.z, s * v2.w };
    }

    inline friend Vec4 operator /(const T& s, const Vec4& v2) {
        return { s / v2.x, s / v2.y, s / v2.z, s / v2.w };
    }

    inline friend Vec4 operator ^(const Vec4& v, const T& p) {
        return { Pow(v.x, p), Pow(v.y, p), Pow(v.z, p), Pow(v.w, p) };
    }

    inline friend Vec4 operator -(const Vec4& v) {
        return { -v.x, -v.y, -v.z, -v.w };
    }

    inline Vec2<T> xy() const {
        return { x, y };
    }

    inline Vec3<T> xyz() const {
        return { x, y, z };
    }
};

template<typename T>
inline T length(const Vec4<T>& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

template<typename T>
inline Vec4<T> normalize(const Vec4<T>& v) {
    T l = length(v);
    return { v.x / l, v.y / l, v.z / l, v.w / l };
}

template<typename T>
inline T dot(const Vec4<T>& v1, const Vec4<T>& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

// TODO(cheerwizard): do I really need a wedged/geometry products?
template<typename T>
inline Vec4<T> cross(const Vec4<T>& v1, const Vec4<T>& v2) {
    // TODO(cheerwizard): not implemented!
    return {};
}

struct Quat final {
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;

    Quat() = default;

    Quat(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    Quat(float nx, float ny, float nz, const Radians& r = Radians(0)) {
        x = nx * std::sin(r * 0.5f);
        y = ny * std::sin(r * 0.5f);
        z = nz * std::sin(r * 0.5f);
        w = std::cos(r * 0.5f);
    }

    Quat(const Vec3<float>& n, const Radians& r = Radians(0)) : Quat(n.x, n.y, n.z , r) {}

    inline friend Quat operator *(const Quat& q1, const Quat& q2) {
        Quat q3;
        q3.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
        q3.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
        q3.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
        q3.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
        return q3;
    }

    inline friend Quat operator-(const Quat& q) {
        return { -q.x, -q.y, -q.z, q.w };
    }

    inline Vec2<float> xy() const {
        return { x, y };
    }

    inline Vec3<float> xyz() const {
        return { x, y, z };
    }
};

inline float length(const Quat& q) {
    return std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z);
}

inline Quat normalize(const Quat& q) {
    float l = length(q);
    return { q.x / l, q.y / l, q.z / l, q.w };
}

inline Quat rotate(const Quat& q, const Vec3<float>& n) {
    return q * Quat(n) * -q;
}

inline Quat slerp(const Quat& q1, const Quat& q2, float t) {
    Quat q3;
    float dot = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
    float theta, st, sut, sout, coeff1, coeff2;

    t /= 2.0;

    theta = std::acos(dot);
    if (theta < 0.0) {
        theta=-theta;
    }

    st = std::sin(theta);
    sut = std::sin(t*theta);
    sout = std::sin((1-t)*theta);
    coeff1 = sout/st;
    coeff2 = sut/st;

    q3.x = coeff1*q1.x + coeff2*q2.x;
    q3.y = coeff1*q1.y + coeff2*q2.y;
    q3.z = coeff1*q1.z + coeff2*q2.z;
    q3.w = coeff1*q1.w + coeff2*q2.w;

    q3 = normalize(q3);

    return q3;
}

template<typename T>
struct Mat2 {
    Vec2<T> m[2];

    Mat2() = default;

    Mat2(
            T m00, T m01,
            T m10, T m11
    ) {
        m[0][0] = m00;
        m[0][1] = m01;

        m[1][0] = m10;
        m[1][1] = m11;
    }

    Mat2(const Vec2<T>& v0, const Vec2<T>& v1) {
        m[0] = v0;
        m[1] = v1;
    }

    inline Vec2<T>& operator [](int i) {
        return m[i];
    }

    inline const Vec2<T>& operator [](int i) const {
        return m[i];
    }

    inline friend Mat2 operator *(const Mat2& m1, const Mat2& m2) {
        Mat2 m3;
        for (int r = 0 ; r < 2 ; r++) {
            for (int c = 0 ; c < 2 ; c++) {
                for (int i = 0 ; i < 2 ; i++) {
                    m3[r][c] += m1[r][i] * m2[i][c];
                }
            }
        }
        return m3;
    }

    inline Mat2 operator -() const {
        Mat2 n;
        for (int r = 0 ; r < 2 ; r++) {
            for (int c = 0 ; c < 2 ; c++) {
                n[r][c] = -m[r][c];
            }
        }
        return n;
    }
};

template<typename T>
inline Mat2<T> transpose(const Mat2<T>& m) {
    Mat2 t = m;

    // Swap(t[0][0], t[0][0]);
    std::swap(t[0][1], t[1][0]);

    // Swap(t[1][0], t[0][1]);
    // Swap(t[1][1], t[1][1]);

    return t;
}

template<typename T>
inline T det(const Mat2<T>& m) {
    return m[0][0] * m[1][1] - m[1][0] * m[0][1];
}

template<typename T>
Mat2<T> inverse(const Mat2<T>& m) {
    Mat2<T> c;
    T d = det(m);
    c[0][0] = m[1][1];
    c[0][1] =-m[1][0];
    c[1][0] =-m[0][1];
    c[1][1] = m[0][0];
    c /= d;
    return c;
}

template<typename T>
struct Mat3 {
    Vec3<T> m[3];

    Mat3() = default;

    Mat3(
            T m00, T m01, T m02,
            T m10, T m11, T m12,
            T m20, T m21, T m22
    ) {
        m[0][0] = m00;
        m[0][1] = m01;
        m[0][2] = m02;

        m[1][0] = m10;
        m[1][1] = m11;
        m[1][2] = m12;

        m[2][0] = m20;
        m[2][1] = m21;
        m[2][2] = m22;
    }

    Mat3(const Vec3<T>& v0, const Vec3<T>& v1, const Vec3<T>& v2) {
        m[0] = v0;
        m[1] = v1;
        m[2] = v2;
    }

    inline Vec3<T>& operator [](int i) {
        return m[i];
    }

    inline const Vec3<T>& operator [](int i) const {
        return m[i];
    }

    inline friend Mat3 operator *(const Mat3& m1, const Mat3& m2) {
        Mat3 m3;
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                for (int i = 0; i < 3; i++) {
                    m3[r][c] += m1[r][i] * m2[i][c];
                }
            }
        }
        return m3;
    }

    inline Mat3 operator -() const {
        Mat3 n;
        for (int r = 0 ; r < 3 ; r++) {
            for (int c = 0 ; c < 3 ; c++) {
                n[r][c] = -m[r][c];
            }
        }
        return n;
    }
};

template<typename T>
inline Mat3<T> transpose(const Mat3<T>& m) {
    Mat3 t = m;

    // Swap(t[0][0], t[0][0]);
    std::swap(t[0][1], t[1][0]);
    std::swap(t[0][2], t[2][0]);

    // Swap(t[1][0], t[0][1]);
    // Swap(t[1][1], t[1][1]);
    std::swap(t[1][2], t[2][1]);

    // Swap(t[2][0], t[0][2]);
    // Swap(t[2][1], t[1][2]);
    // Swap(t[2][2], t[2][2]);

    return t;
}

template<typename T>
inline T det(const Mat3<T>& m) {
    T d = m[0][0] * m[1][1] * m[2][2]
          + m[0][1] * m[1][2] * m[2][0]
          + m[0][2] * m[1][0] * m[2][1]
          - m[2][0] * m[1][1] * m[0][2]
          - m[2][1] * m[1][2] * m[0][0]
          - m[2][2] * m[1][0] * m[0][1];
    return d;
}

template<typename T>
Mat3<T> inverse(const Mat3<T>& m) {
    T d = det(m);

    Mat3<T> c;

    c[0][0] = det(Mat2<T> {
            m[1][1], m[1][2],
            m[2][1], m[2][2]
    });

    c[0][1] = -det(Mat2<T> {
            m[1][0], m[1][2],
            m[2][0], m[2][2]
    });

    c[0][2] = det(Mat2<T> {
            m[1][0], m[1][1],
            m[2][0], m[2][1]
    });

    c[1][0] = -det(Mat2<T> {
            m[0][0], m[0][1],
            m[2][0], m[2][1]
    });

    c[1][1] = det(Mat2<T> {
            m[0][0], m[0][2],
            m[2][0], m[2][2]
    });

    c[1][2] = -det(Mat2<T> {
            m[0][0], m[0][1],
            m[2][0], m[2][1]
    });

    c[2][0] = det(Mat2<T> {
            m[0][1], m[0][2],
            m[1][1], m[1][2]
    });

    c[2][1] = -det(Mat2<T> {
            m[0][0], m[0][2],
            m[1][0], m[1][2]
    });

    c[2][2] = det(Mat2<T> {
            m[0][0], m[0][1],
            m[1][0], m[1][1]
    });

    c /= d;

    return c;
}

template<typename T>
struct Mat4 {
    Vec4<T> m[4];

    Mat4() = default;

    Mat4(
            T m00, T m01, T m02, T m03,
            T m10, T m11, T m12, T m13,
            T m20, T m21, T m22, T m23,
            T m30, T m31, T m32, T m33
    ) {
        m[0][0] = m00;
        m[0][1] = m01;
        m[0][2] = m02;
        m[0][3] = m03;

        m[1][0] = m10;
        m[1][1] = m11;
        m[1][2] = m12;
        m[1][3] = m13;

        m[2][0] = m20;
        m[2][1] = m21;
        m[2][2] = m22;
        m[2][3] = m23;

        m[3][0] = m30;
        m[3][1] = m31;
        m[3][2] = m32;
        m[3][3] = m33;
    }

    Mat4(const Vec4<T> &v0, const Vec4<T> &v1, const Vec4<T> &v2, const Vec4<T> &v3) {
        m[0] = v0;
        m[1] = v1;
        m[2] = v2;
        m[3] = v3;
    }

    Mat4(const Vec3<T> &v0, const Vec3<T> &v1, const Vec3<T> &v2, const Vec3<T> &v3) {
        m[0] = v0;
        m[1] = v1;
        m[2] = v2;
        m[3] = v3;
    }

    Mat4(const Quat &q) {
        T xx = q.x * q.x;
        T xy = q.x * q.y;
        T xz = q.x * q.z;
        T yy = q.y * q.y;
        T zz = q.z * q.z;
        T yz = q.y * q.z;
        T wx = q.w * q.x;
        T wy = q.w * q.y;
        T wz = q.w * q.z;

        m[0][0] = 1.0f - 2.0f * (yy + zz);
        m[1][0] = 2.0f * (xy - wz);
        m[2][0] = 2.0f * (xz + wy);
        m[3][0] = 0.0;

        m[0][1] = 2.0f * (xy + wz);
        m[1][1] = 1.0f - 2.0f * (xx + zz);
        m[2][1] = 2.0f * (yz - wx);
        m[3][1] = 0.0;

        m[0][2] = 2.0f * (xz - wy);
        m[1][2] = 2.0f * (yz + wx);
        m[2][2] = 1.0f - 2.0f * (xx + yy);
        m[3][2] = 0.0;

        m[0][3] = 0;
        m[1][3] = 0;
        m[2][3] = 0;
        m[3][3] = 1;
    }

    inline Vec4<T>& operator[](int i) {
        return m[i];
    }

    inline const Vec4<T>& operator[](int i) const {
        return m[i];
    }

    inline friend Mat4 operator*(const Mat4 &m1, const Mat4 &m2) {
        Mat4 m3;
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                for (int i = 0; i < 4; i++) {
                    m3[r][c] += m1[r][i] * m2[i][c];
                }
            }
        }
        return m3;
    }

    inline Mat4 operator-() const {
        Mat4 n;
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                n[r][c] = -m[r][c];
            }
        }
        return n;
    }

    inline friend Mat4 operator/(const Mat4& m1, T v) {
        Mat4 m2 = m1;
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                m2[r][c] /= v;
            }
        }
        return m2;
    }

    inline friend Mat4 operator/=(const Mat4& m1, T v) {
        Mat4 m2 = m1;
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                m2[r][c] /= v;
            }
        }
        return m2;
    }
};

template<typename T>
inline Mat4<T> transpose(const Mat4<T>& m) {
    Mat4 t = m;

    // Swap(t[0][0], t[0][0]);
    std::swap(t[0][1], t[1][0]);
    std::swap(t[0][2], t[2][0]);
    std::swap(t[0][3], t[3][0]);

    // Swap(t[1][0], t[0][1]);
    // Swap(t[1][1], t[1][1]);
    std::swap(t[1][2], t[2][1]);
    std::swap(t[1][3], t[3][1]);

    // Swap(t[2][0], t[0][2]);
    // Swap(t[2][1], t[1][2]);
    // Swap(t[2][2], t[2][2]);
    std::swap(t[2][3], t[3][2]);

    // Swap(t[3][0], t[0][3]);
    // Swap(t[3][1], t[1][3]);
    // Swap(t[3][2], t[2][3]);
    // Swap(t[3][3], t[3][3]);

    return t;
}

template<typename T>
inline T det(const Mat4<T>& m) {
    T d = m[0][0] * m[1][1] * m[2][2] * m[3][3]
          + m[0][1] * m[1][2] * m[2][3] * m[3][0]
          + m[0][2] * m[1][3] * m[2][0] * m[3][1]
          - m[3][0] * m[2][1] * m[1][2] * m[0][3]
          - m[3][1] * m[2][2] * m[1][3] * m[0][0]
          - m[3][2] * m[2][3] * m[1][0] * m[0][1];
    return d;
}

template<typename T>
Mat4<T> inverse(const Mat4<T>& m) {
    T d = det(m);

    Mat4<T> c;

    c[0][0] = det(Mat3<T>{
            m[1][1], m[1][2], m[1][3],
            m[2][1], m[2][2], m[2][3],
            m[3][1], m[3][2], m[3][3]
    });

    c[0][1] = -det(Mat3<T>{
            m[1][0], m[1][2], m[1][3],
            m[2][0], m[2][2], m[2][3],
            m[3][0], m[3][2], m[3][3]
    });

    c[0][2] = det(Mat3<T>{
            m[1][0], m[1][1], m[1][3],
            m[2][0], m[2][1], m[2][3],
            m[3][0], m[3][1], m[3][3]
    });

    c[0][3] = -det(Mat3<T>{
            m[1][0], m[1][1], m[1][2],
            m[2][0], m[2][1], m[2][2],
            m[3][0], m[3][1], m[3][2]
    });

    c[1][0] = -det(Mat3<T>{
            m[0][1], m[0][2], m[0][3],
            m[2][1], m[2][2], m[2][3],
            m[3][1], m[3][2], m[3][3]
    });

    c[1][1] = det(Mat3<T>{
            m[0][0], m[0][2], m[0][3],
            m[2][0], m[2][2], m[2][3],
            m[3][0], m[3][2], m[3][3]
    });

    c[1][2] = -det(Mat3<T>{
            m[0][0], m[0][1], m[0][3],
            m[2][0], m[2][1], m[2][3],
            m[3][0], m[3][1], m[3][3]
    });

    c[1][3] = det(Mat3<T>{
            m[0][0], m[0][1], m[0][2],
            m[2][0], m[2][1], m[2][2],
            m[3][0], m[3][1], m[3][2]
    });

    c[2][0] = det(Mat3<T>{
            m[0][1], m[0][2], m[0][3],
            m[1][1], m[1][2], m[1][3],
            m[3][1], m[3][2], m[3][3]
    });

    c[2][1] = -det(Mat3<T>{
            m[0][0], m[0][2], m[0][3],
            m[1][0], m[1][2], m[1][3],
            m[3][0], m[3][2], m[3][3]
    });

    c[2][2] = det(Mat3<T>{
            m[0][0], m[0][1], m[0][3],
            m[1][0], m[1][1], m[1][3],
            m[3][0], m[3][1], m[3][3]
    });

    c[2][3] = -det(Mat3<T>{
            m[0][0], m[0][1], m[0][2],
            m[1][0], m[1][1], m[1][2],
            m[3][0], m[3][1], m[3][2]
    });

    c[3][0] = -det(Mat3<T>{
            m[0][1], m[0][2], m[0][3],
            m[1][1], m[1][2], m[1][3],
            m[2][1], m[2][2], m[2][3]
    });

    c[3][1] = det(Mat3<T>{
            m[0][0], m[0][2], m[0][3],
            m[1][0], m[1][2], m[1][3],
            m[2][0], m[2][2], m[2][3]
    });

    c[3][2] = -det(Mat3<T>{
            m[0][0], m[0][1], m[0][3],
            m[1][0], m[1][1], m[1][3],
            m[2][0], m[2][1], m[2][3]
    });

    c[3][3] = det(Mat3<T>{
            m[0][0], m[0][1], m[0][2],
            m[1][0], m[1][1], m[1][2],
            m[2][0], m[2][1], m[2][2]
    });

    c /= d;

    return c;
}

template<typename T>
Mat4<T> fastInverseMatrix(const Mat4<T>& m) {
    // TODO(cheerwizard): not implemented!
    return {};
}

inline void translate(Mat4<float>& m, const Vec3<float>& translation) {
    m[0][3] += translation.x;
    m[1][3] += translation.y;
    m[2][3] += translation.z;
}

inline void scale(Mat4<float>& m, const Vec3<float>& scalar) {
    m[0][0] *= scalar.x;
    m[1][1] *= scalar.y;
    m[2][2] *= scalar.z;
}

inline void rotate(Mat4<float>& m, const Vec3<Radians>& r, const Vec3<float>& axis) {
    Mat4<float> rx;
    {
        float sinx = std::sin(r.x);
        float cosx = std::cos(r.x);
        rx[1][1] = cosx;
        rx[1][2] = -sinx;
        rx[2][1] = sinx;
        rx[2][2] = cosx;
        rx[0][0] = axis.x;
    }

    Mat4<float> ry;
    {
        float siny = std::sin(r.y);
        float cosy = std::cos(r.y);
        ry[0][0] = cosy;
        ry[0][2] = siny;
        ry[2][0] = -siny;
        ry[2][2] = cosy;
        ry[1][1] = axis.y;
    }

    Mat4<float> rz;
    {
        float sinz = std::sin(r.z);
        float cosz = std::cos(r.z);
        rz[0][0] = cosz;
        rz[0][1] = -sinz;
        rz[1][0] = sinz;
        rz[1][1] = cosz;
        rz[2][2] = axis.z;
    }

    m = m * rz * ry * rx;
}

struct ModelMat : Mat4<float> {
    ModelMat(const Vec3<float>& translation, const Quat& rotation, const Vec3<float>& scalar)
    : Mat4<float>()
    {
        Mat4<float>& m = *this;
        translate(m, translation);
        m = m * Mat4<float>(rotation);
        scale(m, scalar);
    }
};

struct RigidBodyMat : Mat4<float> {
    RigidBodyMat(const Vec3<float>& translation, const Quat& rotation) : Mat4<float>() {
        Mat4<float>& m = *this;
        translate(m, translation);
        m = m * Mat4<float>(rotation);
    }
};

struct ViewMatrix : Mat4<float> {
    ViewMatrix(const Vec3<float>& position, const Vec3<float>& front, const Vec3<float>& up)
    : Mat4<float>() {
        Vec3<float> right = normalize(cross(front, up));
        *this = fastInverseMatrix(transpose(Mat4<float>(
                right,
                cross(right, front),
                -front,
                position
        )));
    }
};

struct OrthoMatrix : Mat4<float> {
    OrthoMatrix(float left, float right, float bottom, float top, float z_near, float z_far)
    : Mat4<float>(
            { 2.0f / (right - left), 0.0f, 0.0f, 0.0f },
            { 0.0f, 2.0f / (bottom - top), 0.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f / (z_near - z_far), 0.0f },
            { -(right + left) / (right - left), -(bottom + top) / (bottom - top), z_near / (z_near - z_far), 1.0f }
    ) {}
};

class Matrix {
public:

    static Mat4<float> mat4_perspective(float aspect, Degree fov, float z_near, float z_far) {
        float f = 1.0f / tan(Radians(0.5f * fov));
        return Mat4<float> {
                { f / aspect, 0.0f, 0.0f, 0.0f },
                { 0.0f, -f, 0.0f, 0.0f },
                { 0.0f, 0.0f, z_far / (z_near - z_far), -1.0f },
                { 0.0f, 0.0f, z_near * z_far / (z_near - z_far), 0.0f }
        };
    }

    static Mat4<float> mat4_normal(const Mat4<float>& model) {
        return transpose(inverse(model));
    }

};

#endif //CONNECT_MATH_HPP