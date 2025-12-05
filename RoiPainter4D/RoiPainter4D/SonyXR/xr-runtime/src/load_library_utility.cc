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

#pragma unmanaged //added by Takashi

#include "load_library_utility.h"

#include <stdexcept>
#include <string>

#include "registry_utility.h"

#pragma comment(lib, "version.lib")

XR_DEFINE_HANDLE(SonyOzSessionHandle);

namespace {
constexpr DWORD dll_version[4] = {1, 0, 0, 0};
HMODULE dll;

class RegistryError : public std::runtime_error {
 public:
  RegistryError(const char* message, LONG error_code)
      : std::runtime_error(message), error_code_(error_code) {}

  LONG ErrorCode() const noexcept { return error_code_; }

 private:
  LONG error_code_;
};
}  // namespace

namespace sony::oz::xr_runtime::utility {
std::pair<bool, std::wstring> ValidateLibraryVesion(
    SonyOzPlatformId platform_id, DWORD version[VERSION_SIZE]) {
  std::wstring error_message = L"";
  auto directory = GetRegistryPathW(platform_id);
  if (directory.empty()) {
    error_message = L"Cannot read string from registry";
  }

  bool result = false;
  auto path = directory + L"lib\\xr_runtime.dll";
  DWORD info_size = ::GetFileVersionInfoSizeW(path.c_str(), NULL);
  void* buf = malloc(sizeof(char) * info_size);
  if (buf != nullptr && info_size &&
      ::GetFileVersionInfoW(path.c_str(), NULL, info_size, buf)) {
    VS_FIXEDFILEINFO* file_info;
    UINT len;
    if (::VerQueryValueW(buf, (L"\\"), (LPVOID*)&file_info, &len)) {
      version[0] = int(HIWORD(file_info->dwFileVersionMS));
      version[1] = int(LOWORD(file_info->dwFileVersionMS));
      version[2] = int(HIWORD(file_info->dwFileVersionLS));
      version[3] = int(LOWORD(file_info->dwFileVersionLS));
      result =
          (memcmp(version, dll_version, VERSION_SIZE * sizeof(DWORD)) == 0);
      if (result == false) {
        auto version_string = [](const DWORD in[VERSION_SIZE]) -> std::wstring {
          return std::to_wstring(in[0]) + L"," + std::to_wstring(in[1]) + L"," +
                 std::to_wstring(in[2]) + L"," + std::to_wstring(in[3]);
        };
        error_message = L"version mismatch.\n install version: " +
                        version_string(version) + L"\n wrapper version: " +
                        version_string(dll_version);
      }
    } else {
      error_message = L"Cannot get dll file version info";
    }
  } else {
    error_message = L"Cannot get dll file version";
  }
  free(buf);
  buf = nullptr;

  return {result, error_message};
}

std::unique_ptr<Library> LinkXrLibraryWin64(SonyOzPlatformId platform_id) {
  SetPlatformId(platform_id);
  auto directory = GetRegistryPathW(platform_id);
  if (directory.empty()) {
    throw RegistryError("Cannot read string from registry", GetLastError());
  }

  size_t envStringSize = 0, newEnvStringSize = 0;
  wchar_t* newEnvString = nullptr;
  std::wstring srdPath = std::wstring(L";") + directory + L"lib";

  if (_wgetenv_s(&envStringSize, NULL, 0, L"PATH") == 0) {
    newEnvStringSize = envStringSize + srdPath.length() + 1;
    newEnvString = new wchar_t[newEnvStringSize];
    if (newEnvString != nullptr &&
        _wgetenv_s(&envStringSize, newEnvString, envStringSize, L"PATH") == 0) {
      wcscat_s(newEnvString, newEnvStringSize, srdPath.c_str());
      _wputenv_s(L"PATH", newEnvString);
    }
    if (newEnvString != nullptr) {
      delete[] newEnvString;
    }
  }

  // get program files folder path from directory
  int pos = static_cast<int>(directory.find(L"Sony"));
  if (pos != std::string::npos) {
    std::wstring programfiles_path = directory.substr(0, pos);
    if (_wgetenv_s(&envStringSize, NULL, 0, L"STAPI_BIN_PATH_V1_1") == 0 &&
        envStringSize == 0) {
      std::wstring stapi_bin_path =
          programfiles_path +
          L"Common Files\\OMRON_SENTECH\\StApi\\v1_1\\;C:\\Program "
          L"Files\\Common Files\\OMRON_SENTECH\\GenICam\\v3_2\\bin\\";
      _wputenv_s(L"STAPI_BIN_PATH_V1_1", stapi_bin_path.c_str());
    }
    if (_wgetenv_s(&envStringSize, NULL, 0,
                   L"STAPI_GENICAM_GENTL64_PATH_V1_5") == 0 &&
        envStringSize == 0) {
      std::wstring stapi_bin_path =
          programfiles_path + L"Common Files\\OMRON_SENTECH\\GenTL\\v1_5\\";
      _wputenv_s(L"STAPI_GENICAM_GENTL64_PATH_V1_5", stapi_bin_path.c_str());
    }
    if (_wgetenv_s(&envStringSize, NULL, 0, L"GENICAM_GENTL64_PATH") == 0 &&
        envStringSize == 0) {
      std::wstring stapi_bin_path =
          programfiles_path + L"Common Files\\OMRON_SENTECH\\GenTL\\v1_5\\";
      _wputenv_s(L"GENICAM_GENTL64_PATH", stapi_bin_path.c_str());
    }
  }

  AddDllDirectory((directory + L"lib").c_str());
  AddDllDirectory(
      L"C:\\Program Files\\Common Files\\OMRON_SENTECH\\StApi\\v1_1");
  AddDllDirectory(
      L"C:\\Program Files\\Common Files\\OMRON_SENTECH\\GenICam\\v3_2\\bin");
  auto opencv_path = directory + L"lib\\opencv_world330.dll";
  dll = LoadLibraryExW(
      opencv_path.c_str(), NULL,
      LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32);

  auto internal_dll_path = directory + L"lib\\xr_runtime.dll";
  dll = LoadLibraryExW(
      internal_dll_path.c_str(), NULL,
      LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32);
  if (!dll) {
    return {};
  }

#define GET_FUNCTION(name)                          \
  lib->name = (name##_t)GetProcAddress(dll, #name); \
  if (!lib->name) {                                 \
    throw std::runtime_error("Not found " #name);   \
  }

  auto lib = std::make_unique<Library>();

  GET_FUNCTION(sony_ozGetDeviceNum);
  GET_FUNCTION(sony_ozEnumerateDevices);

  GET_FUNCTION(sony_ozSetDebugLogCallback);

  GET_FUNCTION(sony_ozCreateSession);
  GET_FUNCTION(sony_ozDestroySession);
  GET_FUNCTION(sony_ozBeginSession);
  GET_FUNCTION(sony_ozEndSession);
  GET_FUNCTION(sony_ozGetSessionState);

  GET_FUNCTION(sony_ozUpdateTrackingResultCache);
  GET_FUNCTION(sony_ozGetCachedPose);
  GET_FUNCTION(sony_ozGetProjection);

  GET_FUNCTION(sony_ozSubmitD3d11);
  GET_FUNCTION(sony_ozSubmitD3d12);
  GET_FUNCTION(sony_ozSubmitOpengl);
  GET_FUNCTION(sony_ozSubmitMultiPassD3d11);
  GET_FUNCTION(sony_ozSubmitMultiPassD3d12);
  GET_FUNCTION(sony_ozSubmitMultiPassOpengl);

  GET_FUNCTION(sony_ozEnableStereo);
  GET_FUNCTION(sony_ozSetColorSpace);
  GET_FUNCTION(sony_ozGetTargetMonitorRectangle);
  GET_FUNCTION(sony_ozGetDisplaySpec);

  GET_FUNCTION(sony_ozGetXrSystemError);
  GET_FUNCTION(sony_ozGetXrSystemErrorNum);
  GET_FUNCTION(sony_ozGetXrSystemErrorList);

#undef GET_FUNCTION

  return lib;
}

void UnlinkXrLibraryWin64() {
  if (dll) {
    FreeLibrary(dll);
  }
}

}  // namespace sony::oz::xr_runtime::utility
