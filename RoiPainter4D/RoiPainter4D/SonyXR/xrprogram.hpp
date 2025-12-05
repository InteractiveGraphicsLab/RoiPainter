#pragma once



// originally from Sony native sdk
// modified by Takashi for C++ CLI

// memo 
// - C++/CLI のプロジェクトに利用する
// - 依存関係
//   = xrprogram.hpp/xrprogram.cpp --> includeフォルダに追加、プロジェクトに直接追加
//   = openXR --> includeフォルダ追加/lib追加/dll配置
//   = srd-api    --> includeフォルダ追加/srcを直接プロジェクトに追加
//   = srd-openxr --> includeフォルダ追加/srcを直接プロジェクトに追加
//   = xr-runtime --> includeフォルダ追加/srcを直接プロジェクトに追加
//   = sony_xr_common --> includeフォルダ追加/srcを直接プロジェクトに追加
//  
// - MainFormに追加するもの
// 	System::Windows::Forms::Timer^ xrTimer;
//  XrProgram* xr_program = nullptr;
//  void CloseXrProgram(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e);
//  void XrTimerTick(System::Object^ sender, System::EventArgs^ e);
//    - XrTimerTichを高頻度で起動して、XrProgram::XrMainFunction()を呼び出す
// 
//  VerlayFormを初期化時に追加 : MouseEventを取得する


// カプセル化が不十分だったので、修正した


#ifndef XRSESSION_HPP
#define XRSESSION_HPP

#define XR_USE_GRAPHICS_API_OPENGL
#define XR_USE_PLATFORM_WIN32

#pragma unmanaged
#include <windows.h>
#undef min
#undef max


#include "GL/glew.h"

#include <string>
#include <fstream>
#include <vector>

#include <openxr/openxr.h>
#include <openxr/openxr_platform_defines.h>
#include <openxr/openxr_platform.h>
#include <openxr/xr_linear.h>

#include <srd_openxr_api_wrapper.h>
#include <srd_openxr_library_loader.h>
#include <misc_util_wrapper.h>
#include <xr_api_wrapper.h>


#define VIEW_SPACE_SCALE 15.0f

class XrProgram
{
private:
	char application_name[XR_MAX_APPLICATION_NAME_SIZE];
	std::vector<char*> required_extensions{ (char*)XR_KHR_OPENGL_ENABLE_EXTENSION_NAME };
	std::vector<char*> optional_extensions{ (char*)XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME };
	std::vector<char*> enabled_extensions;

	//added by Takashi 
	HGLRC m_hglrc;
	HDC   m_hdc;
	float m_near_z;
	float m_far_z;
	bool  m_xr_shutdown = false;

	XrInstance m_instance;
	XrSession  m_session;
	XrGraphicsBindingOpenGLWin32KHR m_graphics_binding;
	XrSpace m_reference_space;
	XrSystemId m_system_id;
	XrViewConfigurationType m_view_type = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	XrSessionState m_state = XR_SESSION_STATE_UNKNOWN;
	float m_scale_factor;


	// An array of arrays of framebuffers, one per texture/XR image
	std::vector<std::vector<GLuint>> m_framebuffers;

	//The preffered swapchain format
	int64_t m_swapchain_format;

	//The preffered swapchain format
	int64_t m_depth_swapchain_format;

	//An array of arrays of images for each projection swapchain
	std::vector<std::vector<XrSwapchainImageOpenGLKHR>> m_images;

	//An array of arrays of images for depth info swapchain
	std::vector<std::vector<XrSwapchainImageOpenGLKHR>> m_depth_images;

	//an array of swapchains for the projection views, 1 swapchain per view
	std::vector<XrSwapchain> m_swapchains;

	//an array of swapchains for the projectionViews Depth attachment
	std::vector<XrSwapchain> m_depth_swapchains;

	//An array of projection views, used for rendering to each eye
	std::vector<XrCompositionLayerProjectionView>	m_projection_views;

	//An array of config views, one for each eye
	std::vector<XrViewConfigurationView> m_xr_config_views;

	struct {
		bool supported = false;
		std::vector<XrCompositionLayerDepthInfoKHR> depth_info;
	} depth;


public:
	XrProgram(const char* application_name, HGLRC hglrc, HDC hdc);
	bool init();
	void destroy();
	bool XrMainFunction();

private:

	bool createInstance();
	bool createSession();
	bool createReferenceSpace();
	bool checkExtensionSupport();
	bool createViews();
	bool checkViewConfigs(XrViewConfigurationType view_type);
	bool createSwapchains(int view_count);
	bool checkSwapchainImageSupport(int64_t format);

	//Populate the images array with the swapchain images;
	bool getSwapchainImages();
	bool genFrameBuffers();
	bool genFrameBuffers(int _view_count);
	bool beginSession();
	bool checkXrResult(XrResult);
	bool checkEvents();

	bool renderFrame_SidebySide(int width, int height, 
			XrMatrix4x4f perspective_matrix_L, XrMatrix4x4f	 perspective_matrix_R, 
			XrMatrix4x4f view_matrix_L, XrMatrix4x4f view_matrix_R, 
			GLuint framebuffer, GLuint depthbuffer, XrSwapchainImageOpenGLKHR image, 
			XrTime predicted_time);

	//sony runtime
	// sony::oz and sony::openxr
	float GetPanelSizeScale(bool srdManager, sony::oz::xr_runtime::SupportDevice ConnectedDevice_);
	uint32_t GetDeviceIndex(const SonyOzDeviceInfo* DeviceList, const uint64_t Size);
	sony::oz::xr_runtime::SupportDevice ConnectedDevice = sony::oz::xr_runtime::SupportDevice::ELF_SR1;

	//削除
	//bool renderFrame(int width, int height, XrMatrix4x4f perspective_matrix, XrMatrix4x4f view_matrix, GLuint framebuffer, GLuint depthbuffer, XrSwapchainImageOpenGLKHR image, XrTime predicted_time);

};

#pragma managed

#endif 