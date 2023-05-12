/*
* @date   2023-02-20
* @author dudu
* @brief  ${FILE_DESCRIPTION}
*/
#pragma once

#include <filesystem>
#include <vector>
#include "Utils/UUID.hpp"

namespace SPW
{
	namespace fs = std::filesystem;
	using FilePath = std::filesystem::path;

	class FileSystem
	{
	public:
		static void MountPath(std::string src, std::string dst);

		static void RecursiveCopyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination);

		static std::filesystem::path GetUserHomeDir();

		static std::string GetProjectName();

		static FilePath ToFsPath(const std::string& str);

		static std::string ToRelativePath(const std::string& fullpath, const std::string& basepath);

		static std::string ToAbsolutePath(const std::string& reativepath, const std::string& basepath);

		static std::string ToEningeAbsolutePath(const std::string& reativepath);

		static std::vector<FilePath> GetFiles(const FilePath& directory)
		{
			std::vector<FilePath> files;
			for (auto const& directory_entry: fs::recursive_directory_iterator{ directory })
			{
				if (directory_entry.is_regular_file())
				{
					files.emplace_back(directory_entry);
				}
			}
			return files;
		}

		static std::string GetCleanFilename(const std::string& filename);

		static void ResolveSlash(std::string& str);

		static std::string JoinFileRoute(const FilePath& directory, const std::string& fileRoute);

		static std::string JoinPath(const std::string& lhs, const std::string& rhs);

		template<typename... Args>
		static std::string JoinPaths(const std::string& path, Args... args);

		static std::string GenerateRandomUUID();

		static bool PathExists(const std::string& filename);

		static bool CreateDirectory(const std::string& dir_name);
		static bool CreateDirectory(const std::filesystem::path& dir_name);

		static bool CopyFile(const std::string& src, const std::string& dest);
	};

}

namespace SPW
{
	template<typename... Args>
	std::string FileSystem::JoinPaths(const std::string& path, Args... args)
	{
		std::string result = path;

		((result = FileSystem::JoinPath(result, std::forward<Args>(args))), ...);
		return result;
	}
}