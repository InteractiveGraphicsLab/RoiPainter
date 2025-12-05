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


#include "registry_utility.h"

#include "string_utility.h"

namespace {
std::string product_id_;

std::string platform_id_;
}  // namespace

namespace sony::oz::xr_runtime::utility {
std::wstring GetRegistryString(HKEY hkey, const std::wstring& sub_key,
                               const std::wstring& value) {
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

/**
 * Product ID
 */
void SetProductId(const char* product_id) {
  product_id_ = std::string(product_id);
}

std::string GetProductId() { return product_id_; }

std::wstring GetProductIdWString() { return StringToWString(product_id_); }

/**
 * Platform ID
 */
void SetPlatformId(SonyOzPlatformId platform_id) { platform_id_ = platform_id; }

std::string GetPlatformId() { return platform_id_; }

std::wstring GetPlatformIdW() { return StringToWString(platform_id_); }

/**
 * Registry Path
 */
std::wstring GetRegistryPathW() {
  std::wstring registry_path =
      std::wstring(L"SOFTWARE\\Sony Corporation\\") + GetPlatformIdW();
  return GetRegistryString(HKEY_LOCAL_MACHINE, registry_path, L"Path");
}

std::wstring GetRegistryPathW(SonyOzPlatformId platform_id) {
  std::wstring registry_path = std::wstring(L"SOFTWARE\\Sony Corporation\\") +
                               StringToWString(platform_id);
  return GetRegistryString(HKEY_LOCAL_MACHINE, registry_path, L"Path");
}

std::string GetRegistryPath() {
  std::wstring registry_path =
      std::wstring(L"SOFTWARE\\Sony Corporation\\") + GetPlatformIdW();
  return WStringToString(
      GetRegistryString(HKEY_LOCAL_MACHINE, registry_path, L"Path"));
}

std::string GetRegistryPath(SonyOzPlatformId platform_id) {
  std::wstring registry_path = std::wstring(L"SOFTWARE\\Sony Corporation\\") +
                               StringToWString(platform_id);
  return WStringToString(
      GetRegistryString(HKEY_LOCAL_MACHINE, registry_path, L"Path"));
}
}  // namespace sony::oz::xr_runtime::utility
