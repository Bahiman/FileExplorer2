#pragma once
#include<filesystem>
#include <windows.h>
#include <shlobj.h>

class COleInitialize {
public:
 COleInitialize() : m_hr(OleInitialize(NULL)) { }
 ~COleInitialize() { if (SUCCEEDED(m_hr)) OleUninitialize(); }
 operator HRESULT() const { return m_hr; }
 HRESULT m_hr;
};
namespace test
{
	inline void copy_into_buffer(const std::wstring& ref)
	{
		/*if (!std::filesystem::exists(ref))
			throw std::runtime_error("Doesn't exist!");

		std::ifstream stream(ref, std::ios::binary);

		std::vector<char> buffer((std::istreambuf_iterator<char>(stream)), (std::istreambuf_iterator<char>()));

		std::string encoded = cppcodec::base64_rfc4648::encode(buffer.data(), buffer.size());

		if (!OpenClipboard(nullptr))
			throw std::runtime_error("Couldn't open the clipboard");

		EmptyClipboard();

		HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, encoded.size());

		if (!hg) {
			CloseClipboard();
			throw std::runtime_error("Couldn't allocate the memory for the file!");
		}

		cos()

		memcpy(GlobalLock(hg), encoded.c_str(), encoded.size());
		GlobalUnlock(hg);
		SetClipboardData(CF_TEXT, hg);
		CloseClipboard();
		GlobalFree(hg);*/

		HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

		IShellItem* psi = nullptr;

		hr = SHCreateItemFromParsingName(ref.c_str(), nullptr, IID_PPV_ARGS(&psi));

		if (SUCCEEDED(hr))
		{
			IDataObject* objector = nullptr;
			hr = psi->BindToHandler(nullptr, BHID_DataObject, IID_PPV_ARGS(&objector));
			if(SUCCEEDED(hr))
			{
				hr = OleSetClipboard(objector);
				if (SUCCEEDED(hr))
				{
					// Ensure the data is copied to the clipboard
					hr = OleFlushClipboard();
				}
				objector->Release();
			}
			psi->Release();
		}
		CoUninitialize();
	}

}
