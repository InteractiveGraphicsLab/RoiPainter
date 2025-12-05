/**
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

#pragma unmanaged //added by Takashi

#include "xr_api_wrapper.h"

#include <xr-runtime-common/xr_windows.h>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "load_library_utility.h"

namespace {
std::unique_ptr<sony::oz::xr_runtime::utility::Library> library;
}  // namespace

namespace sony::oz::xr_runtime {
SonyOzResult LinkXrLibrary(SonyOzPlatformId platform_id) {
#ifdef _WIN64
  if (!library) {
    //try {
      // To DO: Verify why hes xr runtime develop branch version is 1.0.0.0
      //DWORD version[4] = {0};
      //auto version_result =
      //    utility::ValidateLibraryVesion(platform_id, version);
      ///*
      //if (version_result.first == false) {
      //  //::MessageBoxW(NULL, version_result.second.c_str(), L"Warning", MB_OK);
      //}
      //*/
      //if (version[0] == 1) {
      //  return SonyOzResult::ERROR_RUNTIME_UNSUPPORTED;
      //}
      library = utility::LinkXrLibraryWin64(platform_id);

      if (!library) {
        return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
      }
    //} catch (std::runtime_error e) {
    //  return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
    //} catch (...) {
    //  return SonyOzResult::ERROR_RUNTIME_FAILURE;
    //}
  }
  return SonyOzResult::SUCCESS;
#else   // _WIN64
        // Win64 is the only platfrom currently supported.
  return SonyOzResult::ERROR_RUNTIME_FAILURE;
#endif  // _WIN64
}

void UnlinkXrLibrary() {
#ifdef _WIN64
  utility::UnlinkXrLibraryWin64();
  library.reset();
#endif  // _WIN64
}

