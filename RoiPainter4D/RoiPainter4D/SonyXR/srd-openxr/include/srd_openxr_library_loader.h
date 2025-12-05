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
#pragma once

#include <memory>

//#include "OpenXRCore.h"

#include "xr-runtime-common/xr_api_defs.h"
#include "openxr/openxr.h"
namespace sony::openxr::extension {

    using sony_openxrSetAppWindowHandle_t = void(*) (XrSession, void*);
    using sony_openxrUpdateSpacePose_t = void(*) (XrSession, void*, void*);
    using sony_openxrSetViewSpaceScale_t = void(*)(XrSession, float);

    struct Library {
        sony_openxrSetAppWindowHandle_t sony_openxrSetAppWindowHandle;
        sony_openxrUpdateSpacePose_t sony_openxrUpdateSpacePose;
        sony_openxrSetViewSpaceScale_t sony_openxrSetViewSpaceScale;
    };

    std::unique_ptr<Library> LinkXrLibraryWin64(SonyOzPlatformId platform_id);
    void UnlinkXrLibraryWin64();

}  // namespace sony::openxr::extension
