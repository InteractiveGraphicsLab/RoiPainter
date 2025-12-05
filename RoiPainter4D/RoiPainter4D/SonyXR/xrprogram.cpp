#include "xrprogram.hpp"
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL


#include <iostream>
#include "../ModeCore.h"
#pragma unmanaged
#pragma comment(lib, "openxr_loader.lib")
#pragma comment(lib, "Advapi32.lib")




static const SonyOzPlatformId k_PlatformId = "Spatial Reality Display";


XrProgram::XrProgram(const char* application_name, HGLRC hglrc, HDC hdc)
{
	strcpy_s(this->application_name, XR_MAX_APPLICATION_NAME_SIZE, application_name);

	m_near_z = 0.01f;
	m_far_z = 100.f;

	//added by Takashi
	m_hglrc = hglrc;
	m_hdc = hdc;
	return;
}


bool XrProgram::init() 
{
	if (!this->createInstance()) 
		return false;

	if (!this->createSession())
		return false;

	if (!this->createReferenceSpace())
		return false;

	if (!this->createViews()) 
		return false;

	if (!this->beginSession()) 
		return false;

	return true;
}

bool XrProgram::createSession() 
{
	//Get system ID of Head mounted Display (We don't use Phone Screens)
	XrSystemGetInfo system_get_info;
	system_get_info.type = XR_TYPE_SYSTEM_GET_INFO;
	system_get_info.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	system_get_info.next = XR_NULL_HANDLE;

	XrSystemId system_id;
	if (!checkXrResult(xrGetSystem(this->m_instance, &system_get_info, &system_id)))
	{
		printf("Unable to get System ID\n");
		return false;
	}

	this->m_system_id = system_id;

	//Check Graphics Requirements (Required for openGL)
	XrGraphicsRequirementsOpenGLKHR graphics_requirements;
	graphics_requirements.type = XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR;

	PFN_xrGetOpenGLGraphicsRequirementsKHR pfnGetOpenGLGraphicsRequirements = NULL;
	if (!checkXrResult(xrGetInstanceProcAddr(this->m_instance, "xrGetOpenGLGraphicsRequirementsKHR", (PFN_xrVoidFunction*)&pfnGetOpenGLGraphicsRequirements)))
	{
		printf("Unable to Get GL Requirements function pointer\n");
		return false;
	}

	if (!checkXrResult(pfnGetOpenGLGraphicsRequirements(this->m_instance, system_id, &graphics_requirements)))
	{
		printf("Unable to get OpenGL Graphics Requirements\n");
		return false;
	}

	// Create graphics binding using GLFW window
	
	this->m_graphics_binding.type = XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR;
	this->m_graphics_binding.next = nullptr;
	
  // modified by Takashi
	this->m_graphics_binding.hGLRC = m_hglrc;
	this->m_graphics_binding.hDC = m_hdc;

	//Create Session
	XrSessionCreateInfo session_create_info;
	session_create_info.type = XR_TYPE_SESSION_CREATE_INFO;
	session_create_info.createFlags = 0;
	session_create_info.next = &this->m_graphics_binding;
	session_create_info.systemId = system_id;

	if (!checkXrResult(xrCreateSession(this->m_instance, &session_create_info, &this->m_session))) 
	{
		printf("Unable to create XR session\n");
		return false;
	}
	// Using Sony::oz namespace here to get the panel scale factor
	uint64_t Size = 0;
	sony::oz::xr_runtime::LinkXrLibrary(k_PlatformId);
	sony::oz::xr_runtime::GetDeviceNum(k_PlatformId, &Size);

	auto DeviceList = std::make_unique<SonyOzDeviceInfo[]>(Size);
	sony::oz::xr_runtime::EnumerateDevices(k_PlatformId, Size, DeviceList.get());

	uint32_t DeviceIndex = GetDeviceIndex(DeviceList.get(), Size);
	if (DeviceIndex == -1) 
	{
		std::cout << "There is no SRD device found\n";
    return false;
	}

	m_scale_factor = GetPanelSizeScale(true, ConnectedDevice);

	return true;
}


