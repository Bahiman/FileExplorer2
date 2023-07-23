#include "FileSystemNode.h"
#include<ctime>
#include<sys/stat.h>
#include <windows.h>
#include <shlobj.h>


std::string FileSystemNode::s_get_creation_date(const std::filesystem::path& p_path)
{ 
	struct stat file_stat{};

	const auto path_str = p_path.string();

	if (stat(path_str.c_str(), &file_stat) < 0)
		throw std::runtime_error("Couldn't get the stats of the file");

	char tm_buff[128];

	if(ctime_s(tm_buff,sizeof(tm_buff),& file_stat.st_ctime))
		throw std::runtime_error("Couldn't get the creation date of the file");

	return tm_buff;
}

std::string FileSystemNode::s_get_last_access_date(const std::filesystem::path& p_path)
{
	struct stat file_stat{};

	const auto path_str = p_path.string();

	if(stat(path_str.c_str(), &file_stat) < 0)
		throw std::runtime_error("Couldn't get the stats of the file");

	return ctime(&file_stat.st_atime);
}

std::string FileSystemNode::s_get_last_modification_date(const std::filesystem::path& p_path)
{
	struct stat file_stat{};

	const auto path_str = p_path.string();

	if (stat(path_str.c_str(), &file_stat) < 0)
		throw std::runtime_error("Couldn't get the stats of the file");

	char tm_buff[128];

	if (ctime_s(tm_buff, sizeof(tm_buff), &file_stat.st_mtime))
		throw std::runtime_error("Couldn't get the last modification date of the file");

	return tm_buff;
}

void FileSystemNode::update_time_info()
{
	m_creation_date_ = s_get_creation_date(m_path_);

	m_last_modification_date_ = s_get_last_modification_date(m_path_);

	m_last_access_date_ = s_get_last_access_date(m_path_);
}

void FileSystemNode::delete_()
{
	const std::string& path_to_string = m_path_.string();

	if(const char* file_path = path_to_string.c_str(); std::remove(file_path))
		throw std::runtime_error("Couldn't delete the file");

	m_path_ = "??????????????????";
}

void FileSystemNode::copy() const
{

	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	IShellItem* psi = nullptr;

	hr = SHCreateItemFromParsingName(m_path_.c_str(), nullptr, IID_PPV_ARGS(&psi));

	if (SUCCEEDED(hr))
	{
		IDataObject* objector = nullptr;
		hr = psi->BindToHandler(nullptr, BHID_DataObject, IID_PPV_ARGS(&objector));
		if (SUCCEEDED(hr))
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

void FileSystemNode::cut()
{
	copy();
	delete_();
}

bool FileSystemNode::is_changed() const
{
	return m_changed_;
}

bool FileSystemNode::is_file() const
{
	return std::filesystem::is_regular_file(m_path_);
}

const std::filesystem::path& FileSystemNode::get_path() const
{
	return m_path_;
}

const std::filesystem::path& FileSystemNode::get_name() const
{
	return m_name_;
}

std::string_view FileSystemNode::get_last_write_date() const
{
	return m_last_modification_date_;
}

std::string_view FileSystemNode::get_creation_date() const
{
	return m_creation_date_;
}

std::string_view FileSystemNode::get_last_access_date() const
{
	return m_last_access_date_;
}
