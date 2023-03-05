#pragma once

#include <filesystem>

namespace SPW
{
	namespace fs = std::filesystem;
	using FilePath = std::filesystem::path;

	inline namespace FileSystem
	{
		static std::vector<FilePath> GetFiles(const FilePath& directory)
		{
			std::vector<FilePath> files;
			for (auto const& directory_entry : fs::recursive_directory_iterator{ directory })
			{
				if (directory_entry.is_regular_file())
				{
					files.emplace_back(directory_entry);
				}
			}
			return files;
		}

		static std::string JoinFileRoute(const FilePath& directory, const std::string& fileRoute)
		{
			return directory.string().append("/").append(fileRoute);
		}

	}

}
