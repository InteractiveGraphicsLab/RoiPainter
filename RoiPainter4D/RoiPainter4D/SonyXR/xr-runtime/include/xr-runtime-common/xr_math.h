/**
 * @file xr_math.h
 * @copyright
 *
 * Sony CONFIDENTIAL
 *
 * Copyright 2019,2020,2021 Sony Corporation
 *
 * DO NOT COPY AND/OR REDISTRIBUTE WITHOUT PERMISSION.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef XR_MATH_H_
#define XR_MATH_H_

#include <math.h>

/**
 * @struct SonyOzQuaternionf
 * @brief 要素がfloat型のクォータニオン
 */
struct SonyOzQuaternionf {
  /// x
  float x;

  /// y
  float y;

  /// z
  float z;

  /// w
  float w;

  /**
   * @brief コンストラクタ
   */
  SonyOzQuaternionf() : x(0.f), y(0.f), z(0.f), w(1.f) {}

  /**
   * @brief コンストラクタ
   * @param[in] in_x xの値を指定する
   * @param[in] in_y xの値を指定する
   * @param[in] in_z xの値を指定する
   * @param[in] in_w xの値を指定する
   */
  SonyOzQuaternionf(float in_x, float in_y, float in_z, float in_w)
      : x(in_x), y(in_y), z(in_z), w(in_w) {}

  void operator=(float a[4]) {
    this->x = a[0];
    this->y = a[1];
    this->z = a[2];
    this->w = a[3];
  }
};

/**
 * @struct SonyOzVector3f
 * @brief 要素がfloat型の3次元ベクトル
 */
struct SonyOzVector3f {
  /// x
  float x;

  /// y
  float y;

  /// z
  float z;

  /**
   * @brief コンストラクタ
   */
  SonyOzVector3f() : x(0.f), y(0.f), z(0.f) {}

  /**
   * @brief コンストラクタ
   * @param[in] in_x xの値を指定する
   * @param[in] in_y xの値を指定する
   * @param[in] in_z xの値を指定する
   */
  SonyOzVector3f(float in_x, float in_y, float in_z)
      : x(in_x), y(in_y), z(in_z) {}

  SonyOzVector3f operator+(SonyOzVector3f a) {
    return SonyOzVector3f(x + a.x, y + a.y, z + a.z);
  }
  SonyOzVector3f operator-(SonyOzVector3f a) {
    return SonyOzVector3f(x - a.x, y - a.y, z - a.z);
  }

  SonyOzVector3f operator*(float a) {
    return SonyOzVector3f(x * a, y * a, z * a);
  }
  SonyOzVector3f operator/(float a) {
    return SonyOzVector3f(x / a, y / a, z / a);
  }

  void operator=(float a[3]) {
    this->x = a[0];
    this->y = a[1];
    this->z = a[2];
  }

  float Dot(SonyOzVector3f a) { return x * a.x + y * a.y + z * a.z; }

  void Normalize() {
    float length = sqrtf(x * x + y * y + z * z);
    x /= length;
    y /= length;
    z /= length;
  }
};

/**
 * @struct SonyOzVector4f
 * @brief 要素がfloat型の4次元ベクトル
 */
struct SonyOzVector4f {
  /// x
  float x;

  /// y
  float y;

  /// z
  float z;

  /// w
  float w;

  /**
   * @brief コンストラクタ
   */
  SonyOzVector4f() : x(0.f), y(0.f), z(0.f), w(0.f) {}

  /**
   * @brief コンストラクタ
   * @param[in] in_x xの値を指定する
   * @param[in] in_y xの値を指定する
   * @param[in] in_z xの値を指定する
   * @param[in] in_w xの値を指定する
   */
  SonyOzVector4f(float in_x, float in_y, float in_z, float in_w)
      : x(in_x), y(in_y), z(in_z), w(in_w) {}

  float operator*(SonyOzVector4f a) {
    return x * a.x + y * a.y + z * a.z + w * a.w;
  }
};

/**
 * @struct SonyOzMatrix4x4f
 * @brief 要素がfloat型の4x4次元マトリクス
 */
struct SonyOzMatrix4x4f {
  /// 4x4次元マトリクス[行][列]
  float matrix[4][4];