bool XrProgram::createInstance() 
{
	//populate Application info struct

	XrApplicationInfo appInfo{};
	appInfo.apiVersion = XR_CURRENT_API_VERSION;
	appInfo.engineVersion = 2019;
	strcpy_s(appInfo.applicationName, XR_MAX_APPLICATION_NAME_SIZE, this->application_name);
	strcpy_s(appInfo.engineName, XR_MAX_APPLICATION_NAME_SIZE, "Visual Studio");
	appInfo.applicationVersion = 1;

	if (!this->checkExtensionSupport()) 
	{
		return false;
	}

	//Instance Create Info Structure
	XrInstanceCreateInfo instance_create_info;
	instance_create_info.type = XR_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.next = XR_NULL_HANDLE;
	instance_create_info.applicationInfo = appInfo;
	instance_create_info.enabledApiLayerCount = 0;
	instance_create_info.enabledApiLayerNames = NULL;
	instance_create_info.enabledExtensionCount = static_cast<uint32_t>(this->enabled_extensions.size());
	instance_create_info.enabledExtensionNames = this->enabled_extensions.data();
	instance_create_info.createFlags = NULL;
	if (!checkXrResult(xrCreateInstance(&instance_create_info, &this->m_instance))) 
	{
		return false;
	}
	return true;
}


//Loop through required Extensions and if one is missing return false
bool XrProgram::checkExtensionSupport() 
{
	uint32_t ext_count = 0;
	if (!checkXrResult(xrEnumerateInstanceExtensionProperties(NULL, 0, &ext_count, NULL))) 
	{
		return false;
	}

	std::vector<XrExtensionProperties> extensions(ext_count, { XR_TYPE_EXTENSION_PROPERTIES, nullptr });

	
	if (!checkXrResult(xrEnumerateInstanceExtensionProperties(NULL, ext_count, &ext_count, extensions.data()))) 
	{
		return false;
	}

	for (uint32_t i = 0; i < ext_count; i++) 
	{
		XrExtensionProperties properties = extensions[i];
		int match = 0;

		for (const char* extension : this->required_extensions) 
		{
			printf("%s\n", properties.extensionName);
			if (!strcmp(properties.extensionName, extension))
			{
				match = 1;
				this->enabled_extensions.push_back(this->required_extensions[0]);
			}
		}

		if (!strcmp(this->optional_extensions[0], properties.extensionName))
		{
			this->depth.supported = true;
			this->enabled_extensions.push_back(this->optional_extensions[0]);
		}
	}
	return true;
}


bool XrProgram::createReferenceSpace() 
{
	const XrPosef  xr_pose_identity = { {0,0,0,1}, {0,0,0} };
	XrReferenceSpaceCreateInfo space_create_info;
	space_create_info.type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO;
	space_create_info.next = NULL;
	space_create_info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
	space_create_info.poseInReferenceSpace = xr_pose_identity;

	if (!checkXrResult(xrCreateReferenceSpace(this->m_session, &space_create_info, &this->m_reference_space))) 
	{
		return false;
	}
	return true;
}


