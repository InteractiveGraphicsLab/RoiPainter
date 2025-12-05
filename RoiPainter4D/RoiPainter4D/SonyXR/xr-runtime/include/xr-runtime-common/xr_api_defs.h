/**
 * @file xr_api_defs.h
 * @copyright
 *
 * Sony CONFIDENTIAL
 *
 * Copyright 2019,2020,2021,2022 Sony Corporation
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
#ifndef XR_API_DEFS_H_
#define XR_API_DEFS_H_

#include "xr_math.h"

// = Standard Library =============================
#include <stdint.h>
#include <wchar.h>

#include <string>

// XR_PTR_SIZE (in bytes)
#if (defined(__LP64__) || defined(_WIN64) ||                            \
     (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) || \
     defined(__ia64) || defined(_M_IA64) || defined(__aarch64__) ||     \
     defined(__powerpc64__))
#define XR_PTR_SIZE 8
#else
#define XR_PTR_SIZE 4
#endif

#if !defined(XR_DEFINE_HANDLE)
#if (XR_PTR_SIZE == 8)
#define XR_DEFINE_HANDLE(object) typedef struct object##_T* object;
#else
#define XR_DEFINE_HANDLE(object) typedef uint64_t object;
#endif
#endif

const float kDefaultDisplayWidth_m = 0.3442176f;
const float kDefaultDisplayHeight_m = 0.1936224f;
const float kDefaultDisplayTilt_rad = 0.785398f;  // = 45f * deg2rad
const uint32_t kDefaultHorizontalResolution_px = 3840;
const uint32_t kDefaultVerticalResolution_px = 2160;

/**
 * @enum SonyOzResult
 * @brief XR Runtime APIの結果
 */
enum class SonyOzResult {
  SUCCESS = 0,                   ///< 成功
  ERROR_RUNTIME_NOT_FOUND = -1,  ///< XR Runtimeがインストールされていない
  ERROR_VALIDATION_FAILURE = -2,  ///< 引数の不正による失敗
  ERROR_RUNTIME_FAILURE = -3,  ///< XR Runtimeの予期せぬ問題による失敗
  ERROR_FUNCTION_UNSUPPORTED = -4,  ///< 非サポートのAPI呼び出しによる失敗
  ERROR_HANDLE_INVALID = -5,  ///< 不正なハンドルの使用による失敗
  ERROR_SESSION_CREATED = -6,  ///< 既にSessionが生成されている
  ERROR_SESSION_READY = -7,  ///< 既にREADYであるSessionは実行できない
  ERROR_SESSION_STARTING = -8,  ///< 既にSTARTINGであるSessionは実行できない
  ERROR_SESSION_RUNNING = -9,  ///< 既にRUNNINGであるSessionは実行できない
  ERROR_SESSION_STOPPING = -10,  ///< 既にSTOPPINGであるSessionは実行できない
  ERROR_SESSION_NOT_CREATE = -11,  ///< Sessionが生成されていない
  ERROR_SESSION_NOT_READY = -12,  ///< READYではないSessionでは実行できない
  ERROR_SESSION_NOT_RUNNING = -13,  ///< RUNNINGではないSessionでは実行できない
  ERROR_SESSION_STILL_USED = -14,  ///< 別の場所でSessionが利用中
  ERROR_POSE_INVALID = -15,      ///< 正しい姿勢を取得できなかった
  ERROR_SET_DATA_FAILURE = -16,  ///< データのセットに失敗
  ERROR_GET_DATA_FAILURE = -17,  ///< データのゲットに失敗
  ERROR_FILE_ACCESS_ERROR = -18,  ///< 指定されたファイルへアクセスできない
  ERROR_DEVICE_NOT_FOUND = -19,  ///< 指定されたデバイスが発見できない
};

/**
 * @typedef SonyOzPlatformId
 * @brief XRプラットフォームを示すID
 */
typedef const char* SonyOzPlatformId;

/**
 * @enum SonyOzSessionState
 * @brief セッションの状態
 */
enum class SonyOzSessionState {
  UNKNOWN = 0b00000000,   ///< 不正な状態
  IDLE = 0b00000001,      ///< 初期状態
  READY = 0b00000010,     ///< 待機状態
  STARTING = 0b00000100,  ///< NodeGraphが起動し始めた状態
  RUNNING = 0b00001000,   ///< NodeGraphが起動している状態
  STOPPING = 0b00010000,  ///< TBD
};

/**
 * @struct SonyOzRect
 * @brief 長方形
 */
struct SonyOzRect {
  /// 左辺のX座標
  int32_t left;

  /// 上辺のY座標
  int32_t top;

  /// 右辺のX座標
  int32_t right;

  // 下辺のY座標
  int32_t bottom;
};

/**
 * @struct SonyOzDeviceInfo
 * @brief デバイスの情報
 */
const uint32_t kDeviceSerialNumberMaxSize = 256;
const uint32_t kDeviceProductIdMaxSize = 256;
struct SonyOzDeviceInfo {
  /// デバイスのインデックス
  uint32_t device_index;

  /// デバイスのシリアルナンバー
  char device_serial_number[kDeviceSerialNumberMaxSize] = "";