  /**
   * @brief コンストラクタ
   */
  SonyOzMatrix4x4f() {
    matrix[0][0] = 0.f;
    matrix[0][1] = 0.f;
    matrix[0][2] = 0.f;
    matrix[0][3] = 0.f;
    matrix[1][0] = 0.f;
    matrix[1][1] = 0.f;
    matrix[1][2] = 0.f;
    matrix[1][3] = 0.f;
    matrix[2][0] = 0.f;
    matrix[2][1] = 0.f;
    matrix[2][2] = 0.f;
    matrix[2][3] = 0.f;
    matrix[3][0] = 0.f;
    matrix[3][1] = 0.f;
    matrix[3][2] = 0.f;
    matrix[3][3] = 0.f;
  }

  SonyOzMatrix4x4f(float x0, float x1, float x2, float x3, float y0, float y1,
                   float y2, float y3, float z0, float z1, float z2, float z3,
                   float w0, float w1, float w2, float w3) {
    matrix[0][0] = x0;
    matrix[0][1] = x1;
    matrix[0][2] = x2;
    matrix[0][3] = x3;
    matrix[1][0] = y0;
    matrix[1][1] = y1;
    matrix[1][2] = y2;
    matrix[1][3] = y3;
    matrix[2][0] = z0;
    matrix[2][1] = z1;
    matrix[2][2] = z2;
    matrix[2][3] = z3;
    matrix[3][0] = w0;
    matrix[3][1] = w1;
    matrix[3][2] = w2;
    matrix[3][3] = w3;
  }

  /**
   * @brief コンストラクタ
   * @param[in] in_x 1行目の横ベクトルを指定する
   * @param[in] in_y 2行目の横ベクトルを指定する
   * @param[in] in_z 3行目の横ベクトルを指定する
   * @param[in] in_w 4行目の横ベクトルを指定する
   */
  SonyOzMatrix4x4f(SonyOzVector4f in_x, SonyOzVector4f in_y,
                   SonyOzVector4f in_z, SonyOzVector4f in_w) {
    matrix[0][0] = in_x.x;
    matrix[0][1] = in_x.y;
    matrix[0][2] = in_x.z;
    matrix[0][3] = in_x.w;
    matrix[1][0] = in_y.x;
    matrix[1][1] = in_y.y;
    matrix[1][2] = in_y.z;
    matrix[1][3] = in_y.w;
    matrix[2][0] = in_z.x;
    matrix[2][1] = in_z.y;
    matrix[2][2] = in_z.z;
    matrix[2][3] = in_z.w;
    matrix[3][0] = in_w.x;
    matrix[3][1] = in_w.y;
    matrix[3][2] = in_w.z;
    matrix[3][3] = in_w.w;
  }

  SonyOzMatrix4x4f operator*(SonyOzMatrix4x4f a) {
    SonyOzVector4f m_0 =
        SonyOzVector4f(matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
    SonyOzVector4f m_1 =
        SonyOzVector4f(matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]);
    SonyOzVector4f m_2 =
        SonyOzVector4f(matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
    SonyOzVector4f m_3 =
        SonyOzVector4f(matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);

    SonyOzVector4f a_0 = SonyOzVector4f(a.matrix[0][0], a.matrix[1][0],
                                        a.matrix[2][0], a.matrix[3][0]);
    SonyOzVector4f a_1 = SonyOzVector4f(a.matrix[0][1], a.matrix[1][1],
                                        a.matrix[2][1], a.matrix[3][1]);
    SonyOzVector4f a_2 = SonyOzVector4f(a.matrix[0][2], a.matrix[1][2],
                                        a.matrix[2][2], a.matrix[3][2]);
    SonyOzVector4f a_3 = SonyOzVector4f(a.matrix[0][3], a.matrix[1][3],
                                        a.matrix[2][3], a.matrix[3][3]);

    float m_00 = m_0 * a_0, m_01 = m_0 * a_1, m_02 = m_0 * a_2,
          m_03 = m_0 * a_3;
    float m_10 = m_1 * a_0, m_11 = m_1 * a_1, m_12 = m_1 * a_2,
          m_13 = m_1 * a_3;
    float m_20 = m_2 * a_0, m_21 = m_2 * a_1, m_22 = m_2 * a_2,
          m_23 = m_2 * a_3;
    float m_30 = m_3 * a_0, m_31 = m_3 * a_1, m_32 = m_3 * a_2,
          m_33 = m_3 * a_3;

    return SonyOzMatrix4x4f(SonyOzVector4f(m_00, m_01, m_02, m_03),
                            SonyOzVector4f(m_10, m_11, m_12, m_13),
                            SonyOzVector4f(m_20, m_21, m_22, m_23),
                            SonyOzVector4f(m_30, m_31, m_32, m_33));
  }
};

#endif  // XR_MATH_H_
