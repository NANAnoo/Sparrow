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
		struct PathStruct
		{
			std::string path;
			std::string name;
			std::string ext;
			bool isOk = false;
		};

	public:
		static void MountPath(std::string src, std::string dst);

		static void RecursiveCopyDirectory(const std::filesystem::path& source, const std::filesystem::path& destination);

		static std::filesystem::path GetUserHomeDir();

		static FilePath ToFsPath(const std::string& str);

		static std::string ToRelativePath(const std::string& fullpath, const std::string& basepath);

		static std::string ToAbsolutePath(const std::string& reativepath, const std::string& basepath);

		static std::string ToEningeAbsolutePath(const std::string& reativepath);

		static std::vector<FilePath> GetFiles(const FilePath& directory);

		static std::string GetSystemTime();

		static std::string CharStarToString(char* array);

		static std::string GetCleanFilename(const std::string& filename);

		static void ResolveSlash(std::string& str);

		static std::string JoinFileRoute(const FilePath& directory, const std::string& fileRoute);

		static std::string JoinPath(const std::string& lhs, const std::string& rhs);

		template<typename... Args>
		static std::string JoinPaths(const std::string& path, Args... args);

		static std::string GenerateRandomUUID();

		static bool PathExists(const std::string& filename);

		static bool SPWCreateDirectory(const std::string& dir_name);
		static bool SPWCreateDirectory(const std::filesystem::path& dir_name);

		static bool SPWCopyFile(const std::string& src, const std::string& dest);

		static bool IsDirectoryCanBeOpened(const std::string& name);

		static bool IsDirectoryExist(const std::string& name)
		{
			bool bExists = false;

			if (!name.empty())
			{
				namespace fs = std::filesystem;
				fs::path pathName = fs::path(name);
				bExists = fs::is_directory(pathName);
			}

			return bExists;    // this is not a directory!
		}

		static bool CreateDirectoryIfNotExist(const std::string& name)
		{
			bool res = false;

			if (!name.empty())
			{
				if (!IsDirectoryExist(name))
				{
					namespace fs = std::filesystem;
					fs::path pathName = fs::path(name);

					std::error_code err;
					if(!fs::create_directory(pathName, err))
						std::cout << "Error creating directory " << name << std::endl;
				}
			}

			return res;
		}

		static PathStruct ParsePathFileName(const std::string& vPathFileName)
		{
			namespace fs = std::filesystem;
			SPW::FileSystem::PathStruct res;
			if (vPathFileName.empty())
				return res;

			auto fsPath = fs::path(vPathFileName);

			if (fs::is_directory(fsPath)) {
				res.name = "";
				res.path = fsPath.string();
				res.isOk = true;

			}
			else if (fs::is_regular_file(fsPath)) {
				res.name = fsPath.filename().string();
				res.path = fsPath.parent_path().string();
				res.isOk = true;
			}

			return res;
		}

		static bool ReplaceString(std::string& str, const std::string& oldStr, const std::string& newStr)
		{
			bool found = false;
			size_t pos = 0;
			while ((pos = str.find(oldStr, pos)) != std::string::npos)
			{
				found = true;
				str.replace(pos, oldStr.length(), newStr);
				pos += newStr.length();
			}
			return found;
		}

		static std::vector<std::string> SplitStringToVector(const std::string& text, char delimiter, bool pushEmpty)
		{
			std::vector<std::string> arr;
			if (!text.empty())
			{
				size_t start = 0;
				size_t end = text.find(delimiter, start);
				while (end != std::string::npos)
				{
					auto token = text.substr(start, end - start);
					if (!token.empty() || (token.empty() && pushEmpty)) //-V728
						arr.push_back(token);
					start = end + 1;
					end = text.find(delimiter, start);
				}
				auto token = text.substr(start);
				if (!token.empty() || (token.empty() && pushEmpty)) //-V728
					arr.push_back(token);
			}
			return arr;
		}

		static void AppendToBuffer(char* vBuffer, size_t vBufferLen, const std::string& vStr)
		{
			std::string st = vStr;
			size_t len = vBufferLen - 1u;
			size_t slen = strlen(vBuffer);

			if (!st.empty() && st != "\n")
			{
				ReplaceString(st, "\n", "");
				ReplaceString(st, "\r", "");
			}
			vBuffer[slen] = '\0';
			std::string str = std::string(vBuffer);
			//if (!str.empty()) str += "\n";
			str += vStr;
			if (len > str.size()) len = str.size();
#ifdef _MSC_VER
			strncpy_s(vBuffer, vBufferLen, str.c_str(), len);
#else // _MSC_VER
			strncpy(vBuffer, str.c_str(), len);
#endif // _MSC_VER
			vBuffer[len] = '\0';
		}

		static void ResetBuffer(char* vBuffer)
		{
			vBuffer[0] = '\0';
		}

		static void SetBuffer(char* vBuffer, size_t vBufferLen, const std::string& vStr)
		{
			ResetBuffer(vBuffer);
			AppendToBuffer(vBuffer, vBufferLen, vStr);
		}

		static std::string LowerCaseString(const std::string& vString)
		{
			auto str = vString;

			// convert to lower case
			for (char& c : str)
				c = (char)std::tolower(c);

			return str;
		}


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