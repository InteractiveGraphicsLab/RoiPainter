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
#pragma once

#include "xr-runtime-common/xr_api_defs.h"
#include "srd-base-settings/srd_base_settings.h"

XR_DEFINE_HANDLE(SonyOzSessionHandle);

namespace sony::oz::xr_runtime {
  SonyOzResult SetCameraWindowEnabled(SonyOzPlatformId platform_id, SonyOzSessionHandle session, const bool enable);
  SonyOzResult GetCrosstalkCorrectionSettings(SonyOzPlatformId platform_id, SonyOzSessionHandle session, sony::oz::srd_base_settings::SrdXrCrosstalkCorrectionMode* mode);
  SonyOzResult SetCrosstalkCorrectionSettings(SonyOzPlatformId platform_id, SonyOzSessionHandle session, const sony::oz::srd_base_settings::SrdXrCrosstalkCorrectionMode mode);

  SonyOzResult GetStubHeadPose(SonyOzSessionHandle session, SonyOzPosef* pose);
  SonyOzResult SetStubHeadPose(SonyOzSessionHandle session, const SonyOzPosef pose);
}
