#pragma once
#include <QFileInfo>
#include<filesystem>

class FileSystemNode
{
public:
	using FSNodes = std::vector<std::unique_ptr<FileSystemNode>>;
	FileSystemNode() = delete;
	virtual ~FileSystemNode() = default;
	template<typename T, typename = std::enable_if<std::is_convertible_v<T, std::filesystem::path>>>
	explicit FileSystemNode(T&& path);
	[[nodiscard]] static std::string s_get_creation_date(const std::filesystem::path& p_path);
	[[nodiscard]] static std::string s_get_last_access_date(const std::filesystem::path& p_path);
	[[nodiscard]] static std::string s_get_last_modification_date(const std::filesystem::path& p_path);
	void update_time_info();
	void delete_();
	void copy() const;														
	void cut();
	[[nodiscard]] const std::filesystem::path& get_path() const;
	[[nodiscard]] const std::filesystem::path& get_name() const;
	[[nodiscard]] std::string_view get_last_write_date() const;
	[[nodiscard]] std::string_view get_creation_date() const;
	[[nodiscard]] std::string_view get_last_access_date() const;
	[[nodiscard]] bool is_changed() const;
	[[nodiscard]] virtual uint64_t get_size() const = 0;
	[[nodiscard]] bool is_file() const;
	
protected:
	bool m_changed_;
	std::filesystem::path m_path_;
	std::filesystem::path m_name_;
	std::string m_last_access_date_;
	std::string m_last_modification_date_;
	std::string m_creation_date_;
	QFileInfo info_;
};


template <typename T, typename>
FileSystemNode::FileSystemNode(T&& path)
{
	m_path_ = std::forward<T>(path);
	m_name_ = m_path_.filename();
	m_changed_ = false;
	info_ = QFileInfo(m_path_);
}


