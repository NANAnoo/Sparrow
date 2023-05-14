//
// Created by dudu on 2023/4/15.
//
#include "FileSystem.h"
#include "ConfigManager.h"

#include <iostream>
#include <Windows.h>
#include <ShlObj.h>
#include <tchar.h>
#include <fstream>
#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>

#ifdef CreateDirectory
#undef CreateDirectory
#endif

#ifdef CopyFile
#undef CopyFile
#endif

namespace SPW
{

	void FileSystem::MountPath(std::string src, std::string dst)
	{
		try
		{
			SPWCreateDirectory(dst);
			for (const auto& entry : std::filesystem::directory_iterator(src))
			{
				const auto& path = entry.path();
				auto new_destination = dst / path.filename();

				if (fs::is_directory(path))
				{
					RecursiveCopyDirectory(path, new_destination);
				}
				else
				{
					fs::copy(path, new_destination);
				}
			}
		}
		catch (std::exception& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}


	std::filesystem::path FileSystem::GetUserHomeDir()
	{
		TCHAR homeDirectory[MAX_PATH];
		HRESULT result = SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, homeDirectory);

		if (result == S_OK)
		{
			return std::filesystem::u8path(std::string(homeDirectory));
		}
		else
		{
			std::cerr << "Error: Unable to retrieve the user home directory." << std::endl;
			return std::string();
		}
	}

	void FileSystem::RecursiveCopyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination)
	{
		try
		{
			std::filesystem::create_directories(destination);

			for (const auto& entry: std::filesystem::directory_iterator(source))
			{
				const auto& path = entry.path();
				auto new_destination = destination / path.filename();

				if (std::filesystem::is_directory(path))
				{
					RecursiveCopyDirectory(path, new_destination);
				}
				else
				{
					std::filesystem::copy(path, new_destination);
				}
			}
		}
		catch (std::exception& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}


	FilePath FileSystem::ToFsPath(const std::string& str)
	{
		return str;
	}


	std::string FileSystem::ToRelativePath(const std::string& fullpath, const std::string& basepath)
	{
		std::string ret = fs::relative(fullpath, basepath).string();
		ResolveSlash(ret);
		return ret;
	}
	std::string FileSystem::ToAbsolutePath(const std::string& reativepath, const std::string& basepath)
	{
		return JoinPaths(basepath, reativepath);
	}

	std::string FileSystem::ToEningeAbsolutePath(const std::string& reativepath)
	{
		return JoinPaths(Config::k_EngineRoot, reativepath);
	}

	std::vector<FilePath> FileSystem::GetFiles(const FilePath& directory)
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

	std::string FileSystem::GetSystemTime()
	{
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);

		std::tm buf = *std::localtime(&in_time_t);

		std::ostringstream os;
		os << std::put_time(&buf, "%Y-%m-%d-%H-%M-%S");
		// std::string timestr = os.str();

		return os.str();
	}

	std::string FileSystem::GetCleanFilename(const std::string& filename)
	{
		return fs::path(filename).filename().replace_extension().string();
	}

	void FileSystem::ResolveSlash(std::string& str)
	{
		std::replace(begin(str), end(str), '\\', '/');
	}

	std::string FileSystem::JoinFileRoute(const FilePath& directory, const std::string& fileRoute)
	{
		std::string ret = directory.string().append("/").append(fileRoute);
		ResolveSlash(ret);
		return ret;
	}

	std::string FileSystem::JoinPath(const std::string& lhs, const std::string& rhs)
	{
		std::string ret = lhs;

		if (!ret.empty() && ret.back() != '/')
		{
			ret += '/';
		}

		ret += rhs;
		return ret;
	}

	std::string FileSystem::GenerateRandomUUID()
	{
		return UUID::randomUUID().toString();
	}

	bool FileSystem::PathExists(const std::string& filename)
	{
		return fs::exists(filename) ? true : false;
	}

	bool FileSystem::SPWCreateDirectory(const std::string& dir_name)
	{
		// Check if the directory already exists.
		if (fs::exists(dir_name))
		{
			std::cout << "The directory " << dir_name << " already exists.\n";
			return true;
		}

		// Attempt to create the directory and handle any errors.
		std::error_code err;
		if (!fs::create_directories(dir_name, err))
		{
			std::cout << "SPWCreateDirectory: FAILED to create " << dir_name
				<< " err: " << err.message() << "\n";
			return false;
		}

		// Directory created successfully.
		return true;
	}

	bool FileSystem::SPWCreateDirectory(const std::filesystem::path& dir_name)
	{
		// Check if the directory already exists.
		if (fs::exists(dir_name))
		{
			std::cout << "The directory " << dir_name << " already exists.\n";
			return true;
		}

		// Attempt to create the directory and handle any errors.
		std::error_code err;
		if (!fs::create_directories(dir_name, err))
		{
			std::cout << "SPWCreateDirectory: FAILED to create " << dir_name
				<< " err: " << err.message() << "\n";
			return false;
		}

		// Directory created successfully.
		return true;
	}

	bool FileSystem::CopyFile(const std::string& src, const std::string& dest)
	{
		try
		{
			// Check if the destination file already exists
			if (fs::exists(dest))
			{
				std::cout << "File " << dest << " already exists. Skipping copy operation.\n";
				return true;
			}
			// Copy the source file to the destination
			fs::copy_file(fs::path(src), fs::path(dest), std::filesystem::copy_options::overwrite_existing);
			std::cout << "File copied successfully from " << src << " to " << dest << std::endl;
			return true;
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
			return false;
		}
	}
}