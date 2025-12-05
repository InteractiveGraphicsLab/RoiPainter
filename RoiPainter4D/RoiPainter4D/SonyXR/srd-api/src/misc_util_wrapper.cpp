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

#include "misc_util_wrapper.h"

#include "xr-runtime-common/xr_windows.h"

namespace misc::util {
  using showComboBoxDialog_t = int64_t (__stdcall*)(HWND hWnd, const wchar_t* item_list[], int32_t size);
  using getPanelSpecOfSupportedDevices_t = bool(__stdcall*)(sony::oz::xr_runtime::supported_panel_spec* panel_spec, int32_t* size);

  showComboBoxDialog_t showComboBoxDialog = nullptr;
  getPanelSpecOfSupportedDevices_t getPanelSpecOfSupportedDevices = nullptr;

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
    std::wstring xrmw_path = directry + L"lib\\misc_util.dll";
    handle = ::LoadLibraryW(xrmw_path.c_str());

    if (handle == nullptr) {
      return false;
    }

    #define GET_FUNCTION(name)                        \
    name = (name##_t)GetProcAddress(handle, #name); \
    if (!name) {                                    \
      return false;                                 \
    }

    GET_FUNCTION(showComboBoxDialog);
    GET_FUNCTION(getPanelSpecOfSupportedDevices);

    return true;
  }
}

namespace sony::oz::xr_runtime {
  int64_t ShowComboBoxDialog(SonyOzPlatformId platform_id, HWND hWnd, const wchar_t* item_list[], int32_t size) {
    if (misc::util::LoadXrRuntimeLibrary() == false) {
      return -1;
    }

    if (misc::util::showComboBoxDialog == nullptr) {
      return -1;
    }

    return misc::util::showComboBoxDialog(hWnd, item_list, size);
  }

  bool GetPanelSpecOfSupportedDevices(SonyOzPlatformId platform_id, supported_panel_spec* panel_spec, int32_t* size) {
    if (misc::util::LoadXrRuntimeLibrary() == false) {
      return false;
    }

    if (misc::util::getPanelSpecOfSupportedDevices == nullptr) {
      return false;
    }

    return misc::util::getPanelSpecOfSupportedDevices(panel_spec, size);
  }
}
