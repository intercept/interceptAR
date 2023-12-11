#pragma once

using namespace std::string_view_literals;

#ifdef INTERCEPT_NO_MODULE
#define I_INLINE inline
#else
#define I_INLINE
#endif



export namespace Util {
    I_INLINE std::vector<std::string_view>& split(std::string_view s, char delim, std::vector<std::string_view>& elems) {
        std::string::size_type lastPos = 0;
        const std::string::size_type length = s.length();

        while (lastPos < length + 1) {
            std::string::size_type pos = s.find_first_of(delim, lastPos);
            if (pos == std::string::npos) {
                pos = length;
            }

            // if (pos != lastPos || !trimEmpty)
            elems.emplace_back(s.data() + lastPos, pos - lastPos);

            lastPos = pos + 1;
        }

        return elems;
    }


    I_INLINE std::vector<std::string_view> split(std::string_view s, char delim) {
        std::vector<std::string_view> elems;
        split(s, delim, elems);
        return elems;
    }

    I_INLINE std::string_view trim(std::string_view string, std::string_view trimChars) {
        if (string.empty())
            return "";

        auto begin = string.find_first_not_of(trimChars);
        auto end = string.find_last_not_of(trimChars);
        if (begin == std::string::npos)
            return std::string_view();
        if (end == std::string::npos)
            end = string.length() - 1;
        return string.substr(begin, end - begin + 1);
    }


    I_INLINE std::string_view trim(std::string_view string) {
        /*
         * Trims tabs and spaces on either side of the string.
         */
        if (string.empty())
            return "";

        return trim(string, "\t "sv);
    }

    I_INLINE std::string toLower(std::string_view string) {
        std::string ret(string);
        std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);

        return ret;
    }

    I_INLINE bool stringEqualsCaseInsensitive(std::string_view l, std::string_view r) {
        if (l.length() != r.length())
            return false;

        return std::equal(l.cbegin(), l.cend(),
                          r.cbegin(), r.cend(),
                          [](unsigned char l, unsigned char r) {
                              return l == r || tolower(l) == tolower(r);
                          });
    }

    I_INLINE float parseArmaNumber(std::string_view armaNumber) {
        return static_cast<float>(std::strtof(armaNumber.data(), nullptr));
    }
    I_INLINE int parseArmaNumberToInt(std::string_view armaNumber) {
        return static_cast<int>(std::round(parseArmaNumber(armaNumber)));
    }
    I_INLINE int64_t parseArmaNumberToInt64(std::string_view armaNumber) {
        int64_t ret;
        std::from_chars(armaNumber.data(), armaNumber.data() + armaNumber.length(), ret);
        // #TODO error checking?
        return ret;
    }

    I_INLINE bool isTrue(std::string_view string) {
        if (string.length() != 4) // small speed optimization
            return string.length() == 1 && string.at(0) == '1';
        return string == "true";
    }


    struct StringHash {
        using is_transparent = void;
        [[nodiscard]] size_t operator()(std::string_view txt) const { return std::hash<std::string_view>{}(txt); }
        [[nodiscard]] size_t operator()(const std::string& txt) const { return std::hash<std::string>{}(txt); }
    };

    I_INLINE std::filesystem::path GetCurrentDLLPath() {
#ifdef _WIN32
        char path[MAX_PATH];
        HMODULE hm = NULL;

        if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                  GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              (LPCSTR)&GetCurrentDLLPath, &hm) == 0) {
            int ret = GetLastError();
            fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
            // Return or however you want to handle an error.
        }
        if (GetModuleFileName(hm, path, sizeof(path)) == 0) {
            int ret = GetLastError();
            fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
            // Return or however you want to handle an error.
        }

        return std::filesystem::path(path);
#else
        raise(SIGTRAP);
        return {};
#endif
    }

    I_INLINE std::wstring UTF8ToUTF16(std::string_view input) {
#ifdef _WIN32
        const auto wantedSize = MultiByteToWideChar(CP_UTF8, 0, input.data(), input.length(), nullptr, 0);
        std::wstring result;
        result.resize(wantedSize);
        MultiByteToWideChar(CP_UTF8, 0, input.data(), input.length(), result.data(), result.length());
        return result;
#else
        raise(SIGTRAP);
        return {};
#endif
    }

    I_INLINE std::string UTF16ToUTF8(std::wstring_view input) {
#ifdef _WIN32
        const auto wantedSize = WideCharToMultiByte(CP_UTF8, 0, input.data(), input.length(), nullptr, 0, nullptr, nullptr);
        std::string result;
        result.resize(wantedSize);
        WideCharToMultiByte(CP_UTF8, 0, input.data(), input.length(), result.data(), result.length(), nullptr, nullptr);
        return result;
#else
        raise(SIGTRAP);
        return {};
#endif
    }


    I_INLINE void* GetArmaHostProcAddress(std::string name) {
#ifdef _WIN32
        return GetProcAddress(GetModuleHandleA(nullptr), name.data());
#else
        raise(SIGTRAP);
        return nullptr;
#endif
    }

    I_INLINE bool IsDebuggerPresent() {
#ifdef _WIN32
        return ::IsDebuggerPresent();
#else
        raise(SIGTRAP);
        return false;
#endif
    }

   I_INLINE  void BreakToDebuggerIfPresent() {
#ifdef _WIN32
        if (::IsDebuggerPresent())
            __debugbreak();
#else
        raise(SIGTRAP);
#endif
    }

    I_INLINE void WaitForDebuggerSilent() {
#ifdef _WIN32
        if (IsDebuggerPresent())
            return;

        while (!IsDebuggerPresent())
            Sleep(10);

        // We waited for attach, so break us there
        __debugbreak();
#else
        raise(SIGTRAP);
#endif
    }

    I_INLINE void WaitForDebuggerPrompt() {
#ifdef _WIN32
        if (IsDebuggerPresent())
            return;

        while (!IsDebuggerPresent())
            MessageBoxA(0, "Intercept Waiting for debugger!", "Waiting for debugger!", 0);

        // We waited for attach, so break us there
        __debugbreak();
#else
        raise(SIGTRAP);
#endif
    }


    I_INLINE void PrintDebugString(std::string message) {
#ifdef _WIN32
        OutputDebugStringA(message.data());
        OutputDebugStringA("\n");
#else
        raise(SIGTRAP);
#endif
    }
}; // namespace Util

export template <unsigned N>
struct FixedString {
    std::array<char, N+1> buf{};
    constexpr FixedString(const char* s) {
        for (unsigned i = 0; i != N; ++i)
            buf[i] = s[i];
    }
    constexpr operator char const*() const { return buf.data(); }
    constexpr size_t size() const { return N; }

    auto operator<=>(const FixedString&) const = default;
};
export template <unsigned N>
FixedString(char const (&)[N]) -> FixedString<N - 1>;

#undef I_INLINE






