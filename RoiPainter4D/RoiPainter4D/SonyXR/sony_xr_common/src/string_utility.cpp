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


#include "string_utility.h"

#include <xr-runtime-common/xr_windows.h>

namespace sony::oz::xr_runtime::utility {
std::wstring StringToWString(std::string oString) {
  int buffer_size =
      MultiByteToWideChar(CP_OEMCP, 0, oString.c_str(), -1, (wchar_t*)NULL, 0);
  LPWSTR cpWideByte = new WCHAR[buffer_size];
  MultiByteToWideChar(CP_OEMCP, 0, oString.c_str(), -1, cpWideByte,
                      buffer_size);

  std::wstring out{cpWideByte, cpWideByte + buffer_size - 1};

  delete[] cpWideByte;

  return out;
}

std::string WStringToString(std::wstring oWString) {
  int buffer_size = WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str(), -1,
                                        (char*)NULL, 0, NULL, NULL);
  LPSTR cpMultiByte = new CHAR[buffer_size];
  WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str(), -1, cpMultiByte,
                      buffer_size, NULL, NULL);

  std::string out{cpMultiByte, cpMultiByte + buffer_size - 1};

  delete[] cpMultiByte;

  return out;
}
}  // namespace sony::oz::xr_runtime::utility
