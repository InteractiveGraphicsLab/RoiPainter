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


#include "xr_api_wrapper_utility.h"

#include <xr-runtime-common/xr_windows.h>

#include <chrono>

#include "xr_api_wrapper.h"

namespace sony::oz::xr_runtime::utility {
bool WaitUntilRunningState(SonyOzSessionHandle handle) {
  using namespace std::chrono;

  constexpr uint32_t timeout = 5000;

  milliseconds elapsed_time;
  auto begin = steady_clock::now();
  do {
    SonyOzSessionState state;
    if (sony::oz::xr_runtime::GetSessionState(handle, &state) !=
        SonyOzResult::SUCCESS) {
      return false;
    }

    if (state == SonyOzSessionState::RUNNING) {
      return true;
    }

    auto end = steady_clock::now();
    elapsed_time = duration_cast<milliseconds>(end - begin);

  } while (elapsed_time.count() < timeout);

  return false;
}
}  // namespace sony::oz::xr_runtime::utility
