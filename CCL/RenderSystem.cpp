#include "RenderSystem.h"
#include "SpriteModule.h"

SamplerStates::SamplerStates(ID3D11Device* device) {
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC samplerDesc;						// �T���v���[�̏�Ԃ��L�q
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	// �T���v�����O����Ƃ��Ɏg�p����t�B���^�����O���@ (����͓_�T���v�����O���k���E�g��E�~�b�v���x���T���v�����O�Ɏg�p)
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;		// 0~1�̂� u �e�N�X�`�����W���������邽�߂̕��@ (???)
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;		// 
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;		//
	samplerDesc.MipLODBias = 0;							// �v�Z���ꂽ�~�b�v�}�b�v���x������̃I�t�Z�b�g
	samplerDesc.MaxAnisotropy = 16;						// D3D11_FILTER_ANISOTROPIC�܂���D3D11_FILTER_COMPARISON_ANISOTROPIC���w�肳��Ă���ꍇ�Ɏg�p�����N�����v�l�B�L���Ȓl�� 1 ~ 16
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	// �T���v�����O���ꂽ�f�[�^�������̃T���v�����O�f�[�^�Ɣ�r����֐� (����̏ꍇ�����r����)
	samplerDesc.BorderColor[0] = 0.0f;						// Address U V W �̋��E���̐F 0 ~ 1.0f
	samplerDesc.BorderColor[1] = 0.0f;						// ����͂���Ȃ��Ȃ��H
	samplerDesc.BorderColor[2] = 0.0f;						// 
	samplerDesc.BorderColor[3] = 0.0f;						// 
	samplerDesc.MinLOD = 0;								// �N�����v�A�N�Z�X����~�b�v�}�b�v�͈͂̉��[
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;				// ��[�i����̏ꍇ�͂ł������傫�Ȓl�����Ă��邪0���ő�j

	hr = device->CreateSamplerState(						// �e�N�X�`���̃T���v�����O�����J�v�Z��������T���v���[��ԃI�u�W�F�N�g���쐬
		&samplerDesc,										// �T���v���[��Ԃ̐����ւ̃|�C���^�[
		&samplerStates[0]									// �쐬���ꂽ�T���v���[��ԃI�u�W�F�N�g�ւ̃|�C���^�̃A�h���X
	);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// ����͏k���E�g��ɂ̓|�C���g�T���v�����O���g�p�A�~�b�v���x���T���v�����O�͐��`��Ԃ��g�p
	hr = device->CreateSamplerState(
		&samplerDesc,
		&samplerStates[1]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;			// ����͈ٕ�����Ԃ��g�p
	hr = device->CreateSamplerState(
		&samplerDesc,
		&samplerStates[2]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

DepthStencilStates::DepthStencilStates(ID3D11Device* device) {
	HRESULT hr = S_OK;

	// �[�x�X�e���V���X�e�[�g�I�u�W�F�N�g�̍쐬
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	// �[�x�e�X�g �I���A�[�xWrite �I��
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilStates[static_cast<int>(DEPTH_STATE::ENABLE_AND_WRITE)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �[�x�e�X�g �I�t�A�[�xWrite �I��
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilStates[static_cast<int>(DEPTH_STATE::DISABLE_AND_WRITE)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �[�x�e�X�g �I���A�[�xWrite �I�t
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilStates[static_cast<int>(DEPTH_STATE::ENABLE_NOT_WRITE)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �[�x�e�X�g �I�t�A�[�xWrite �I�t
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilStates[static_cast<int>(DEPTH_STATE::DISABLE_NOT_WRITE)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

BlendStates::BlendStates(ID3D11Device* device) {
	HRESULT hr = S_OK;

	D3D11_BLEND_DESC blendDesc = {};

	// NONE
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, &blendStates[static_cast<int>(BLEND_TYPE::NONE)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// alphaBlend�̐ݒ���s��
	//  res.r = dst.r * (1 - src.a) + src.r * src.a
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;			// �s�N�Z���V�F�[�_�̏o�͂ɑ΂��鑀��
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;		// ���݂�RGB�̒l�ɑ΂��čs������
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				// Src Dst �̑���̑g�ݍ��킹�����`
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;					// �s�N�Z���V�F�[�_�̃A���t�@�o�͂ɑ΂��鑀��
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;					// ���݂̃A���t�@�ɑ΂��鑀��
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;				// Src Dst �̑���̑g�ݍ��킹�����`
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;		// �������݃}�X�N
	hr = device->CreateBlendState(&blendDesc, &blendStates[static_cast<int>(BLEND_TYPE::ALPHA)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// add
	//  res.r = dst.r + (src.r * src.a)
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3D11_BLEND_ONE
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, &blendStates[static_cast<int>(BLEND_TYPE::ADD)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// subtract
	//  res.r = dst.r - (src.r * src.a)
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO; //D3D11_BLEND_SRC_ALPHA
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR; //D3D11_BLEND_ONE
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; //D3D11_BLEND_OP_SUBTRACT
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, &blendStates[static_cast<int>(BLEND_TYPE::SUBTRACT)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// replace
	//  res.r = src.r * src.a
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, &blendStates[static_cast<int>(BLEND_TYPE::REPLACE)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// multiply
	//  res.r = src.r * dst.r
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO; //D3D11_BLEND_DEST_COLOR
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR; //D3D11_BLEND_SRC_COLOR
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, &blendStates[static_cast<int>(BLEND_TYPE::MULTIPLY)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// lighten
	//  res.r = max(src.r, dst.r)
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, &blendStates[static_cast<int>(BLEND_TYPE::LIGHTEN)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// darken
	//  res.r = min(src.r, dst.r)
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, &blendStates[static_cast<int>(BLEND_TYPE::DARKEN)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// screen
	//  res.r = dst.r * (1 - src.r) + (src.r * src.a)
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3DBLEND_INVDESTCOLOR
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR; //D3DBLEND_ONE
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, &blendStates[static_cast<int>(BLEND_TYPE::SCREEN)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

RasterizerStates::RasterizerStates(ID3D11Device* device) {
	HRESULT hr = S_OK;

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<int>(RASTERIZER_STATE::FILL_AND_CULL)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	rasterizerDesc.FrontCounterClockwise = TRUE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<int>(RASTERIZER_STATE::FILL_AND_CULL_COUNTERCLOCK)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.AntialiasedLineEnable = TRUE;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<int>(RASTERIZER_STATE::WIREFRAME_NOT_CULL)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizerDesc.FrontCounterClockwise = TRUE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<int>(RASTERIZER_STATE::WIREFRAME_NOT_CULL_COUNTERCLOCK)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.AntialiasedLineEnable = TRUE;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<int>(RASTERIZER_STATE::WIREFRAME_AND_CULL)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizerDesc.FrontCounterClockwise = TRUE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<int>(RASTERIZER_STATE::WIREFRAME_AND_CULL_COUNTERCLOCK)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rasterizerDesc.AntialiasedLineEnable = TRUE;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<int>(RASTERIZER_STATE::FILL_NOT_CULL)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizerDesc.FrontCounterClockwise = TRUE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<int>(RASTERIZER_STATE::FILL_NOT_CULL_COUNTERCLOCK)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

RenderSystem::RenderSystem(UINT screenWidth, UINT screenHeight, HWND hwnd, bool fullScreen) {
	InitDevice_DeviceContext_SwapChain(screenWidth, screenHeight, hwnd, fullScreen);
	InitRenderTargetView();
	InitViewPort(screenWidth, screenHeight);
	Init2dTexture(screenWidth, screenHeight);

	renderSettings = RenderSettings(device.Get());

	immediateContext->PSSetSamplers(
		0,											// �f�o�C�X��0����n�܂�z��ɃC���f�b�N�X�����T���v���[�̐ݒ���J�n����
		1,											// �z����̃T���v���[��
		renderSettings.samplerStates.GetAddressOf(SAMPLER_STATE::POINT)							// �T���v���[��Ԃ̃C���^�[�t�F�C�X�̔z��ւ̃|�C���^
	);
	immediateContext->PSSetSamplers(1, 1,
		renderSettings.samplerStates.GetAddressOf(SAMPLER_STATE::LINEAR));
	immediateContext->PSSetSamplers(2, 1,
		renderSettings.samplerStates.GetAddressOf(SAMPLER_STATE::ANISOTROPIC));

	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bufferDesc = {};

	bufferDesc.ByteWidth = sizeof(SceneConstants::Data);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;


	frameBuffers[0] = std::make_unique<FrameBuffer>(device.Get(), 1280.0f, 720.0f);
	frameBuffers[1] = std::make_unique<FrameBuffer>(device.Get(), 1280.0f / 2.0f, 720.0f / 2.0f);
	bitBlockTransfer = std::make_unique<FullScreenQuad>(device.Get());
	bitBlockWindow = std::make_unique<OffScreenWindow>(device.Get());
	CreatePsFromCso(device.Get(), "./resources/Shader/luminanceExtractionPS.cso", pixelShaders[0].GetAddressOf());
	CreatePsFromCso(device.Get(), "./resources/Shader/BlurPS.cso", pixelShaders[1].GetAddressOf());


	constantBuffer.AddConstantBuffer(1);
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOfBuffer(0));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void RenderSystem::InitDevice_DeviceContext_SwapChain(UINT screenWidth, UINT screenHeight, HWND hwnd, bool fullScreen) {
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG; // �f�o�C�X�̍쐬�Ɏg�p�����p�����[�^�̐���
#endif

	D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0; // DirectX�f�o�C�X�̑ΏۂƂȂ�@�\�̃Z�b�g�ɂ��Ă̐��� 

	DXGI_SWAP_CHAIN_DESC swapChainDesc = MakeSwapChainDesk(screenWidth, screenHeight, hwnd, fullScreen);


	hr = D3D11CreateDeviceAndSwapChain(			// �߂�l�@[Direct3D 11�@���^�[���R�[�h]�̂����ꂩ��Ԃ�

		NULL,									// IDXGI�A�_�v�^�[�@����̃A�_�v�^�Ȃ�NULL
		D3D_DRIVER_TYPE_HARDWARE,				// DriverType [ D3D_DRIVER_TYPE ] 
		NULL,									// Software drivertype�� D3D_DRIVER_TYPE_SOFTWARE �Ȃ�@NULL�ȊO�ɂ��邱��
		createDeviceFlags,					// Flags �L���ɂ��郉���^�C�����C���[ [ D3D11_CREATE_DEVICE_FLAG ]
		&featureLevels,						// featureLevels [ D3D_FEATURE_LEVEL ] �쐬���悤����@�\���x���̏��������肷�� [] �̔z��ւ̃|�C���^�@NULL�ł�����̋@�\�̔z����g�p����
		1,										// feature_levels�̗v�f��
		D3D11_SDK_VERSION,						// SDK�̃o�[�W����
		&swapChainDesc,						// swapchaindesc_p �X���b�v�`�F�[���ւ̃|�C���^�[ 
		&swapChain,								// swapchain_pp �\���Ɏg����X���b�v�`�F�[����\��[ IDXGISwapChain ]�I�u�W�F�N�g�ւ̃|�C���^�[�̃A�h���X
		&device,								// �쐬���ꂽ�f�o�C�X��\�� [ ID3D11Device ]�I�u�W�F�N�g�ւ̃|�C���^�[�̃A�h���X��Ԃ��@
		NULL,									// featurelevel_p �f�o�C�X�ŃT�|�[�g���ꂽ�@�\���x���̔z��̍ŏ��̗v�f��\���@����̕K�v���Ȃ��ꍇNULL
		&immediateContext						// �f�o�C�X�R���e�L�X�g [ D3D11_CREATE_DEVICE_FLAG ]�I�u�W�F�N�g�ւ̃|�C���^�[��Ԃ��@NULL�Ȃ� []�͕Ԃ���Ȃ�
	);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
void RenderSystem::InitRenderTargetView() {
	HRESULT hr = S_OK;

	//�����_�[�^�[�Q�b�g�r���[�̍쐬 �o�b�N�o�b�t�@�[�ł͂Ȃ����ԃo�b�t�@�[�Ƀ����_�����O���s����悤�ɂ���
	ID3D11Texture2D* backBuffer = {};				// �\�����������ł���e�N�Z���f�[�^���Ǘ�
	hr = swapChain->GetBuffer(					// �X���b�v�`�F�[���̃o�b�t�@�̈�ɃA�N�Z�X
		0,										// 
		__uuidof(ID3D11Texture2D),				// �o�b�t�@�[�̑���Ɏg�p�����C���^�[�t�F�[�X�̌^
		reinterpret_cast<LPVOID*>(&backBuffer) // �o�b�N�o�b�t�@�[�C���^�[�t�F�[�X�ւ̃|�C���^�[
	);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = device->CreateRenderTargetView(
		backBuffer,								// �����_�����O�^�[�Q�b�g��\�� [ID3D11���\�[�X]�ւ̃|�C���^�[�@[ D3D11_BIND_RENDER_TARGET ]�t���O���g�p���č쐬����Ă���K�v������
		NULL,									// �����_�[�^�[�Q�b�g�r���[�̐���������[ D3D11_RENDER_TARGET_VIEW_DESC ]�̃|�C���^�[�ւ̃|�C���^�[
		renderTargetView.GetAddressOf()			//�����_�[�^�[�Q�b�g�r���[�̃|�C���^�[�̃A�h���X
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	backBuffer->Release();
}
void RenderSystem::InitViewPort(UINT screenWidth, UINT screenHeight) {
	//�r���[�|�[�g
	D3D11_VIEWPORT viewport = {};					// �r���[�|�[�g�̐��@���`
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediateContext->RSSetViewports(			// �r���[�|�[�g�̔z����p�C�v���C���̃��X�^���C�U�@�X�e�[�W�Ƀo�C���h
		1,										// �o�C���h����r���[�|�[�g�̐�
		&viewport								// �f�o�C�X�Ƀo�C���h����r���[�|�[�g�̍\���̔z��
	);
}
void RenderSystem::Init2dTexture(UINT screenWidth, UINT screenHeight) {
	HRESULT hr = S_OK;

	//�[�x�X�e���V���r���[
	ID3D11Texture2D* depthStencilBuffer = {};
	D3D11_TEXTURE2D_DESC texture2dDesc = {};		// 2D�e�N�X�`���Ɋւ��Đ���
	texture2dDesc.Width = screenWidth;		//
	texture2dDesc.Height = screenHeight;		//
	texture2dDesc.MipLevels = 1;				// �e�N�X�`���̃~�b�v�}�b�v���x���̍ő吔
	texture2dDesc.ArraySize = 1;				// �e�N�X�`���z����̃e�N�X�`���̔z�� 1 = 2048
	texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2dDesc.SampleDesc.Count = 1;		// �s�N�Z��������̃}���`�T���v����
	texture2dDesc.SampleDesc.Quality = 0;		// �掿���x��
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;	// �e�N�X�`���̓ǂݏ����̕��@�����ʁ@[ D3D11_USAGE ]
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// �p�C�v���C���X�e�[�W�ւ̃o�C���h�Ɋւ���t���O
	texture2dDesc.CPUAccessFlags = 0;			// CPU�A�N�Z�X�̎�ނ��w�肷��t���O�@�A�N�Z�X�s�v�Ȃ� 0 
	texture2dDesc.MiscFlags = 0;				// ��ʓI�łȂ����̗����q�I�v�V���������ʂ���t���O

	hr = device->CreateTexture2D(				// 2D�e�N�X�`���̔z����쐬
		&texture2dDesc,						// 2D�e�N�X�`�����\�[�X���L�q���� [ D3D11_TEXTURE2D_DESC ]�\���̂ւ̃|�C���^�[
		NULL,									// 2D�e�N�X�`�����\�[�X���L�q����[ D3D11_SUBRESOURCE_DATA ]�\���̂ւ̃|�C���^�[
		&depthStencilBuffer					// �쐬���ꂽ�e�N�X�`����ID2D11Texture2D�@�C���^�[�t�F�[�X�ւ̃|�C���^�[���󂯎��o�b�t�@�[�ւ̃|�C���^�[
	);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};	// �[�x�X�e���V���r���[����A�N�Z�X�ł���e�N�X�`���̃T�u���\�[�X���w�肵�܂�
	depthStencilViewDesc.Format = texture2dDesc.Format;		// ���� texture2D_desc�Ɠ����ł�������
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// �[�x�X�e���V�����\�[�X�ւ̃A�N�Z�X���@���w��
	depthStencilViewDesc.Texture2D.MipSlice = 0;							// �ŏ��̃~�b�v�}�b�v���x���̃C���f�b�N�X�@�i����͂��������ő傪0�j
	hr = device->CreateDepthStencilView(		// ���\�[�X�f�[�^�ɃA�N�Z�X���邽�߂̐[�x�X�e���V���r���[���쐬
		depthStencilBuffer,					// �[�x�X�e���V���@�T�[�t�F�X�Ƃ��ċ@�\���郊�\�[�X�ւ̃|�C���^�[[ D3D11_BIND_DEPTH_STENCIL ]�t���O���g�p���č쐬����Ă���K�v������
		&depthStencilViewDesc,				// �[�x�X�e���V���r���[�̐����̃|�C���^�[
		depthStencilView.GetAddressOf()						// [ ID3D11DepthStencilView ]�ւ̃|�C���^�[�̃A�h���X	
	);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	depthStencilBuffer->Release();
}
DXGI_SWAP_CHAIN_DESC RenderSystem::MakeSwapChainDesk(UINT screenWidth, UINT screenHeight, HWND hwnd, bool fullScreen) {
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {}; // �X���b�v�`�F�[�����L�q
	swap_chain_desc.BufferCount = 1; //�X���b�v�`�F�[�����̃o�b�t�@���̎w��
	//BufferDesc �o�b�t�@�\�����[�h���L�q
	swap_chain_desc.BufferDesc.Width = screenWidth;
	swap_chain_desc.BufferDesc.Height = screenHeight;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //�o�b�N�o�b�t�@�[�̃T�[�t�F�X�g�p�󋵂�CPU�A�N�Z�X�I�v�V�������L�q�@����̓��\�[�X�������_�[�^�[�Q�b�g�Ƃ��Ďd�l
	swap_chain_desc.OutputWindow = hwnd;			//�o�̓E�B���h�E�ւ̃n���h���@NULL�̓_��
	swap_chain_desc.SampleDesc.Count = 1;			//�}���`�T���v�����O�p�����[�^���L�q
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Windowed = !fullScreen;	//�E�B���h�E���[�h�������

	return swap_chain_desc;
}

void RenderSystem::UpdateSubresourceData(int index) {
	immediateContext->UpdateSubresource(constantBuffer.GetBuffer(index), 0, 0, constantBuffer.GetBufferData(index) , 0, 0);
}
void RenderSystem::SetVSConstantBuffer(int index) {
	immediateContext->VSSetConstantBuffers(1, 1, constantBuffer.GetAddressOfBuffer(index));
}
void RenderSystem::SetPSConstantBuffer(int index) {
	immediateContext->PSSetConstantBuffers(1, 1, constantBuffer.GetAddressOfBuffer(index));
}