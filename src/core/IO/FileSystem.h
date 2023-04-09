#pragma once

#include <filesystem>
#include <vector>
#include "Utils/UUID.hpp"

namespace SPW
{
	namespace fs = std::filesystem;
	using FilePath = std::filesystem::path;

        struct FileRoot
        {
          std::string engine;
          std::string resource;
          std::string asset;
        };

	class FileSystem
	{
            public:
                static FilePath ToFsPath(const std::string& filename)
                {
                    return FilePath(filename);
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
