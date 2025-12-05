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
#include <stdint.h>

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#else
#include <windows.h>
#endif

namespace sony::oz::xr_runtime {
  /**
   * @brief 渡した文字列の配列をコンボボックスに格納したダイアログを表示する
   * @param[in] platform_id 利用するプラットフォームのIDを指定する
   * @param[in] hWnd 親となるウィンドウを指定する
   * @param[in] item_list コンボボックスに格納したい文字列のポインタの配列を指定する
   * @param[in] size リストのサイズを格納する
   * return ダイアログで選択された選択肢のインデックス
  */
  int64_t ShowComboBoxDialog(SonyOzPlatformId platform_id, HWND hWnd, const wchar_t* item_list[], int32_t size);

  struct supported_panel_spec {
    char device_name[16]; // max 15 characters
    float width;    // in meter
    float height;   // in meter
    float angle;    // in radian
  };

  enum class SupportDevice {
    ELF_SR1 = 0,
    ELF_SR2 = 1
  };

  /**
   * @brief 機種ごとのパネルのサイズを取得する
   * @param[in] platform_id 利用するプラットフォームのIDを指定する
   * @param[in,out] panel_spec 機種ごとのパネルのスペックの情報(リスト)が返される / nullptr
   * @param[in,out] size 渡すリストのサイズ / 返される情報のリストのサイズ
   * return 取得できたか
  */
  bool GetPanelSpecOfSupportedDevices(SonyOzPlatformId platform_id, supported_panel_spec* panel_spec, int32_t* size);
}
