/**
 * @file xr_api_wrapper.h
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
#ifndef XR_API_WRAPPER_H_
#define XR_API_WRAPPER_H_

#include "log-settings/log_defs.h"
#include "xr-runtime-common/xr_api_defs.h"
#include "xr-runtime-common/xr_platform_option.h"
#include "xr-runtime-common/xr_runtime_option.h"

// = Standard Library =============================
#include <vector>

XR_DEFINE_HANDLE(SonyOzSessionHandle);

/**
 * @namespace sony::oz
 * @brief API
 */
namespace sony::oz::xr_runtime {

/******************************************************************************
 * Load/Unload xr runtime library process
 */

SonyOzResult LinkXrLibrary(SonyOzPlatformId platform_id);
void UnlinkXrLibrary();

/******************************************************************************
 * Device library process
 */

SonyOzResult GetDeviceNum(SonyOzPlatformId platform_id, uint64_t* num);
SonyOzResult EnumerateDevices(SonyOzPlatformId platform_id, uint64_t size,
                              SonyOzDeviceInfo* device_list);

/******************************************************************************
 * Log process
 */

SonyOzResult SetDebugLogCallback(SonyOzPlatformId platform_id,
                                 SonyOzLogSettings_LogCallback callback);

/******************************************************************************
 * Session process
 */

SonyOzResult CreateSession(SonyOzPlatformId platform_id,
                           const SonyOzDeviceInfo* device,
                           RUNTIME_OPTION_BIT_FLAG runtime_option_bit_flag,
                           PLATFORM_OPTION_BIT_FLAG platform_option_bit_flag,
                           SonyOzSessionHandle* session);
SonyOzResult DestroySession(SonyOzSessionHandle* session);
SonyOzResult BeginSession(SonyOzSessionHandle session);
SonyOzResult EndSession(SonyOzSessionHandle session);
SonyOzResult GetSessionState(SonyOzSessionHandle session,
                             SonyOzSessionState* session_state);

/******************************************************************************
 * Tracking process
 */

SonyOzResult UpdateTrackingResultCache(SonyOzSessionHandle session);
SonyOzResult GetCachedPose(SonyOzSessionHandle session, SonyOzPoseId pose_id,
                           SonyOzPosef* pose, bool* is_valid);
SonyOzResult GetProjection(SonyOzSessionHandle session, SonyOzPoseId pose_id,
                           SonyOzProjection* projection);

/******************************************************************************
 * Rendering process
 */

SonyOzResult SubmitD3d11(SonyOzSessionHandle session, ID3D11Device* device,
                         ID3D11Texture2D* side_by_side, bool flip_y_position,
                         ID3D11Texture2D* target);
SonyOzResult SubmitD3d12(SonyOzSessionHandle session,
                         ID3D12CommandQueue* command_queue, uint32_t node_mask,
                         ID3D12Resource* side_by_side, bool flip_y_position,
                         DXGI_FORMAT format, ID3D12Resource* target);
SonyOzResult SubmitOpengl(SonyOzSessionHandle session,
                          unsigned int side_by_side, bool flip_y_position,
                          unsigned int target);
SonyOzResult SubmitMultiPassD3d11(SonyOzSessionHandle session,
                                  ID3D11Device* device, ID3D11Texture2D* left,
                                  ID3D11Texture2D* right, bool flip_y_position,
                                  ID3D11Texture2D* target);
SonyOzResult SubmitMultiPassD3d12(SonyOzSessionHandle session,
                                  ID3D12CommandQueue* command_queue,
                                  uint32_t node_mask, ID3D12Resource* left,
                                  ID3D12Resource* right, bool flip_y_position,
                                  DXGI_FORMAT format, ID3D12Resource* target);
SonyOzResult SubmitMultiPassOpengl(SonyOzSessionHandle session,
                                   unsigned int left, unsigned int right,
                                   bool flip_y_position, unsigned int target);

/******************************************************************************
 * Data set/get process
 */

SonyOzResult EnableStereo(SonyOzSessionHandle session, bool enable);
SonyOzResult SetColorSpace(SonyOzSessionHandle session, int input_gamma_count,
                           int output_gamma_count, float gamma);
SonyOzResult GetTargetMonitorRectangle(SonyOzSessionHandle session,
                                       SonyOzRect* rect);
SonyOzResult GetDisplaySpec(SonyOzSessionHandle session,
                            SonyOzDisplaySpec* display_spec);

/******************************************************************************
 * System error process
 */

SonyOzResult GetXrSystemError(SonyOzSessionHandle session,
                              SonyOzXrSystemError* error);
SonyOzResult GetXrSystemErrorNum(SonyOzSessionHandle session, uint16_t* num);
SonyOzResult GetXrSystemErrorList(SonyOzSessionHandle session, uint16_t num,
                                  SonyOzXrSystemError* errors);
}  // namespace sony::oz::xr_runtime

#endif  // XR_API_WRAPPER_H_
