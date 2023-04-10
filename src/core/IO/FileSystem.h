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

	inline namespace FileRoots
	{
        extern std::string k_Root;
        extern std::string k_Engine;
        extern std::string k_Assets;
        extern std::string k_Scenes;
	}

	class FileSystem
	{
    public:
		static void Boost()
		{
			// copy engine files into engine
			CreateDirectory(k_Engine);
			// copy project asset/ resource,
			CreateDirectory(k_Assets);
			CreateDirectory(k_Scenes);
		}

		static std::string GetProjectName()
		{
			return fs::path(k_Root).filename().string();
		}

		static FilePath ToFsPath(const std::string& str)
        {
            return str;
        }

        static std::string ToRelativePath(const std::string& fullpath, const std::string& basepath)
		{
            std::string ret = fs::relative(fullpath, basepath).string();
            ResolveSlash(ret);
            return ret;
		}

		static std::string ToAbsolutePath(const std::string& reativepath, const std::string& basepath)
		{
            return JoinPaths(basepath, reativepath);
        }

		static std::string ToAbsolutePath(const std::string& reativepath)
		{
            return JoinPaths(k_Root, reativepath);
        }

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

        static std::string GetCleanFilename(const std::string& filename)
        {
                return fs::path(filename).filename().replace_extension().string();
        }

        static void ResolveSlash(std::string& str)
        {
            std::replace(begin(str), end(str), '\\', '/');
        }

		static std::string JoinFileRoute(const FilePath& directory, const std::string& fileRoute)
		{
            std::string ret = directory.string().append("/").append(fileRoute);
            ResolveSlash(ret);
            return ret;
		}

		static std::string JoinPath(const std::string& lhs, const std::string& rhs)
		{
            std::string ret = lhs;

            if (!ret.empty() && ret.back() != '/') {
                ret += '/';
            }

            ret += rhs;
            return ret;
        }

        template <typename... Args>
        static std::string JoinPaths(const std::string& path, Args... args) {
            std::string result = path;

            ((result = FileSystem::JoinPath(result, std::forward<Args>(args))), ...);
            return result;
        }

        static std::string GenerateRandomUUID()
        {
            return UUID::randomUUID().toString();
        }

        static bool PathExists(const std::string& filename)
        {
            if (fs::exists(filename))
                return true;    // the folder probably already existed
            else
                std::cout << " File Not Exist \n";

            return false;
        }

        static bool CreateDirectory(const std::string& dir_name)
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

        static void CopyFile(const std::string& src, const std::string& dest)
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
	};
}
