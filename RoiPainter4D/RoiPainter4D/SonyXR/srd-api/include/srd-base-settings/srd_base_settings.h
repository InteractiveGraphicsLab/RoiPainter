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
#ifndef SRD_BASE_SETTINGS_H_
#define SRD_BASE_SETTINGS_H_

namespace sony::oz::srd_base_settings {
  struct SrdXrCameraSettings {
    float gain;
    bool auto_exposure_enabled;

    SrdXrCameraSettings()
        : gain(1.f)
        , auto_exposure_enabled(true)
    {}
  };

  enum class SrdXrCrosstalkCorrectionMode {
    DISABLED = 0,
    DEPENDS_ON_APPLICATION = 1,
    GRADATION_CORRECTION_MEDIUM = 2,
    GRADATION_CORRECTION_ALL = 3,
    GRADATION_CORRECTION_HIGH_PRECISE = 4,
  };

  struct SrdXrCrosstalkCorrectionSystemSettings {
    SrdXrCrosstalkCorrectionMode crosstalk_compensation_mode;
    float gamma;
    float alpha;
    float limit_l;
    float limit_h;

    SrdXrCrosstalkCorrectionSystemSettings(SrdXrCrosstalkCorrectionMode in_crosstalk_compensation_mode, float in_gamma, float in_alpha, float in_limit_l, float in_limit_h)
        : crosstalk_compensation_mode(in_crosstalk_compensation_mode)
        , gamma(in_gamma)
        , alpha(in_alpha)
        , limit_l(in_limit_l)
        , limit_h(in_limit_h)
    {}

    SrdXrCrosstalkCorrectionSystemSettings()
        : crosstalk_compensation_mode(SrdXrCrosstalkCorrectionMode::DEPENDS_ON_APPLICATION)
        , gamma(0.f)
        , alpha(0.f)
        , limit_l(0.f)
        , limit_h(0.f)
    {}
  };

  enum class SrdXrFaceTrackingMode {
    NO_CHANGE = 0,
    SIZE_PRIORITY = 1,
    DISTANCE_PRIORITY = 2,
  };

  struct SrdBaseSettings
  {
    int id;
    SrdXrCameraSettings cameraSettings;
    bool autoExposureEnabled;
    float cameraGain;
    SrdXrCrosstalkCorrectionSystemSettings crosstalkCorrectionSystemSettings;
    SrdXrFaceTrackingMode trackingMode;

    SrdBaseSettings()
        : id(0)
        , cameraSettings(SrdXrCameraSettings())
        , autoExposureEnabled(false)
        , cameraGain(0)
        , crosstalkCorrectionSystemSettings(SrdXrCrosstalkCorrectionSystemSettings())
        , trackingMode(SrdXrFaceTrackingMode::NO_CHANGE)
    {}
  };
}
#endif