SonyOzResult GetDeviceNum(SonyOzPlatformId platform_id, uint64_t* num) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozGetDeviceNum) {
    return library->sony_ozGetDeviceNum(platform_id, num);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult EnumerateDevices(SonyOzPlatformId platform_id, uint64_t size,
                              SonyOzDeviceInfo* device_list) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozEnumerateDevices) {
    return library->sony_ozEnumerateDevices(platform_id, size, device_list);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult SetDebugLogCallback(SonyOzPlatformId platform_id,
                                 SonyOzLogSettings_LogCallback callback) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozSetDebugLogCallback) {
    return library->sony_ozSetDebugLogCallback(platform_id, callback);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult CreateSession(SonyOzPlatformId platform_id,
                           const SonyOzDeviceInfo* device,
                           RUNTIME_OPTION_BIT_FLAG runtime_option_bit_flag,
                           PLATFORM_OPTION_BIT_FLAG platform_option_bit_flag,
                           SonyOzSessionHandle* session) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozCreateSession) {
    return library->sony_ozCreateSession(platform_id, device,
                                         runtime_option_bit_flag,
                                         platform_option_bit_flag, session);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult DestroySession(SonyOzSessionHandle* session) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozDestroySession) {
    return library->sony_ozDestroySession(session);
  }

  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult BeginSession(SonyOzSessionHandle session) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozBeginSession) {
    return library->sony_ozBeginSession(session);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult EndSession(SonyOzSessionHandle session) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozEndSession) {
    return library->sony_ozEndSession(session);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult GetSessionState(SonyOzSessionHandle session,
                             SonyOzSessionState* session_state) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozGetSessionState) {
    return library->sony_ozGetSessionState(session, session_state);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult UpdateTrackingResultCache(SonyOzSessionHandle session) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozUpdateTrackingResultCache) {
    return library->sony_ozUpdateTrackingResultCache(session);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult GetCachedPose(SonyOzSessionHandle session, SonyOzPoseId pose_id,
                           SonyOzPosef* pose, bool* is_valid) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozGetCachedPose) {
    return library->sony_ozGetCachedPose(session, pose_id, pose, is_valid);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult GetProjection(SonyOzSessionHandle session, SonyOzPoseId pose_id,
                           SonyOzProjection* projection) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozGetProjection) {
    return library->sony_ozGetProjection(session, pose_id, projection);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult SubmitD3d11(SonyOzSessionHandle session, ID3D11Device* device,
                         ID3D11Texture2D* side_by_side, bool flip_y_position,
                         ID3D11Texture2D* target) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozSubmitD3d11) {
    return library->sony_ozSubmitD3d11(session, device, side_by_side,
                                       flip_y_position, target);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult SubmitD3d12(SonyOzSessionHandle session,
                         ID3D12CommandQueue* command_queue, uint32_t node_mask,
                         ID3D12Resource* side_by_side, bool flip_y_position,
                         DXGI_FORMAT format, ID3D12Resource* target) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozSubmitD3d12) {
    return library->sony_ozSubmitD3d12(session, command_queue, node_mask,
                                       side_by_side, flip_y_position, format,
                                       target);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult SubmitOpengl(SonyOzSessionHandle session,
                          unsigned int side_by_side, bool flip_y_position,
                          unsigned int target) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozSubmitOpengl) {
    return library->sony_ozSubmitOpengl(session, side_by_side, flip_y_position,
                                        target);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult SubmitMultiPassD3d11(SonyOzSessionHandle session,
                                  ID3D11Device* device, ID3D11Texture2D* left,
                                  ID3D11Texture2D* right, bool flip_y_position,
                                  ID3D11Texture2D* target) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozSubmitMultiPassD3d11) {
    return library->sony_ozSubmitMultiPassD3d11(session, device, left, right,
                                                flip_y_position, target);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult SubmitMultiPassD3d12(SonyOzSessionHandle session,
                                  ID3D12CommandQueue* command_queue,
                                  uint32_t node_mask, ID3D12Resource* left,
                                  ID3D12Resource* right, bool flip_y_position,
                                  DXGI_FORMAT format, ID3D12Resource* target) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozSubmitMultiPassD3d12) {
    return library->sony_ozSubmitMultiPassD3d12(
        session, command_queue, node_mask, left, right, flip_y_position, format,
        target);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult SubmitMultiPassOpengl(SonyOzSessionHandle session,
                                   unsigned int left, unsigned int right,
                                   bool flip_y_position, unsigned int target) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozSubmitMultiPassOpengl) {
    return library->sony_ozSubmitMultiPassOpengl(session, left, right,
                                                 flip_y_position, target);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult EnableStereo(SonyOzSessionHandle session, bool enable) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozEnableStereo) {
    return library->sony_ozEnableStereo(session, enable);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult SetColorSpace(SonyOzSessionHandle session, int input_gamma_count,
                           int output_gamma_count, float gamma) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozSetColorSpace) {
    return library->sony_ozSetColorSpace(session, input_gamma_count,
                                         output_gamma_count, gamma);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult GetTargetMonitorRectangle(SonyOzSessionHandle session,
                                       SonyOzRect* rect) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozGetTargetMonitorRectangle) {
    return library->sony_ozGetTargetMonitorRectangle(session, rect);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult GetDisplaySpec(SonyOzSessionHandle session,
                            SonyOzDisplaySpec* display_spec) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozGetDisplaySpec) {
    return library->sony_ozGetDisplaySpec(session, display_spec);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult GetXrSystemError(SonyOzSessionHandle session,
                              SonyOzXrSystemError* error) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozGetXrSystemError) {
    return library->sony_ozGetXrSystemError(session, error);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult GetXrSystemErrorNum(SonyOzSessionHandle session, uint16_t* num) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozGetXrSystemErrorNum) {
    return library->sony_ozGetXrSystemErrorNum(session, num);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

SonyOzResult GetXrSystemErrorList(SonyOzSessionHandle session, uint16_t num,
                                  SonyOzXrSystemError* errors) {
  if (!library) {
    return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
  }

  if (library->sony_ozGetXrSystemErrorList) {
    return library->sony_ozGetXrSystemErrorList(session, num, errors);
  }
  return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
}

}  // namespace sony::oz::xr_runtime
