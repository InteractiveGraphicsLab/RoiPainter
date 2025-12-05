/**
 * Sony CONFIDENTIAL
 *
 * Copyright 2022 Sony Corporation
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

#include "xr_basic_api_wrapper.h"
#include "xr-runtime-common/xr_windows.h"

namespace srdisplay::basic::api {
  using SetCameraWindowEnabled_t = SonyOzResult(*)(SonyOzSessionHandle, const bool);
  SetCameraWindowEnabled_t SetCameraWindowEnabled = nullptr;

  using GetCrosstalkCorrectionSettings_t = SonyOzResult(*)(SonyOzSessionHandle, sony::oz::srd_base_settings::SrdXrCrosstalkCorrectionMode*);
  GetCrosstalkCorrectionSettings_t GetCrosstalkCorrectionSettings = nullptr;

  using SetCrosstalkCorrectionSettings_t = SonyOzResult(*)(SonyOzSessionHandle, sony::oz::srd_base_settings::SrdXrCrosstalkCorrectionMode);
  SetCrosstalkCorrectionSettings_t SetCrosstalkCorrectionSettings = nullptr;

  using GetStubHeadPose_t = SonyOzResult(*)(SonyOzSessionHandle, SonyOzPosef*);
  GetStubHeadPose_t GetStubHeadPose = nullptr;

  using SetStubHeadPose_t = SonyOzResult(*)(SonyOzSessionHandle, const SonyOzPosef);
  SetStubHeadPose_t SetStubHeadPose = nullptr;

  std::wstring GetInstallPath() {

    HKEY hkey = HKEY_LOCAL_MACHINE;
    const std::wstring sub_key = std::wstring(L"SOFTWARE\\Sony Corporation\\Spatial Reality Display");
    const std::wstring value = L"Path";

    DWORD data_size{};
    LONG return_code =
      ::RegGetValueW(hkey, sub_key.c_str(), value.c_str(), RRF_RT_REG_SZ,
                     nullptr, nullptr, &data_size);
    if (return_code != ERROR_SUCCESS) {
      return L"";
    }

    std::wstring data;
    data.resize(data_size / sizeof(wchar_t));

    return_code = ::RegGetValueW(hkey, sub_key.c_str(), value.c_str(),
                                 RRF_RT_REG_SZ, nullptr, &data[0], &data_size);
    if (return_code != ERROR_SUCCESS) {
      return L"";
    }

    DWORD string_length_in_wchars = data_size / sizeof(wchar_t);

    // Exclude the NULL written by the Win32 API
    string_length_in_wchars--;

    data.resize(string_length_in_wchars);
    return data;
  }

  bool LoadXrRuntimeLibrary() {
    static HMODULE handle = nullptr;

    if (handle) {
      return true;
    }

    std::wstring directry = GetInstallPath();
    std::wstring xrmw_path = directry + L"lib\\xr_runtime.dll";
    handle = ::LoadLibraryW(xrmw_path.c_str());

    if (handle == nullptr) {
      return false;
    }

#define GET_FUNCTION(name)                        \
  name = (name##_t)GetProcAddress(handle, #name); \
  if (!name) {                                    \
    return false;                                 \
  }

    GET_FUNCTION(SetCameraWindowEnabled);
    GET_FUNCTION(GetCrosstalkCorrectionSettings);
    GET_FUNCTION(SetCrosstalkCorrectionSettings);
    GET_FUNCTION(GetStubHeadPose);
    GET_FUNCTION(SetStubHeadPose);

    return true;
  }
}

namespace sony::oz::xr_runtime {
  SonyOzResult SetCameraWindowEnabled(SonyOzPlatformId platform_id, SonyOzSessionHandle session, const bool enable) {
    if (srdisplay::basic::api::LoadXrRuntimeLibrary() == false) {
      return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
    }

    if (srdisplay::basic::api::SetCameraWindowEnabled == nullptr) {
      return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
    }

    return srdisplay::basic::api::SetCameraWindowEnabled(session, enable);
  }

  SonyOzResult GetCrosstalkCorrectionSettings(SonyOzPlatformId platform_id, SonyOzSessionHandle session, sony::oz::srd_base_settings::SrdXrCrosstalkCorrectionMode* mode) {
    if (srdisplay::basic::api::LoadXrRuntimeLibrary() == false) {
      return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
    }

    if (srdisplay::basic::api::GetCrosstalkCorrectionSettings == nullptr) {
      return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
    }

    return srdisplay::basic::api::GetCrosstalkCorrectionSettings(session, mode);
  }

  SonyOzResult SetCrosstalkCorrectionSettings(SonyOzPlatformId platform_id, SonyOzSessionHandle session, const sony::oz::srd_base_settings::SrdXrCrosstalkCorrectionMode mode) {
    if (srdisplay::basic::api::LoadXrRuntimeLibrary() == false) {
      return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
    }

    if (srdisplay::basic::api::SetCrosstalkCorrectionSettings == nullptr) {
      return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
    }

    return srdisplay::basic::api::SetCrosstalkCorrectionSettings(session, mode);
  }

  SonyOzResult GetStubHeadPose(SonyOzSessionHandle session, SonyOzPosef* pose) {
    if (srdisplay::basic::api::LoadXrRuntimeLibrary() == false) {
      return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
    }
    if (srdisplay::basic::api::GetStubHeadPose == nullptr) {
      return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
    }

    return srdisplay::basic::api::GetStubHeadPose(session, pose);
  }

  SonyOzResult SetStubHeadPose(SonyOzSessionHandle session, const SonyOzPosef pose) {
    if (srdisplay::basic::api::LoadXrRuntimeLibrary() == false) {
      return SonyOzResult::ERROR_RUNTIME_NOT_FOUND;
    }
    if (srdisplay::basic::api::SetStubHeadPose == nullptr) {
      return SonyOzResult::ERROR_FUNCTION_UNSUPPORTED;
    }

    return srdisplay::basic::api::SetStubHeadPose(session, pose);
  }
}