  /// デバイスのプロダクトID
  char product_id[kDeviceProductIdMaxSize] = "";

  /// SR Display の仮想スクリーン上の座標
  SonyOzRect target_monitor_rectangle;

  /// primary monitor の仮想スクリーン上の座標
  SonyOzRect primary_monitor_rectangle;

  SonyOzDeviceInfo(){};
  SonyOzDeviceInfo(const SonyOzDeviceInfo& obj)
      : device_index(obj.device_index),
        target_monitor_rectangle(obj.target_monitor_rectangle),
        primary_monitor_rectangle(obj.primary_monitor_rectangle) {
    strncpy_s(device_serial_number, obj.device_serial_number,
              _countof(device_serial_number));
    strncpy_s(product_id, obj.product_id, _countof(product_id));
  }
};

/**
 * @struct SonyOzFovf
 * @brief レンダリング・カメラの視野角
 */
struct SonyOzFovf {
  float angle_left;   ///< 左方向の視野角
  float angle_right;  ///< 右方向の視野角
  float angle_up;     ///< 上方向の視野角
  float angle_down;   ///< 下方向の視野角

  /**
   * @brief コンストラクタ
   */
  SonyOzFovf()
      : angle_left(0.f), angle_right(0.f), angle_up(0.f), angle_down(0.f) {}

  /**
   * @brief コンストラクタ
   * @param[in] in_angle 左・右・上・下方向の視野角に同じ値を指定する
   */
  SonyOzFovf(float in_angle)
      : angle_left(in_angle),
        angle_right(in_angle),
        angle_up(in_angle),
        angle_down(in_angle) {}

  /**
   * @brief コンストラクタ
   * @param[in] in_angle_left 左方向の視野角を指定する
   * @param[in] in_angle_right 右方向の視野角を指定する
   * @param[in] in_angle_up 上方向の視野角を指定する
   * @param[in] in_angle_down 下方向の視野角を指定する
   */
  SonyOzFovf(float in_angle_left, float in_angle_right, float in_angle_up,
             float in_angle_down)
      : angle_left(in_angle_left),
        angle_right(in_angle_right),
        angle_up(in_angle_up),
        angle_down(in_angle_down) {}
};

/**
 * @struct SonyOzFrustumPlanes
 * @brief  the view space coordinates of the near projection plane.
 *         (See:
 * https://docs.unity3d.com/ja/current/ScriptReference/FrustumPlanes.html,
 * https://docs.microsoft.com/ja-jp/windows/win32/api/directxmath/nf-directxmath-xmmatrixperspectiveoffcenterrh)
 */
struct SonyOzFrustumPlanesf {
  float viewLeft;
  float viewRight;
  float viewBottom;
  float viewTop;
  float nearZ;
  float farZ;

  /**
   * @brief Constructor
   * @param[in] horizontal_fov in radian
   * @param[in] vertical_fov  in radian
   * @param[in] near_clip in meter
   * @param[in] far_clip in meter
   */
  SonyOzFrustumPlanesf(float horizontal_fov, float vertical_fov,
                       float near_clip, float far_clip)
      : viewLeft(near_clip * tanf(horizontal_fov * 0.5f)),
        viewRight(-near_clip * tanf(horizontal_fov * 0.5f)),
        viewBottom(-near_clip * tanf(vertical_fov * 0.5f)),
        viewTop(near_clip * tanf(vertical_fov * 0.5f)),
        nearZ(near_clip),
        farZ(far_clip) {}

  /**
   * @brief Constructor
   */
  SonyOzFrustumPlanesf() : SonyOzFrustumPlanesf(1.47f, 0.70f, 0.15f, 10.f) {}

  /**
   * @brief Constructor
   */
  SonyOzFrustumPlanesf(float in_view_left, float in_view_right,
                       float in_view_bottom, float in_view_top, float in_near_z,
                       float in_far_z)
      : viewLeft(in_view_left),
        viewRight(in_view_right),
        viewBottom(in_view_bottom),
        viewTop(in_view_top),
        nearZ(in_near_z),
        farZ(in_far_z) {}

  SonyOzFovf ToFov() {
    return SonyOzFovf(atan2f(fabsf(viewLeft), fabsf(nearZ)),
                      atan2f(fabsf(viewRight), fabsf(nearZ)),
                      atan2f(fabsf(viewTop), fabsf(nearZ)),
                      atan2f(fabsf(viewBottom), fabsf(nearZ)));
  }
};

/**
 * @struct SonyOzPosef
 * @brief 姿勢（位置・方向）
 */
struct SonyOzPosef {
  /// 方向
  SonyOzQuaternionf orientation;

  /// 位置
  SonyOzVector3f position;

  /**
   * @brief コンストラクタ
   */
  SonyOzPosef() {}

  /**
   * @brief コンストラクタ
   * @param[in] in_orientation 方向を指定する
   * @param[in] in_position 位置を指定する
   */
  SonyOzPosef(SonyOzQuaternionf in_orientation, SonyOzVector3f in_position)
      : orientation(in_orientation), position(in_position) {}
};

