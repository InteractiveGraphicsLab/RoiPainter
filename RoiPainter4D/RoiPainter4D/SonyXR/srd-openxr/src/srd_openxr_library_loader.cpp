/**
 * Sony CONFIDENTIAL
 *
 * Copyright 2023 Sony Corporation
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


#include "srd_openxr_library_loader.h"

#include <stdexcept>

#include "registry_utility.h"
using namespace sony::oz::xr_runtime::utility;

namespace sony::openxr::extension {
    HMODULE g_dll;

    class RegistryError : public std::runtime_error {
    public:
        RegistryError(const char* message, LONG error_code)
            : std::runtime_error(message), error_code_(error_code) {}

        LONG ErrorCode() const noexcept { return error_code_; }

    private:
        LONG error_code_;
    };
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
        g_dll = LoadLibraryExW(
            opencv_path.c_str(), NULL,
            LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32);

        auto internal_dll_path = directory + L"lib\\xr_runtime.dll";
        g_dll = LoadLibraryExW(
            internal_dll_path.c_str(), NULL,
            LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!g_dll) {
            return {};
        }

#define GET_FUNCTION(name)                          \
  lib->name = (name##_t)GetProcAddress(g_dll, #name); \
  if (!lib->name) {                                 \
    throw std::runtime_error("Not found " #name);   \
  }

        auto lib = std::make_unique<Library>();

        GET_FUNCTION(sony_openxrSetAppWindowHandle);
        GET_FUNCTION(sony_openxrUpdateSpacePose);
        GET_FUNCTION(sony_openxrSetViewSpaceScale);
#undef GET_FUNCTION

        return lib;
    }

    void UnlinkXrLibraryWin64() {
        if (g_dll) {
            FreeLibrary(g_dll);
        }
    }

}  // namespace sony::openxr::extension
