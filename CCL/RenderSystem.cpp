#include "RenderSystem.h"
#include "SpriteModule.h"

SamplerStates::SamplerStates(ID3D11Device* device) {
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC samplerDesc;						// サンプラーの状態を記述
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	// サンプリングするときに使用するフィルタリング方法 (今回は点サンプリングを縮小・拡大・ミップレベルサンプリングに使用)
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;		// 0~1のの u テクスチャ座標を解決するための方法 (???)
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;		// 
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;		//
	samplerDesc.MipLODBias = 0;							// 計算されたミップマップレベルからのオフセット
	samplerDesc.MaxAnisotropy = 16;						// D3D11_FILTER_ANISOTROPICまたはD3D11_FILTER_COMPARISON_ANISOTROPICが指定されている場合に使用されるクランプ値。有効な値は 1 ~ 16
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	// サンプリングされたデータを既存のサンプリングデータと比較する関数 (今回の場合毎回比較する)
	samplerDesc.BorderColor[0] = 0.0f;						// Address U V W の境界線の色 0 ~ 1.0f
	samplerDesc.BorderColor[1] = 0.0f;						// 今回はいらなくない？
	samplerDesc.BorderColor[2] = 0.0f;						// 
	samplerDesc.BorderColor[3] = 0.0f;						// 
	samplerDesc.MinLOD = 0;								// クランプアクセスするミップマップ範囲の下端
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;				// 上端（今回の場合はできる限り大きな値を入れているが0が最大）

	hr = device->CreateSamplerState(						// テクスチャのサンプリング情報をカプセル化するサンプラー状態オブジェクトを作成
		&samplerDesc,										// サンプラー状態の説明へのポインター
		&samplerStates[0]									// 作成されたサンプラー状態オブジェクトへのポインタのアドレス
	);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// 今回は縮小・拡大にはポイントサンプリングを使用、ミップレベルサンプリングは線形補間を使用
	hr = device->CreateSamplerState(
		&samplerDesc,
		&samplerStates[1]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;			// 今回は異方性補間を使用
	hr = device->CreateSamplerState(
		&samplerDesc,
		&samplerStates[2]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

DepthStencilStates::DepthStencilStates(ID3D11Device* device) {
	HRESULT hr = S_OK;

	// 深度ステンシルステートオブジェクトの作成
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	// 深度テスト オン、深度Write オン
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilStates[static_cast<int>(DEPTH_STATE::ENABLE_AND_WRITE)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度テスト オフ、深度Write オン
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilStates[static_cast<int>(DEPTH_STATE::DISABLE_AND_WRITE)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度テスト オン、深度Write オフ
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilStates[static_cast<int>(DEPTH_STATE::ENABLE_NOT_WRITE)]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度テスト オフ、深度Write オフ
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

	// alphaBlendの設定を行う
	//  res.r = dst.r * (1 - src.a) + src.r * src.a
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;			// ピクセルシェーダの出力に対する操作
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;		// 現在のRGBの値に対して行う操作
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				// Src Dst の操作の組み合わせ方を定義
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;					// ピクセルシェーダのアルファ出力に対する操作
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;					// 現在のアルファに対する操作
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;				// Src Dst の操作の組み合わせ方を定義
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;		// 書き込みマスク
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
		0,											// デバイスの0から始まる配列にインデックスをつけサンプラーの設定を開始する
		1,											// 配列内のサンプラー数
		renderSettings.samplerStates.GetAddressOf(SAMPLER_STATE::POINT)							// サンプラー状態のインターフェイスの配列へのポインタ
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
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG; // デバイスの作成に使用されるパラメータの説明
#endif

	D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0; // DirectXデバイスの対象となる機能のセットについての説明 

	DXGI_SWAP_CHAIN_DESC swapChainDesc = MakeSwapChainDesk(screenWidth, screenHeight, hwnd, fullScreen);


	hr = D3D11CreateDeviceAndSwapChain(			// 戻り値　[Direct3D 11　リターンコード]のいずれかを返す

		NULL,									// IDXGIアダプター　既定のアダプタならNULL
		D3D_DRIVER_TYPE_HARDWARE,				// DriverType [ D3D_DRIVER_TYPE ] 
		NULL,									// Software drivertypeが D3D_DRIVER_TYPE_SOFTWARE なら　NULL以外にすること
		createDeviceFlags,					// Flags 有効にするランタイムレイヤー [ D3D11_CREATE_DEVICE_FLAG ]
		&featureLevels,						// featureLevels [ D3D_FEATURE_LEVEL ] 作成しようする機能レベルの順序を決定する [] の配列へのポインタ　NULLでも既定の機能の配列を使用する
		1,										// feature_levelsの要素数
		D3D11_SDK_VERSION,						// SDKのバージョン
		&swapChainDesc,						// swapchaindesc_p スワップチェーンへのポインター 
		&swapChain,								// swapchain_pp 表示に使われるスワップチェーンを表す[ IDXGISwapChain ]オブジェクトへのポインターのアドレス
		&device,								// 作成されたデバイスを表す [ ID3D11Device ]オブジェクトへのポインターのアドレスを返す　
		NULL,									// featurelevel_p デバイスでサポートされた機能レベルの配列の最初の要素を表す　特定の必要がない場合NULL
		&immediateContext						// デバイスコンテキスト [ D3D11_CREATE_DEVICE_FLAG ]オブジェクトへのポインターを返す　NULLなら []は返されない
	);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
void RenderSystem::InitRenderTargetView() {
	HRESULT hr = S_OK;

	//レンダーターゲットビューの作成 バックバッファーではなく中間バッファーにレンダリングが行えるようにする
	ID3D11Texture2D* backBuffer = {};				// 構造化メモリであるテクセルデータを管理
	hr = swapChain->GetBuffer(					// スワップチェーンのバッファの一つにアクセス
		0,										// 
		__uuidof(ID3D11Texture2D),				// バッファーの操作に使用されるインターフェースの型
		reinterpret_cast<LPVOID*>(&backBuffer) // バックバッファーインターフェースへのポインター
	);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = device->CreateRenderTargetView(
		backBuffer,								// レンダリングターゲットを表す [ID3D11リソース]へのポインター　[ D3D11_BIND_RENDER_TARGET ]フラグを使用して作成されている必要がある
		NULL,									// レンダーターゲットビューの説明をする[ D3D11_RENDER_TARGET_VIEW_DESC ]のポインターへのポインター
		renderTargetView.GetAddressOf()			//レンダーターゲットビューのポインターのアドレス
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	backBuffer->Release();
}
void RenderSystem::InitViewPort(UINT screenWidth, UINT screenHeight) {
	//ビューポート
	D3D11_VIEWPORT viewport = {};					// ビューポートの寸法を定義
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediateContext->RSSetViewports(			// ビューポートの配列をパイプラインのラスタライザ　ステージにバインド
		1,										// バインドするビューポートの数
		&viewport								// デバイスにバインドするビューポートの構造体配列
	);
}
void RenderSystem::Init2dTexture(UINT screenWidth, UINT screenHeight) {
	HRESULT hr = S_OK;

	//深度ステンシルビュー
	ID3D11Texture2D* depthStencilBuffer = {};
	D3D11_TEXTURE2D_DESC texture2dDesc = {};		// 2Dテクスチャに関して説明
	texture2dDesc.Width = screenWidth;		//
	texture2dDesc.Height = screenHeight;		//
	texture2dDesc.MipLevels = 1;				// テクスチャのミップマップレベルの最大数
	texture2dDesc.ArraySize = 1;				// テクスチャ配列内のテクスチャの配列数 1 = 2048
	texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2dDesc.SampleDesc.Count = 1;		// ピクセル当たりのマルチサンプル数
	texture2dDesc.SampleDesc.Quality = 0;		// 画質レベル
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;	// テクスチャの読み書きの方法を識別　[ D3D11_USAGE ]
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// パイプラインステージへのバインドに関するフラグ
	texture2dDesc.CPUAccessFlags = 0;			// CPUアクセスの種類を指定するフラグ　アクセス不要なら 0 
	texture2dDesc.MiscFlags = 0;				// 一般的でない他の李索子オプションを識別するフラグ

	hr = device->CreateTexture2D(				// 2Dテクスチャの配列を作成
		&texture2dDesc,						// 2Dテクスチャリソースを記述する [ D3D11_TEXTURE2D_DESC ]構造体へのポインター
		NULL,									// 2Dテクスチャリソースを記述する[ D3D11_SUBRESOURCE_DATA ]構造体へのポインター
		&depthStencilBuffer					// 作成されたテクスチャのID2D11Texture2D　インターフェースへのポインターを受け取るバッファーへのポインター
	);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};	// 深度ステンシルビューからアクセスできるテクスチャのサブリソースを指定します
	depthStencilViewDesc.Format = texture2dDesc.Format;		// 多分 texture2D_descと同じでいいかな
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// 深度ステンシルリソースへのアクセス方法を指定
	depthStencilViewDesc.Texture2D.MipSlice = 0;							// 最初のミップマップレベルのインデックス　（今回はそもそも最大が0）
	hr = device->CreateDepthStencilView(		// リソースデータにアクセスするための深度ステンシルビューを作成
		depthStencilBuffer,					// 深度ステンシル　サーフェスとして機能するリソースへのポインター[ D3D11_BIND_DEPTH_STENCIL ]フラグを使用して作成されている必要がある
		&depthStencilViewDesc,				// 深度ステンシルビューの説明のポインター
		depthStencilView.GetAddressOf()						// [ ID3D11DepthStencilView ]へのポインターのアドレス	
	);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	depthStencilBuffer->Release();
}
DXGI_SWAP_CHAIN_DESC RenderSystem::MakeSwapChainDesk(UINT screenWidth, UINT screenHeight, HWND hwnd, bool fullScreen) {
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {}; // スワップチェーンを記述
	swap_chain_desc.BufferCount = 1; //スワップチェーン内のバッファ数の指定
	//BufferDesc バッファ表示モードを記述
	swap_chain_desc.BufferDesc.Width = screenWidth;
	swap_chain_desc.BufferDesc.Height = screenHeight;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //バックバッファーのサーフェス使用状況とCPUアクセスオプションを記述　今回はリソースをレンダーターゲットとして仕様
	swap_chain_desc.OutputWindow = hwnd;			//出力ウィンドウへのハンドル　NULLはダメ
	swap_chain_desc.SampleDesc.Count = 1;			//マルチサンプリングパラメータを記述
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Windowed = !fullScreen;	//ウィンドウモードかを入力

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