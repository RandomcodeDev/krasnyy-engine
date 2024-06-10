#include "framework/Log.h"
#include "framework/Util.h"

#include "CWindowsPlatform.h"
#include "CWindowsSharedLibrary.h"

void CWindowsPlatform::Initialize()
{
#ifndef KR_PLATFORM_XBOX
	bool haveConsole = AttachConsole(ATTACH_PARENT_PROCESS);
#if defined KR_DEBUG || defined KR_RELWITHDEBINFO
	if (haveConsole || (!haveConsole && AllocConsole()))
#else
	if (haveConsole)
#endif
	{
		KR_LOG_INFO("Attaching to console");

		FILE* file;
		freopen_s(&file, "CONIN$", "r", stdin);
		freopen_s(&file, "CONOUT$", "w", stdout);
		freopen_s(&file, "CONOUT$", "w", stderr);

		// bump first message away from prompt in case running in cmd
		printf("\n");
	}
#endif

#ifdef KR_PLATFORM_GDK
	KR_LOG_INFO("Initializing Xbox gaming runtime");
	HRESULT result = XGameRuntimeInitialize();
	if (!SUCCEEDED(result))
	{
		KR_QUIT("Failed to initialize gaming runtime: {}", GetHresultString(result));
	}
#endif
}

void CWindowsPlatform::Shutdown()
{
#ifdef KR_PLATFORM_GDK
	XGameRuntimeUninitialize();
#endif
}

const std::string& CWindowsPlatform::DescribeOs()
{
	static std::string desc;

	// This shouldn't ever change between runs
	if (desc.length() > 0)
	{
		return desc;
	}

	// All versions
	HKEY currentVersionKey;
	CHAR rawEdition[32] = {};
	CHAR product[32] = {};
	DWORD size = 0;

	// Windows 10 and above
	CHAR installationType[32] = {};
	DWORD majorVersion = 0;
	DWORD minorVersion = 0;
	CHAR buildNumber[8] = {};
	DWORD ubr = 0;
	CHAR displayVersion[8] = {};
	CHAR buildLabExtended[64] = {};

	// Windows 8 and below
	CHAR csdVersion[8] = {};
	CHAR buildLab[64] = {};

	BOOL isWow64 = FALSE;
#ifdef _M_IX86
	IsWow64Process(INVALID_HANDLE_VALUE, &isWow64);
#endif

	RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE, &currentVersionKey);

	size = sizeof(rawEdition);
	RegQueryValueExA(currentVersionKey, "EditionID", nullptr, nullptr, (LPBYTE)rawEdition, &size);

	size = sizeof(product);
	RegQueryValueExA(currentVersionKey, "ProductName", nullptr, nullptr, (LPBYTE)product, &size);

	size = sizeof(INT);
	if (RegQueryValueExA(currentVersionKey, "CurrentMajorVersionNumber", nullptr, nullptr, (LPBYTE)&majorVersion, &size) == ERROR_SUCCESS)
	{
		size = sizeof(INT);
		RegQueryValueExA(currentVersionKey, "CurrentMinorVersionNumber", nullptr, nullptr, (LPBYTE)&minorVersion, &size);

		size = sizeof(installationType);
		RegQueryValueExA(currentVersionKey, "InstallationType", nullptr, nullptr, (LPBYTE)installationType, &size);

		size = sizeof(buildNumber);
		RegQueryValueExA(currentVersionKey, "CurrentBuildNumber", nullptr, nullptr, (LPBYTE)buildNumber, &size);

		size = sizeof(INT);
		RegQueryValueExA(currentVersionKey, "UBR", nullptr, nullptr, (LPBYTE)&ubr, &size);

		size = sizeof(buildLabExtended);
		RegQueryValueExA(currentVersionKey, "BuildLabEx", nullptr, nullptr, (LPBYTE)&buildLabExtended, &size);

		size = sizeof(displayVersion);
		RegQueryValueExA(currentVersionKey, "DisplayVersion", nullptr, nullptr, (LPBYTE)displayVersion, &size);

		std::string edition(rawEdition, std::min(strlen(rawEdition), KR_ARRAY_SIZE(rawEdition)));
		desc = fmt::format(
#ifdef KR_DEBUG
			"{} {} {}.{}.{} {}{}",
#else
			"{} {} {}.{}.{}.{} {}{}",
#endif
			edition == "SystemOS" ? "Xbox System Software" : "Windows",
			(strncmp(installationType, "Client", KR_ARRAY_SIZE(installationType)) == 0) ? "Desktop" : installationType, majorVersion, minorVersion,
#ifdef KR_DEBUG
			buildLabExtended, edition, isWow64 ? " (WoW64)" : ""
#else
			currentBuild, ubr, edition, isWow64 ? " (WoW64)" : ""
#endif
		);
	}
	else
	{
		size = sizeof(csdVersion);
		RegQueryValueExA(currentVersionKey, "CSDVersion", nullptr, nullptr, (LPBYTE)buildLab, &size);
		size = sizeof(buildLab);
		RegQueryValueExA(currentVersionKey, "BuildLab", nullptr, nullptr, (LPBYTE)buildLab, &size);

		desc = fmt::format("Windows {} {} {} (build lab {}{})", product, rawEdition, csdVersion, buildLab, isWow64 ? ", WoW64" : "");
	}
	return desc;
}

