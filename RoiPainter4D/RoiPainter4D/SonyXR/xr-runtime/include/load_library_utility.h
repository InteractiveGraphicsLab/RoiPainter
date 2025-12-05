/**
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
#ifndef LOAD_LIBRARY_UTILITY_H_
#define LOAD_LIBRARY_UTILITY_H_

#include <memory>
#include <utility>
#include <vector>

#include "log-settings/log_defs.h"
#include "xr-runtime-common/xr_api_defs.h"
#include "xr-runtime-common/xr_platform_option.h"
#include "xr-runtime-common/xr_runtime_option.h"
#include "xr-runtime-common/xr_windows.h"
XR_DEFINE_HANDLE(SonyOzSessionHandle);

namespace sony::oz::xr_runtime::utility {

/******************************************************************************
 * Device library process
 */

using sony_ozGetDeviceNum_t = SonyOzResult (*)(SonyOzPlatformId, uint64_t*);
using sony_ozEnumerateDevices_t = SonyOzResult (*)(SonyOzPlatformId, uint64_t,
                                                   SonyOzDeviceInfo*);

/******************************************************************************
 * Log process
 */

using sony_ozSetDebugLogCallback_t =
    SonyOzResult (*)(SonyOzPlatformId, SonyOzLogSettings_LogCallback);

/******************************************************************************
 * Session process
 */

using sony_ozCreateSession_t = SonyOzResult (*)(SonyOzPlatformId,
                                                const SonyOzDeviceInfo*,
                                                RUNTIME_OPTION_BIT_FLAG,
                                                PLATFORM_OPTION_BIT_FLAG,
                                                SonyOzSessionHandle*);
using sony_ozDestroySession_t = SonyOzResult (*)(SonyOzSessionHandle*);
using sony_ozBeginSession_t = SonyOzResult (*)(SonyOzSessionHandle);
using sony_ozEndSession_t = SonyOzResult (*)(SonyOzSessionHandle);
using sony_ozGetSessionState_t = SonyOzResult (*)(SonyOzSessionHandle,
                                                  SonyOzSessionState*);

/******************************************************************************
 * Tracking process
 */

using sony_ozUpdateTrackingResultCache_t =
    SonyOzResult (*)(SonyOzSessionHandle);
using sony_ozGetCachedPose_t = SonyOzResult (*)(SonyOzSessionHandle,
                                                SonyOzPoseId, SonyOzPosef*,
                                                bool*);
using sony_ozGetProjection_t = SonyOzResult (*)(SonyOzSessionHandle,
                                                SonyOzPoseId,
                                                SonyOzProjection*);

/******************************************************************************
 * Rendering process
 */

using sony_ozSubmitD3d11_t = SonyOzResult (*)(SonyOzSessionHandle,
                                              ID3D11Device*, ID3D11Texture2D*,
                                              bool, ID3D11Texture2D*);
using sony_ozSubmitD3d12_t = SonyOzResult (*)(SonyOzSessionHandle,
                                              ID3D12CommandQueue*, uint32_t,
                                              ID3D12Resource*, bool,
                                              DXGI_FORMAT, ID3D12Resource*);
using sony_ozSubmitOpengl_t = SonyOzResult (*)(SonyOzSessionHandle,
                                               unsigned int, bool,
                                               unsigned int);
using sony_ozSubmitMultiPassD3d11_t = SonyOzResult (*)(SonyOzSessionHandle,
                                                       ID3D11Device*,
                                                       ID3D11Texture2D*,
                                                       ID3D11Texture2D*, bool,
                                                       ID3D11Texture2D*);
using sony_ozSubmitMultiPassD3d12_t = SonyOzResult (*)(
    SonyOzSessionHandle, ID3D12CommandQueue*, uint32_t, ID3D12Resource*,
    ID3D12Resource*, bool, DXGI_FORMAT, ID3D12Resource*);
using sony_ozSubmitMultiPassOpengl_t = SonyOzResult (*)(SonyOzSessionHandle,
                                                        unsigned int,
                                                        unsigned int, bool,
                                                        unsigned int);

/******************************************************************************
 * Data set/get process
 */

using sony_ozEnableStereo_t = SonyOzResult (*)(SonyOzSessionHandle, bool);
using sony_ozSetColorSpace_t = SonyOzResult (*)(SonyOzSessionHandle, int, int,
                                                float);
using sony_ozGetTargetMonitorRectangle_t = SonyOzResult (*)(SonyOzSessionHandle,
                                                            SonyOzRect*);
using sony_ozGetDisplaySpec_t = SonyOzResult (*)(SonyOzSessionHandle,
                                                 SonyOzDisplaySpec*);

/******************************************************************************
 * System error process
 */

using sony_ozGetXrSystemError_t = SonyOzResult (*)(SonyOzSessionHandle,
                                                   SonyOzXrSystemError*);
using sony_ozGetXrSystemErrorNum_t = SonyOzResult (*)(SonyOzSessionHandle,
                                                      uint16_t*);
using sony_ozGetXrSystemErrorList_t = SonyOzResult (*)(SonyOzSessionHandle,
                                                       uint16_t,
                                                       SonyOzXrSystemError*);

struct Library {
  sony_ozGetDeviceNum_t sony_ozGetDeviceNum;
  sony_ozEnumerateDevices_t sony_ozEnumerateDevices;

  sony_ozSetDebugLogCallback_t sony_ozSetDebugLogCallback;

  sony_ozCreateSession_t sony_ozCreateSession;
  sony_ozDestroySession_t sony_ozDestroySession;
  sony_ozBeginSession_t sony_ozBeginSession;
  sony_ozEndSession_t sony_ozEndSession;
  sony_ozGetSessionState_t sony_ozGetSessionState;

  sony_ozUpdateTrackingResultCache_t sony_ozUpdateTrackingResultCache;
  sony_ozGetCachedPose_t sony_ozGetCachedPose;
  sony_ozGetProjection_t sony_ozGetProjection;

  sony_ozSubmitD3d11_t sony_ozSubmitD3d11;
  sony_ozSubmitD3d12_t sony_ozSubmitD3d12;
  sony_ozSubmitOpengl_t sony_ozSubmitOpengl;
  sony_ozSubmitMultiPassD3d11_t sony_ozSubmitMultiPassD3d11;
  sony_ozSubmitMultiPassD3d12_t sony_ozSubmitMultiPassD3d12;
  sony_ozSubmitMultiPassOpengl_t sony_ozSubmitMultiPassOpengl;

  sony_ozEnableStereo_t sony_ozEnableStereo;
  sony_ozSetColorSpace_t sony_ozSetColorSpace;
  sony_ozGetTargetMonitorRectangle_t sony_ozGetTargetMonitorRectangle;
  sony_ozGetDisplaySpec_t sony_ozGetDisplaySpec;

  sony_ozGetXrSystemError_t sony_ozGetXrSystemError;
  sony_ozGetXrSystemErrorNum_t sony_ozGetXrSystemErrorNum;
  sony_ozGetXrSystemErrorList_t sony_ozGetXrSystemErrorList;
};

constexpr uint32_t VERSION_SIZE{4};

std::pair<bool, std::wstring> ValidateLibraryVesion(
    SonyOzPlatformId platform_id, DWORD version[VERSION_SIZE]);
std::unique_ptr<Library> LinkXrLibraryWin64(SonyOzPlatformId platform_id);
void UnlinkXrLibraryWin64();

}  // namespace sony::oz::xr_runtime::utility

#endif  // LOAD_LIBRARY_UTILITY_H_
