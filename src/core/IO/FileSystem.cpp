//
// Created by dudu on 2023/4/15.
//
#include "FileSystem.h"

namespace SPW
{

	void FileSystem::Boost()
	{
		// if(!root.empty())
		//     k_Root = "./Project/";
		// else
		//     k_Root = root;
		// k_Engine = k_Root  + "/Engine/";
		// k_Assets = k_Root  + "/Assets/";
		// k_Scenes = k_Root  + "/Scenes/";

		// create engine root
		CreateDirectory(k_Root);
		// copy engine files into engine
		CreateDirectory(k_Engine);
		// copy project asset/ resource,
		CreateDirectory(k_Assets);
		CreateDirectory(k_Scenes);
		MountEngine();
	}
	void FileSystem::MountEngine()
	{
		try
		{
			for (const auto& entry: std::filesystem::directory_iterator("./resources/"))
			{
				const auto& path = entry.path();
				auto new_destination = k_Engine / path.filename();

				if (fs::is_directory(path))
				{
					copy_directory(path, new_destination);
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
	void FileSystem::copy_directory(const std::filesystem::path& source, const std::filesystem::path& destination)
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
					copy_directory(path, new_destination);
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
	std::string FileSystem::GetProjectName()
	{
		return fs::path(k_Root).filename().string();
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
	std::string FileSystem::ToAbsolutePath(const std::string& reativepath)
	{
		return JoinPaths(k_Root, reativepath);
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
	bool FileSystem::CreateDirectory(const std::string& dir_name)
	{
		std::error_code err;
		if (!fs::create_directories(dir_name, err))
		{
			if (fs::exists(dir_name))
			{
				std::cout << " the folder probably already existed\n ";
				return true;    // the folder probably already existed
			}

			std::cout << "CreateDirectoryRecursive: FAILED to create" <<
					  dir_name.c_str() << "err" << err.message().c_str() << "\n";

			return false;
		}
		return true;
	}
	void FileSystem::CopyFile(const std::string& src, const std::string& dest)
	{
		try
		{
			fs::copy_file(fs::path(src), fs::path(dest), std::filesystem::copy_options::overwrite_existing);
			std::cout << "File copied successfully from " << src << " to " << dest << std::endl;
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
}