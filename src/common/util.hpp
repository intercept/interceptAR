#pragma once

#pragma warning(push)
#pragma warning(disable: 4005) // macro redefinition
import <filesystem>;
import <string_view>;
import <vector>;
//import <Windows.h>;
#pragma warning(pop)

using namespace std::string_view_literals;

export namespace Util {
    std::vector<std::string_view>& split(std::string_view s, char delim, std::vector<std::string_view>& elems) {
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


    std::vector<std::string_view> split(std::string_view s, char delim) {
        std::vector<std::string_view> elems;
        split(s, delim, elems);
        return elems;
    }

    std::string_view trim(std::string_view string, std::string_view trimChars) {
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


    std::string_view trim(std::string_view string) {
        /*
         * Trims tabs and spaces on either side of the string.
         */
        if (string.empty())
            return "";

        return trim(string, "\t "sv);
    }

    std::string toLower(std::string_view string) {
        std::string ret(string);
        std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);

        return ret;
    }

    bool stringEqualsCaseInsensitive(std::string_view l, std::string_view r) {
        if (l.length() != r.length())
            return false;

        return std::equal(l.cbegin(), l.cend(),
                          r.cbegin(), r.cend(),
                          [](unsigned char l, unsigned char r) {
                              return l == r || tolower(l) == tolower(r);
                          });
    }

    float parseArmaNumber(std::string_view armaNumber) {
        return static_cast<float>(std::strtof(armaNumber.data(), nullptr));
    }
    int parseArmaNumberToInt(std::string_view armaNumber) {
        return static_cast<int>(std::round(parseArmaNumber(armaNumber)));
    }
    int64_t parseArmaNumberToInt64(std::string_view armaNumber) {
        int64_t ret;
        std::from_chars(armaNumber.data(), armaNumber.data() + armaNumber.length(), ret);
        // #TODO error checking?
        return ret;
    }

    bool isTrue(std::string_view string) {
        if (string.length() != 4) // small speed optimization
            return string.length() == 1 && string.at(0) == '1';
        return string == "true";
    }


    struct StringHash {
        using is_transparent = void;
        [[nodiscard]] size_t operator()(std::string_view txt) const { return std::hash<std::string_view>{}(txt); }
        [[nodiscard]] size_t operator()(const std::string& txt) const { return std::hash<std::string>{}(txt); }
    };

    std::filesystem::path GetCurrentDLLPath() {
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
    }

    void* GetArmaHostProcAddress(std::string name) {
        return GetProcAddress(GetModuleHandleA(nullptr), name.data());
    }

    bool IsDebuggerPresent() {
        return ::IsDebuggerPresent();
    }

    void BreakToDebuggerIfPresent() {
        if (::IsDebuggerPresent())
            __debugbreak();
    }

    void WaitForDebuggerSilent() {
        if (IsDebuggerPresent())
            return;

        while (!IsDebuggerPresent())
            Sleep(10);

        // We waited for attach, so break us there
        __debugbreak();
    }

    void WaitForDebuggerPrompt() {
        if (IsDebuggerPresent())
            return;

        while (!IsDebuggerPresent())
            MessageBoxA(0, "Intercept Waiting for debugger!", "Waiting for debugger!", 0);

        // We waited for attach, so break us there
        __debugbreak();
    }


    void PrintDebugString(std::string message) {
        OutputDebugStringA(message.data());
        OutputDebugStringA("\n");
    }
}; // namespace Util

export template <unsigned N>
struct FixedString {
    char buf[N + 1]{};
    constexpr FixedString(const char* s) {
        for (unsigned i = 0; i != N; ++i)
            buf[i] = s[i];
    }
    constexpr operator char const*() const { return buf; }

    auto operator<=>(const FixedString&) const = default;
};
export template <unsigned N>
FixedString(char const (&)[N]) -> FixedString<N - 1>;