/**
 * @struct SonyOzHeadPosef
 * @brief 姿勢（位置・方向）
 */
struct SonyOzHeadPosef {
  /// 顔（眼間中央）の姿勢
  SonyOzPosef pose;

  /// 左眼の姿勢
  SonyOzPosef left_eye_pose;

  /// 右眼の姿勢
  SonyOzPosef right_eye_pose;
};

/**
 * @struct SonyOzDisplayResolution
 * @brief ディスプレイの解像度
 */
struct SonyOzDisplayResolution {
  /// ディスプレイの横幅の解像度（px単位）
  uint32_t width;

  /// ディスプレイの縦幅の解像度（px単位）
  uint32_t height;

  /// ディスプレイの面積
  uint32_t area;

  /**
   * @brief コンストラクタ
   */
  SonyOzDisplayResolution() : width(0), height(0), area(width * height) {}

  /**
   * @brief コンストラクタ
   * @param[in] in_width ディスプレイの横幅の解像度（px単位）を指定する
   * @param[in] in_height ディスプレイの縦幅の解像度（px単位）を指定する
   */
  SonyOzDisplayResolution(uint32_t in_width, uint32_t in_height)
      : width(in_width), height(in_height), area(width * height) {}
};

/**
 * @struct SonyOzDisplaySize
 * @brief ディスプレイの寸法
 */
struct SonyOzDisplaySize {
  /// ディスプレイの横幅（m単位）
  float width_m;

  /// ディスプレイの縦幅（m単位）
  float height_m;

  /**
   * @brief コンストラクタ
   */
  SonyOzDisplaySize()
      : width_m(kDefaultDisplayWidth_m), height_m(kDefaultDisplayHeight_m) {}

  /**
   * @brief コンストラクタ
   * @param[in] in_width_m ディスプレイの横幅（m単位）を指定する
   * @param[in] in_height_m ディスプレイの縦幅（m単位）を指定する
   */
  SonyOzDisplaySize(float in_width_m, float in_height_m)
      : width_m(in_width_m), height_m(in_height_m) {}
};

/**
 * @struct SonyOzProjectionMatrix
 * @brief プロジェクション・マトリクス
 */
struct SonyOzProjectionMatrix {
  /// 顔（眼間中央）のレンダリングカメラ用のプロジェクション・マトリクス
  SonyOzMatrix4x4f projection;

  /// 左眼のレンダリングカメラ用のプロジェクション・マトリクス
  SonyOzMatrix4x4f left_projection;

  /// 右眼のレンダリングカメラ用のプロジェクション・マトリクス
  SonyOzMatrix4x4f right_projection;
};

/**
 * @struct SonyOzDisplaySpec
 * @brief ディスプレイの情報
 */
struct SonyOzDisplaySpec {
  /// ディスプレイの寸法
  SonyOzDisplaySize display_size;

  /// ディスプレイの解像度
  SonyOzDisplayResolution display_resolution;

  /// ディスプレイの傾き
  float display_tilt_rad;

  /**
   * @brief コンストラクタ
   */
  SonyOzDisplaySpec()
      : display_size(kDefaultDisplayWidth_m, kDefaultDisplayHeight_m),
        display_resolution(kDefaultHorizontalResolution_px,
                           kDefaultVerticalResolution_px),
        display_tilt_rad(kDefaultDisplayTilt_rad) {}

  /**
   * @brief コンストラクタ
   */
  SonyOzDisplaySpec(SonyOzDisplaySize in_display_size,
                    SonyOzDisplayResolution in_display_resolution,
                    float in_display_tilt_rad)
      : display_size(in_display_size),
        display_resolution(in_display_resolution),
        display_tilt_rad(in_display_tilt_rad) {}
};

/**
 * @enum SonyOzXrSystemErrorResult
 * @brief XR Runtime内で発生したシステムエラーの種類
 */
enum class SonyOzXrSystemErrorResult : uint8_t { Success, Warning, Error };

/**
 * @struct SonyOzXrSystemError
 * @brief XR Runtime内で発生したシステムエラー
 */
constexpr uint16_t kXrSystemErrorMsgSize = 512;
struct SonyOzXrSystemError {
  SonyOzXrSystemErrorResult result = SonyOzXrSystemErrorResult::Success;
  int32_t code = 0;
  char msg[kXrSystemErrorMsgSize] = "";

  SonyOzXrSystemError() {}
  SonyOzXrSystemError(SonyOzXrSystemErrorResult in_result, int32_t in_code,
                      const char* in_msg)
      : result(in_result), code(in_code) {
    strcpy_s(msg, kXrSystemErrorMsgSize, in_msg);
  };
};

enum class SonyOzPoseId : int32_t {
  HEAD = 0,
  LEFT_EYE = 1,
  RIGHT_EYE = 2,
};

struct SonyOzProjection {
  float half_angles_left;
  float half_angles_right;
  float half_angles_top;
  float half_angles_bottom;
};

struct ID3D11Device;
struct ID3D11Texture2D;
struct ID3D12CommandQueue;
struct ID3D12Resource;

enum DXGI_FORMAT;

#endif  // XR_API_DEFS_H_
