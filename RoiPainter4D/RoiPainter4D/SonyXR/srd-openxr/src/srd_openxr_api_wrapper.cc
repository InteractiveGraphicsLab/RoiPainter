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

#include "srd_openxr_api_wrapper.h"

#include "srd_openxr_library_loader.h"

namespace sony::openxr::extension {
    static std::unique_ptr<Library> g_library;

    bool LinkXrLibrary() {
        static const SonyOzPlatformId platform_id_ = "Spatial Reality Display";
#ifdef _WIN64
        if (!g_library) {
            g_library = LinkXrLibraryWin64(platform_id_);

            if (!g_library) {
                return false;
            }
        }
        return true;
#else   // _WIN64
        // Win64 is the only platfrom currently supported.
        return false;
#endif  // _WIN64
        }

    void UnlinkXrLibrary() {
#ifdef _WIN64
        UnlinkXrLibraryWin64();
        g_library.reset();
#endif  // _WIN64
    }

    void SetAppWindowHandle(XrSession Session, void* WindowHandle)
    {
        if (g_library && g_library->sony_openxrSetAppWindowHandle) {
            g_library->sony_openxrSetAppWindowHandle(Session, WindowHandle);
        }
    }

    void UpdateSpacePose(XrSession Session, void* Space, void* Pose)
    {
        if (g_library && g_library->sony_openxrUpdateSpacePose) {
            g_library->sony_openxrUpdateSpacePose(Session, Space, Pose);
        }
    }

    void SetViewSpaceScale(XrSession Session, float ViewSpaceScale)
    {
        if (g_library && g_library->sony_openxrSetViewSpaceScale) {
            return g_library->sony_openxrSetViewSpaceScale(Session, ViewSpaceScale);
        }
    }
    }  // namespace sony::openxr::extension
