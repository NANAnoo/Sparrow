#pragma once

#include <filesystem>

namespace SPW
{
	namespace fs = std::filesystem;
	using FilePath = std::filesystem::path;

	class FileSystem
	{
	public:
		std::vector<FilePath> GetFiles(const FilePath& directory);
		std::string JoinFileRoute(const FilePath& directory, const std::string& fileRoute);
	};

}