const std::string& CWindowsPlatform::DescribeHardware()
{
	static std::string desc;
	return desc;
}

const std::string& CWindowsPlatform::GetUserDataPath()
{
	static std::string appData;

	if (appData.length() > 0)
	{
		return appData;
	}

	char path[MAX_PATH + 1] = {0};

	HRESULT result = SHGetFolderPathA(nullptr, CSIDL_APPDATA | CSIDL_FLAG_CREATE, nullptr, SHGFP_TYPE_CURRENT, path);
	if (!SUCCEEDED(result))
	{
		KR_LOG_WARN("Failed to get AppData path: {}", GetHresultString(result));
		appData = "C:/Temp/";
		return appData;
	}

	appData = path;
	Replace(appData, "\\", "/");
	appData += '/';

	return appData;
}

[[noreturn]] void CWindowsPlatform::Quit(
	const std::string& message, bool useLastError, const std::string& file, const std::string& function, int line)
{
	std::string fullMessage = fmt::format("Fatal error in {} at {}:{}: {}", function, file, line, message);
	KR_LOG_FATAL(fullMessage);
	MessageBoxA(nullptr, fullMessage.c_str(), "FATAL ERROR", MB_ICONERROR | MB_ABORTRETRYIGNORE);
	std::exit(HRESULT_FROM_WIN32(useLastError ? GetLastError() : ERROR_FATAL_APP_EXIT));
}

std::shared_ptr<ISharedLibrary> CWindowsPlatform::LoadLibrary(const std::string& name, const std::vector<std::string>& paths)
{
	std::string fullName = name + ".dll";

	if (paths.empty())
	{
		HMODULE handle = LoadLibraryA((fullName).c_str());
		if (handle)
		{
			return std::make_unique<CWindowsSharedLibrary>(name, handle);
		}
	}
	else
	{
		for (const auto& searchPath : paths)
		{
			HMODULE handle = LoadLibraryExA(fmt::format("{}/{}", searchPath, fullName).c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
			if (handle)
			{
				return std::make_unique<CWindowsSharedLibrary>(name, handle);
			}
		}
	}

	return nullptr;
}

bool CWindowsPlatform::CreateDirectory(const std::string& name)
{
	size_t offset = 0;
	while ((offset = name.find('/', offset)))
	{
		std::string current = name.substr(0, offset);
		if (!CreateDirectoryA(current.c_str(), NULL))
		{
			if (GetLastError() != ERROR_FILE_EXISTS)
			{
				return false;
			}
		}
	}

	return true;
}

// static so that most functions can be used whenever (mainly Quit)
static CWindowsPlatform s_platform;
FRAMEWORK_API IPlatform* GetPlatform()
{
	return &s_platform;
}