bool XrProgram::createViews() 
{
	if (!checkViewConfigs(this->m_view_type))
	{
		return false;
	}

	uint32_t view_count = 0;

	if(!checkXrResult(xrEnumerateViewConfigurationViews(this->m_instance, this->m_system_id, this->m_view_type, view_count, &view_count, NULL))) 
	{
		return false;
	}
	this->m_xr_config_views.resize(view_count, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
	
	if (!checkXrResult(xrEnumerateViewConfigurationViews(this->m_instance, this->m_system_id, this->m_view_type, view_count, &view_count, this->m_xr_config_views.data())))
	{
		printf("Unable to get View config views\n");
		return false;
	}
	std::cout << "view_count:" << view_count << std::endl;

	// SRD's OpenXR runtime currently only supports side by side drawing for a single Swapchain.
	// Build a Swapchain with double the width instead of the recommended Swapchain size passed
	// in the xrEnumerateViewConfigurationViews ().
	if (2 <= view_count) {
		if (m_xr_config_views[0].recommendedImageRectWidth == m_xr_config_views[1].recommendedImageRectWidth) {
			m_xr_config_views[0].recommendedImageRectWidth *= 2;
			m_xr_config_views[1].recommendedImageRectWidth *= 2;
		}
	}
	//if (!createSwapchains(view_count)) 
	//{
	//	printf("Unable to create Swapchain\n");
	//	return false;
	//}
	if (!createSwapchains(1))
	{
		printf("Unable to create Swapchain\n");
		return false;
	}
	
	//this->images.resize(view_count);
	//if (this->depth.supported) 
	//{
	//	this->depth_images.resize(view_count);
	//}
	this->m_images.resize(1);
	if (this->depth.supported)
	{
		this->m_depth_images.resize(1);
	}

	if (!getSwapchainImages()) 
	{
		printf("Unable to populate swapchain images\n");
		return false;
	}

	//// Right now this is always true
	//if (!genFrameBuffers()) 
	//{
	//	return false;
	//}
	// Right now this is always true
	if (!genFrameBuffers(1))
	{
		return false;
	}

	this->m_projection_views.resize(view_count);

	//Allocate Composition Layer Projection View. Everything can be filled in except FOV and Pose
	for (uint32_t i = 0; i < view_count; i++) 
	{
		this->m_projection_views[i].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
		this->m_projection_views[i].next = NULL;
		//this->projection_views[i].subImage.swapchain = this->swapchains[i];
		this->m_projection_views[i].subImage.swapchain = this->m_swapchains[0];
		this->m_projection_views[i].subImage.imageArrayIndex = 0;
		this->m_projection_views[i].subImage.imageRect.offset.x = 0;
		this->m_projection_views[i].subImage.imageRect.offset.y = 0;
		this->m_projection_views[i].subImage.imageRect.extent.height = this->m_xr_config_views[i].recommendedImageRectHeight;
		this->m_projection_views[i].subImage.imageRect.extent.width = this->m_xr_config_views[i].recommendedImageRectWidth;
	}

	if (this->depth.supported)
	{
		this->depth.depth_info.resize(view_count);
		for (uint32_t i = 0; i < view_count; i++) 
		{
			this->depth.depth_info[i].type = XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR;
			//this->depth.depth_info[i].subImage.swapchain = this->depth_swapchains[i];
			this->depth.depth_info[i].subImage.swapchain = this->m_depth_swapchains[0];
			this->depth.depth_info[i].subImage.imageArrayIndex = 0;
			this->depth.depth_info[i].subImage.imageRect.offset.x = 0;
			this->depth.depth_info[i].subImage.imageRect.offset.y = 0;
			this->depth.depth_info[i].subImage.imageRect.extent.height = this->m_xr_config_views[i].recommendedImageRectHeight;
			this->depth.depth_info[i].subImage.imageRect.extent.width = this->m_xr_config_views[i].recommendedImageRectWidth;
			//this->projection_views[i].next = &this->depth.depth_info[i];
		}
		
	}

	return true;
}



bool XrProgram::checkViewConfigs(XrViewConfigurationType view_type)
{
	uint32_t config_count = 0;

	if (!checkXrResult(xrEnumerateViewConfigurations(this->m_instance, this->m_system_id, config_count, &config_count, NULL)))
	{
		printf("Unable to get View Configs\n");
		return false;
	}

	std::vector<XrViewConfigurationType> view_configs(config_count);

	if (!checkXrResult(xrEnumerateViewConfigurations(this->m_instance, this->m_system_id, config_count, &config_count, view_configs.data())))
	{
		printf("Unable to get View Configs data\n");
		return false;
	}

	bool configFound = false;
	for (uint32_t i = 0; i < config_count; ++i)
	{
		if (view_configs[i] == view_type)
		{
			configFound = true;
			break;  // Pick the first supported, i.e. preferred, view configuration.
		}
	}

	// Cannot support any view configuration of this system.
	if (!configFound)
	{
		printf("View Config not supported\n");
		return false;   
	}

	return true;
}


bool XrProgram::createSwapchains(int view_count) 
{
	int64_t preffered_format = GL_SRGB8_ALPHA8;
	if (!checkSwapchainImageSupport(preffered_format)) 
	{
		printf("Preferred Swapchain format %s not supported\n", "GL_SRGB8_APLHA8");
	}

	this->m_swapchains.resize(view_count);

	for (int i = 0; i < view_count; i++) 
	{
		XrSwapchainCreateInfo swapchain_create_info;
		swapchain_create_info.type = XR_TYPE_SWAPCHAIN_CREATE_INFO;
		swapchain_create_info.next = NULL;
		swapchain_create_info.createFlags = 0;
		swapchain_create_info.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT;
		swapchain_create_info.format = this->m_swapchain_format;
		swapchain_create_info.sampleCount = this->m_xr_config_views[i].recommendedSwapchainSampleCount;
		swapchain_create_info.width = this->m_xr_config_views[i].recommendedImageRectWidth;
		swapchain_create_info.height = this->m_xr_config_views[i].recommendedImageRectHeight;
		swapchain_create_info.faceCount = 1;
		swapchain_create_info.arraySize = 1;
		swapchain_create_info.mipCount = 1;

		if (!checkXrResult(xrCreateSwapchain(this->m_session, &swapchain_create_info, &this->m_swapchains[i]))) 
		{
			printf("Unable to create Swapchain for view %d\n", i);
			return false;
		}
	}

	if (this->m_depth_swapchain_format != -1 && this->depth.supported) 
	{
		this->m_depth_swapchains.resize(view_count);
		for (int i = 0; i < view_count; i++)
		{
			XrSwapchainCreateInfo depth_swapchain_create_info;
			depth_swapchain_create_info.type = XR_TYPE_SWAPCHAIN_CREATE_INFO;
			depth_swapchain_create_info.next = NULL;
			depth_swapchain_create_info.createFlags = 0;
			depth_swapchain_create_info.usageFlags = XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			depth_swapchain_create_info.format = this->m_depth_swapchain_format;
			depth_swapchain_create_info.sampleCount = this->m_xr_config_views[i].recommendedSwapchainSampleCount;
			depth_swapchain_create_info.width = this->m_xr_config_views[i].recommendedImageRectWidth;
			depth_swapchain_create_info.height = this->m_xr_config_views[i].recommendedImageRectHeight;
			depth_swapchain_create_info.faceCount = 1;
			depth_swapchain_create_info.arraySize = 1;
			depth_swapchain_create_info.mipCount = 1;
			if (!checkXrResult(xrCreateSwapchain(this->m_session, &depth_swapchain_create_info, &this->m_depth_swapchains[i])))
			{
				printf("Unable to create Depth Swapchain for view %d\n", i);
				return false;
			}
		}
	}

	return true;
}


bool XrProgram::checkSwapchainImageSupport(int64_t image_format)
{
	uint32_t format_count = 0;
	if (!checkXrResult(xrEnumerateSwapchainFormats(this->m_session, format_count, &format_count, NULL))) 
	{
		printf("Couldn't get Swapchain image formats\n");
		return false;
	}
	std::vector<int64_t> swapchain_image_formats(format_count);
	if (!checkXrResult(xrEnumerateSwapchainFormats(this->m_session, format_count, &format_count, swapchain_image_formats.data())))
	{
		printf("Couldn't get Swapchain image formats\n");
		return false;
	}
	bool preferred = false;
	bool depth_preferred = false;

	//Temporary Depth preferred format
	int64_t depth_pref_format = GL_DEPTH_COMPONENT32F;

	this->m_swapchain_format = swapchain_image_formats[0];
	this->m_depth_swapchain_format = -1;
	for (int i = 0; i < swapchain_image_formats.size(); i++) 
	{
		if (swapchain_image_formats[i] == image_format) 
		{
			this->m_swapchain_format = image_format;
			preferred = true;
			printf("Prefered format found\n");
		}

		if (swapchain_image_formats[i] == depth_pref_format) 
		{
			this->m_depth_swapchain_format = depth_pref_format;
			depth_preferred = true;;
			printf("Preferred depth format found\n");
		}
	}
	if (!preferred) 
	{
		return false;
	}
	if (depth_preferred) 
	{
		this->depth.supported = true;
	}
	return true;
}


bool XrProgram::getSwapchainImages() 
{
	for (int i = 0; i < this->m_swapchains.size(); i++) 
	{
		uint32_t image_count = 0;
		if (!checkXrResult(xrEnumerateSwapchainImages(this->m_swapchains[i], image_count, &image_count, NULL))) 
		{
			return false;
		}
		this->m_images[i].resize(image_count, { XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR, nullptr});

		if (!checkXrResult(xrEnumerateSwapchainImages(this->m_swapchains[i], image_count, &image_count, (XrSwapchainImageBaseHeader*)this->m_images[i].data())))
		{
			return false;
		}
	}
	if (this->depth.supported) {
		for (int i = 0; i < this->m_depth_swapchains.size(); i++)
		{
			uint32_t image_count = 0;
			if (!checkXrResult(xrEnumerateSwapchainImages(this->m_depth_swapchains[i], image_count, &image_count, NULL)))
			{
				return false;
			}
			this->m_depth_images[i].resize(image_count, { XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR, nullptr });

			if (!checkXrResult(xrEnumerateSwapchainImages(this->m_depth_swapchains[i], image_count, &image_count, (XrSwapchainImageBaseHeader*)this->m_depth_images[i].data())))
			{
				return false;
			}
		}
	}
	return true;
}


bool XrProgram::genFrameBuffers() 
{
	int view_count = static_cast<int>(this->m_xr_config_views.size());
	this->m_framebuffers.resize(view_count);
	for (int i = 0; i < view_count; i++) 
	{
		this->m_framebuffers[i].resize(this->m_images[i].size());
		glGenFramebuffers(static_cast<GLsizei>(this->m_framebuffers[i].size()), this->m_framebuffers[i].data());
	}
	return true;
}

bool XrProgram::genFrameBuffers(int _view_count)
{
	int view_count = _view_count;
	this->m_framebuffers.resize(_view_count);
	for (int i = 0; i < view_count; i++)
	{
		this->m_framebuffers[i].resize(this->m_images[i].size());
		glGenFramebuffers(static_cast<GLsizei>(this->m_framebuffers[i].size()), this->m_framebuffers[i].data());
	}
	return true;
}

bool XrProgram::beginSession() 
{
	//sony runtime
	sony::openxr::extension::LinkXrLibrary();

	XrSessionBeginInfo session_begin_info;
	session_begin_info.type = XR_TYPE_SESSION_BEGIN_INFO;
	session_begin_info.primaryViewConfigurationType = this->m_view_type;
	if (!checkXrResult(xrBeginSession(this->m_session, &session_begin_info)))
	{
		printf("Unable to begin session\n");
		return false;
	}
	return true;
}

float XrProgram::GetPanelSizeScale(bool ScalingMode, sony::oz::xr_runtime::SupportDevice ConnectedDevice_)
{
	if (ScalingMode == true) {
		int32_t Size = 0;
		if (!sony::oz::xr_runtime::GetPanelSpecOfSupportedDevices(k_PlatformId, nullptr, &Size) || Size < 1) {
			return 1.0f;
		}
		auto PanelSpecSizeList = std::make_unique<sony::oz::xr_runtime::supported_panel_spec[]>(Size);
		if (!sony::oz::xr_runtime::GetPanelSpecOfSupportedDevices(k_PlatformId, PanelSpecSizeList.get(), &Size)) {
			return 1.0f;
		}
		return PanelSpecSizeList[static_cast<int>(ConnectedDevice_)].width / PanelSpecSizeList[0].width;
	}
	return 1.0f;
}

uint32_t XrProgram::GetDeviceIndex(const SonyOzDeviceInfo* DeviceList, const uint64_t Size)
{
	uint64_t DeviceIndex = 0;
	auto ItemList = std::make_unique<const wchar_t* []>(Size);
	int ItemCount = 0;
	for (int i = 0; i < Size; i++) {
		SonyOzDeviceInfo DeviceInfo = DeviceList[i];
		if (strcmp(DeviceInfo.product_id, "Deviceless") == 0) {
			continue;
		}
		const size_t ItemStrSize = sizeof(DeviceInfo.product_id) + sizeof(DeviceInfo.device_serial_number) + 1;
		char Str[ItemStrSize] = "";
		strcat_s(Str, ItemStrSize, DeviceInfo.product_id);
		strcat_s(Str, ItemStrSize, " ");
		strcat_s(Str, ItemStrSize, DeviceInfo.device_serial_number);
		wchar_t* ItemStr = new wchar_t[ItemStrSize];
		size_t ret = 0;
		mbstowcs_s(&ret, ItemStr, ItemStrSize, Str, _TRUNCATE);
		ItemList.get()[i] = ItemStr;
		ItemCount++;
	}
	if (ItemCount > 1) {
		DeviceIndex = sony::oz::xr_runtime::ShowComboBoxDialog(k_PlatformId, NULL, ItemList.get(), (int32_t)Size);
	}

	if (strcmp(DeviceList[DeviceIndex].product_id, "ELF-SR2") == 0) {
		ConnectedDevice = sony::oz::xr_runtime::SupportDevice::ELF_SR2;
	}
	else {
		ConnectedDevice = sony::oz::xr_runtime::SupportDevice::ELF_SR1;
	}

	return (uint32_t)DeviceIndex;
}




bool XrProgram::XrMainFunction()
{
	//this->depth_swapchain_format = -1;
	this->checkEvents();
	if (this->m_xr_shutdown == true)
	{
		return false;
	}

	// Get FrameState
	XrFrameState frame_state;
	frame_state.type = XR_TYPE_FRAME_STATE;
	frame_state.next = XR_NULL_HANDLE;

	XrFrameWaitInfo frame_wait_info;
	frame_wait_info.next = XR_NULL_HANDLE;
	frame_wait_info.type = XR_TYPE_FRAME_WAIT_INFO;

	if (!this->checkXrResult(xrWaitFrame(this->m_session, &frame_wait_info, &frame_state)))
	{
		printf("Unable to get Frame State\n");
		return false;
	}
	
	XrViewLocateInfo view_locate_info;
	view_locate_info.type = XR_TYPE_VIEW_LOCATE_INFO;
	view_locate_info.next = XR_NULL_HANDLE;
	view_locate_info.viewConfigurationType = this->m_view_type;
	view_locate_info.displayTime = frame_state.predictedDisplayTime;
	view_locate_info.space = this->m_reference_space;

	uint32_t view_count = static_cast<uint32_t>(this->m_xr_config_views.size());
	std::vector<XrView> views;
	views.resize(view_count);
	for (uint32_t i = 0; i < view_count; i++) 
	{
		views[i].type = XR_TYPE_VIEW;
		views[i].next = NULL;
	}

	XrViewState view_state;
	view_state.type = XR_TYPE_VIEW_STATE;
	view_state.next = XR_NULL_HANDLE;
	//view_state.viewStateFlags = 0;

	// Setting view space to runtime based on sr1 and sr2.
	float view_space_scale_ = VIEW_SPACE_SCALE / m_scale_factor;
	sony::openxr::extension::SetViewSpaceScale(this->m_session, view_space_scale_);

	if (!this->checkXrResult(xrLocateViews(this->m_session, &view_locate_info, &view_state, view_count, &view_count, views.data()))) 
	{
		printf("Couldn't Locate Views\n");
		return false;
	}

	XrFrameBeginInfo frame_begin_info;
	frame_begin_info.type = XR_TYPE_FRAME_BEGIN_INFO;
	frame_begin_info.next = XR_NULL_HANDLE;

	if (!this->checkXrResult(xrBeginFrame(this->m_session, &frame_begin_info))) 
	{
		printf("Couldn't begin frame\n");
		return false;
	}

	for (uint32_t i = 0; i < m_swapchains.size(); i++)
	{

		XrFovf left_eye = views[0].fov;
		XrFovf right_eye = views[1].fov;

		//Actual Creation of projection and View Matrix
		XrMatrix4x4f Projection_matrix_L;
		XrMatrix4x4f_CreateProjectionFov(&Projection_matrix_L, GRAPHICS_OPENGL, left_eye, m_near_z, m_far_z);
		XrMatrix4x4f view_matrix_L;
		XrMatrix4x4f_CreateViewMatrix(&view_matrix_L, &views[0].pose.position, &views[0].pose.orientation);
		XrMatrix4x4f Projection_matrix_R;
		XrMatrix4x4f_CreateProjectionFov(&Projection_matrix_R, GRAPHICS_OPENGL, right_eye, m_near_z, m_far_z);
		XrMatrix4x4f view_matrix_R;
		XrMatrix4x4f_CreateViewMatrix(&view_matrix_R, &views[1].pose.position, &views[1].pose.orientation);

		//Wait to aquire swapchain info
		XrSwapchainImageAcquireInfo swapchain_image_aquire_info;
		swapchain_image_aquire_info.type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO;
		swapchain_image_aquire_info.next = XR_NULL_HANDLE;

		uint32_t index;
		if (!this->checkXrResult(xrAcquireSwapchainImage(this->m_swapchains[i], &swapchain_image_aquire_info, &index))) 
		{
			printf("Unable to aquire swapchain Image Index");
			return false;
		}

		XrSwapchainImageWaitInfo swapchain_image_wait_info;
		swapchain_image_wait_info.type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO;
		swapchain_image_wait_info.next = XR_NULL_HANDLE;
		swapchain_image_wait_info.timeout = 1000;

		if (!this->checkXrResult(xrWaitSwapchainImage(this->m_swapchains[i], &swapchain_image_wait_info))) 
		{
			printf("Unable to wait for swapchain image\n");
			return false;
		}

		uint32_t depth_index = UINT32_MAX;
		if (this->m_depth_swapchain_format != -1) 
		{
			//Wait to aquire swapchain info
			XrSwapchainImageAcquireInfo depth_swapchain_image_aquire_info;
			depth_swapchain_image_aquire_info.type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO;
			depth_swapchain_image_aquire_info.next = XR_NULL_HANDLE;

			if (!this->checkXrResult(xrAcquireSwapchainImage(this->m_depth_swapchains[i], &depth_swapchain_image_aquire_info, &depth_index)))
			{
				printf("Unable to aquire depth swapchain Image Index");
				return false;
			}

			XrSwapchainImageWaitInfo depth_swapchain_image_wait_info;
			depth_swapchain_image_wait_info.type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO;
			depth_swapchain_image_wait_info.next = XR_NULL_HANDLE;
			depth_swapchain_image_wait_info.timeout = 1000;
			if (!this->checkXrResult(xrWaitSwapchainImage(this->m_depth_swapchains[i], &depth_swapchain_image_wait_info)))
			{
				printf("Unable to wait for depth swapchain image\n");
				return false;
			}
		}
		this->m_projection_views[i].fov = views[i].fov;
		this->m_projection_views[i].pose = views[i].pose;

		GLuint depth_image = this->m_depth_swapchain_format != -1 ? this->m_depth_images[i][depth_index].image : UINT32_MAX;

		/*bool result = renderFrame(this->xr_config_views[i].recommendedImageRectWidth, this->xr_config_views[i].recommendedImageRectHeight, Projection_matrix, view_matrix, this->framebuffers[i][index], depth_image, this->images[i][index], frame_state.predictedDisplayTime);*/
		bool result = renderFrame_SidebySide(this->m_xr_config_views[i].recommendedImageRectWidth, 
																				 this->m_xr_config_views[i].recommendedImageRectHeight, 
																				 Projection_matrix_L, Projection_matrix_R, 
																				 view_matrix_L, view_matrix_R, this->m_framebuffers[i][index], 
																				 depth_image, this->m_images[i][index], frame_state.predictedDisplayTime);
		
		if (!result) 
		{
			printf("unable to render frame\n");
			return false;
		}
		//Wait for all openGL calls to be done before contiuing
		glFinish();
		glFlush();
		XrSwapchainImageReleaseInfo swapchain_image_release_info;
		swapchain_image_release_info.type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO;
		swapchain_image_release_info.next = XR_NULL_HANDLE;

		if (!this->checkXrResult(xrReleaseSwapchainImage(this->m_swapchains[i], &swapchain_image_release_info))) 
		{
			printf("Unable to release swapchain Image\n");
			return false;
		}
		
		if (this->m_depth_swapchain_format != -1) 
		{
			XrSwapchainImageReleaseInfo depth_swapchain_image_release_info;
			depth_swapchain_image_release_info.type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO;
			depth_swapchain_image_release_info.next = XR_NULL_HANDLE;

			if (!this->checkXrResult(xrReleaseSwapchainImage(this->m_depth_swapchains[i], &depth_swapchain_image_release_info)))
			{
				printf("Unable to release depth swapchain Image\n");
				return false;
			}
		}
	}

	XrCompositionLayerProjection projection_layer;
	projection_layer.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION;
	projection_layer.next = XR_NULL_HANDLE;
	projection_layer.layerFlags = 0;
	projection_layer.space = this->m_reference_space;
	projection_layer.viewCount = view_count;
	projection_layer.views = this->m_projection_views.data();

	XrCompositionLayerBaseHeader* composition_layers[1];
	composition_layers[0] = (XrCompositionLayerBaseHeader *)&projection_layer;
	XrFrameEndInfo frame_end_info;
	frame_end_info.type = XR_TYPE_FRAME_END_INFO;
	frame_end_info.next = NULL;
	frame_end_info.displayTime = frame_state.predictedDisplayTime;
	frame_end_info.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
	frame_end_info.layerCount = 1;
	frame_end_info.layers = composition_layers;

	if (!this->checkXrResult(xrEndFrame(this->m_session, &frame_end_info))) 
	{
		printf("Unable to End Frame\n");
		return false;
	}

	return true;
}


//
//static void DrawScene(
//	const float view[16], 
//	const float proj[16],
//	float length   = 1.0f, 
//	float distance = 0.0f)
//{
//	glUseProgram(0);
//	glEnable(GL_DEPTH_TEST);
//	glDisable(GL_CULL_FACE); // 線なのでカリング不要
//
//	// 射影
//	glMatrixMode(GL_PROJECTION);
//	glLoadMatrixf(proj);
//
//	// モデルビュー = view * model
//	glMatrixMode(GL_MODELVIEW);
//	glLoadMatrixf(view);
//
//	// ★ ビュー空間で前へ（-Z 方向に）オフセットして原点に軸を出す
//	glTranslatef(0.f, 0.f, -distance);
//
//	glLineWidth(5.0f);
//	glBegin(GL_LINES);
//	glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(length, 0, 0);   // X 赤
//	glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, length, 0);   // Y 緑
//	glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, length);   // Z 青
//	glEnd();
//
//
//
//
//}
//
//



bool XrProgram::renderFrame_SidebySide(
		int width, 
		int height, 
		XrMatrix4x4f perspective_matrix_L, 
		XrMatrix4x4f perspective_matrix_R, 
		XrMatrix4x4f view_matrix_L, 
		XrMatrix4x4f view_matrix_R, 
		GLuint framebuffer, 
		GLuint depthbuffer, 
		XrSwapchainImageOpenGLKHR image, 
		XrTime predicted_time)
{
	//Bind the framebuffer to openGL
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glViewport(0, 0, width/2, height);
	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 0, width/2, height);

	//Clear the framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//Attach depth buffers
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image.image, 0);
	if (depthbuffer != UINT32_MAX) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthbuffer, 0);
	}
	
	ModeCore::GetInst()->DrawScene(view_matrix_L.m, perspective_matrix_L.m);


	glViewport(width/2, 0, width / 2, height);
	glScissor(width/2, 0, width / 2, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	RenderCore::GetInst()->XrDrawScene(view_matrix_R.m, perspective_matrix_R.m);

	glDisable(GL_SCISSOR_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}




bool XrProgram::checkEvents()
{
	XrEventDataBuffer runtime_event = { XR_TYPE_EVENT_DATA_BUFFER };
	runtime_event.next = NULL;
	XrResult result = xrPollEvent(this->m_instance, &runtime_event);
	while (result == XR_SUCCESS)
	{
		switch (runtime_event.type) 
		{
		case(XR_TYPE_EVENT_DATA_EVENTS_LOST):
			break;
		case(XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING):
			this->m_xr_shutdown = true;
			break;
		case(XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED):

			break;
		case(XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED):
			this->m_state = ((XrEventDataSessionStateChanged*)&runtime_event)->state;
			if (this->m_state == XR_SESSION_STATE_STOPPING) 
			{
				this->m_xr_shutdown = true;
			}
			break;
		default:
			break;
		}

		//Grab next event
		XrEventDataBuffer runtime_event = { XR_TYPE_EVENT_DATA_BUFFER };
		runtime_event.next = NULL;
		result = xrPollEvent(this->m_instance, &runtime_event);
	}
	return true;
}



bool XrProgram::checkXrResult(XrResult result) 
{
	if (result != XR_SUCCESS) 
	{
		return false;
	}
	return true;
}



void XrProgram::destroy()
{
	for (int i = 0; i < this->m_swapchains.size(); i++)
	{
		if (this->m_swapchains[i] != XR_NULL_HANDLE)
		{
			xrDestroySwapchain(this->m_swapchains[i]);
		}
	}

	if (this->depth.supported) 
	{
		for (int i = 0; i < this->m_depth_swapchains.size(); i++)
		{
			if (this->m_depth_swapchains[i] != XR_NULL_HANDLE)
			{
				xrDestroySwapchain(this->m_depth_swapchains[i]);
			}
		}
	}

	if (this->m_reference_space != XR_NULL_HANDLE)
	{
		xrDestroySpace(this->m_reference_space);
	}

	if (this->m_session != XR_NULL_HANDLE)
	{
		xrDestroySession(this->m_session);
	}

	if (this->m_instance != XR_NULL_HANDLE) 
	{
		sony::openxr::extension::UnlinkXrLibrary();
		sony::oz::xr_runtime::UnlinkXrLibrary();
		xrDestroyInstance(this->m_instance);
	}
}





/*
利用してないので一旦削除（by Takashi)
// sony runtime
extern int frameCount;

#define ENSURE_SONY_OZ_RESULT(ApiCall) \
		{ \
				SonyOzResult _result = (ApiCall); \
				if (_result != SonyOzResult::SUCCESS) \
				{ \
						UE_LOG(SRDisplayOpenXR, Error, TEXT("Error occurred: %d"), static_cast<int32>(_result)); \
						ensure(false); \
				} \
		}




bool XrProgram::renderFrame(
		int width, int height,
		XrMatrix4x4f perspective_matrix,
		XrMatrix4x4f view_matrix,
		GLuint framebuffer,
		GLuint depthbuffer, XrSwapchainImageOpenGLKHR image, XrTime predicted_time)
{
	//Bind the framebuffer to openGL
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glViewport(0, 0, width, height);
	glScissor(0, 0, width, height);

	//Clear the framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Attach depth buffers
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image.image, 0);
	if (depthbuffer != UINT32_MAX) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthbuffer, 0);
	}

	XrMatrix4x4f vp_matrix_xr;
	XrMatrix4x4f_Multiply(&vp_matrix_xr, &perspective_matrix, &view_matrix);
	glm::mat4 vp_matrix = glm::make_mat4(vp_matrix_xr.m);
	this->m_square.draw(vp_matrix);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}
*/
