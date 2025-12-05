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
#ifndef REGISTRY_UTILITY_H_
#define REGISTRY_UTILITY_H_

#include <xr-runtime-common/xr_api_defs.h>
#include <xr-runtime-common/xr_windows.h>

#include <string>

namespace sony::oz::xr_runtime::utility {
std::wstring GetRegistryString(HKEY hkey, const std::wstring& sub_key,
                               const std::wstring& value);
void SetProductId(const char* product_id);
std::string GetProductId();
std::wstring GetProductIdWString();
void SetPlatformId(SonyOzPlatformId platform_id);
std::string GetPlatformId();
std::wstring GetPlatformIdW();
std::wstring GetRegistryPathW();
std::wstring GetRegistryPathW(SonyOzPlatformId platform_id);
std::string GetRegistryPath();
std::string GetRegistryPath(SonyOzPlatformId platform_id);
}  // namespace sony::oz::xr_runtime::utility

#endif  // REGISTRY_UTILITY_H_
