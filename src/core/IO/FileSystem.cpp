#include "IO/FileSystem.h"

namespace SPW
{
	std::vector<FilePath> FileSystem::GetFiles(const FilePath& directory)
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


	std::string FileSystem::JoinFileRoute(const FilePath& directory, const std::string& fileRoute)
	{
		return directory.string().append("/").append(fileRoute);
	}
}
