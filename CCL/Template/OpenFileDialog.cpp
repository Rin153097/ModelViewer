#include "OpenFileDialog.h"

#include <shobjidl_core.h>



FilePathList FilePathManager::pathList = {};
CStringA     FilePathManager::next = {};
CStringA     FilePathManager::null = {};


void FilePathManager::GetPathByDialog() {
	// COMライブラリーをイニシャライズする
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr))
	{
		// extend settings
		const COMDLG_FILTERSPEC c_rgShowTypes[] =
		{
			{L"fbx",			L"*.fbx"},
			{L"cereal",			L"*.cereal"},
			{L"すべて (*.*)",	L"*.*"}
		};

		// make Dialog
		IFileOpenDialog* pfd = nullptr;

		hr = CoCreateInstance( 
			CLSID_FileOpenDialog, 
			NULL, 
			CLSCTX_INPROC_SERVER, 
			IID_PPV_ARGS(&pfd));  // f1 -> macro
			//IID_IFileOpenDialog,
			//reinterpret_cast<void**>(&pfd));

		if (SUCCEEDED(hr))
		{
			DWORD dwFlags = 0;
			pfd->GetOptions(&dwFlags);  // defalutFlag
			pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM | FOS_ALLOWMULTISELECT);  // multi select
			pfd->SetFileTypes(ARRAYSIZE(c_rgShowTypes), c_rgShowTypes);  // types
			pfd->SetFileTypeIndex(1);  // index + 1;
			pfd->SetOkButtonLabel(L"開く");  // Change Button text "OK"
			pfd->SetTitle(L"読み込むDataを選択");  // title

			// g_aryDeleteFiles.RemoveAll();

			hr = pfd->Show(NULL); // Show Dialog

			if (SUCCEEDED(hr))
			{
				IShellItemArray* pShellItemResult;
				hr = pfd->GetResults(&pShellItemResult); // GetDataList

				if (SUCCEEDED(hr))
				{
					DWORD dwNumItems = 0;
					pShellItemResult->GetCount(&dwNumItems); 

					IShellItem* pShellItem = NULL;

					for (DWORD i = 0; i < dwNumItems; i++)
					{
						hr = pShellItemResult->GetItemAt(i, &pShellItem);

						if (SUCCEEDED(hr))
						{
							PWSTR pszFilePath = NULL;
							hr = pShellItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

							if (SUCCEEDED(hr))
							{
								CStringA strA(pszFilePath);
								pathList.push(strA);

								LPCSTR str = strA.GetString();

								CoTaskMemFree(pszFilePath);
							}
							pShellItem->Release();
						}
					}
					pShellItemResult->Release();
				}
			}

			pfd->Release();  // IUnknown::Release COMオブジェクトの参照カウントを減らす
		}

		CoUninitialize();
	}
}