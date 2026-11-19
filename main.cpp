
#include <string>
#include <stdexcept>
#include <cctype>
#include <cstring>

#include <emscripten.h>
#include <emscripten/fetch.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Send successful translations to the frontend.
EM_JS(void, notifySuccess, (const char *translation), {
    if (window.onCppTranslationSuccess) {
        window.onCppTranslationSuccess(UTF8ToString(translation));
    }
});

// Send errors to the frontend.
EM_JS(void, notifyError, (const char *message), {
    if (window.onCppTranslationError) {
        window.onCppTranslationError(UTF8ToString(message));
    }
});

// URL-encode the input text.
string urlEncode(const string &value)
{
    const char hex[] = "0123456789ABCDEF";
    string encoded;

    for (unsigned char c : value)
    {
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' ||
            c == '.' || c == '~')
        {
            encoded += static_cast<char>(c);
        }
        else
        {
            encoded += '%';
            encoded += hex[c >> 4];
            encoded += hex[c & 15];
        }
    }

    return encoded;
}

// Handle a successful HTTP response.
void onFetchSuccess(emscripten_fetch_t *fetch)
{
    try
    {
        if (fetch->status != 200)
        {
            throw runtime_error(
                "MyMemory HTTP error: " + to_string(fetch->status));
        }

        string response(fetch->data, fetch->numBytes);
        json data = json::parse(response);

        if (!data.contains("responseStatus") ||
            data["responseStatus"].get<int>() != 200)
        {
            string details = "Translation failed.";

            if (data.contains("responseDetails") &&
                data["responseDetails"].is_string())
            {
                details = data["responseDetails"].get<string>();
            }

            throw runtime_error(details);
        }

        if (!data.contains("responseData") ||
            !data["responseData"].contains("translatedText") ||
            !data["responseData"]["translatedText"].is_string())
        {
            throw runtime_error("Invalid translation response.");
        }

        string translation =
            data["responseData"]["translatedText"].get<string>();

        notifySuccess(translation.c_str());
    }
    catch (const exception &e)
    {
        notifyError(e.what());
    }

    emscripten_fetch_close(fetch);
}

// Handle a failed HTTP request.
void onFetchError(emscripten_fetch_t *fetch)
{
    string message = "Could not contact MyMemory.";

    if (fetch->status != 0)
    {
        message += " HTTP status: " + to_string(fetch->status);
    }

    notifyError(message.c_str());

    emscripten_fetch_close(fetch);
}

// Exported C++ function callable from JavaScript.
extern "C"
{
    EMSCRIPTEN_KEEPALIVE
    void translate(const char *inputText,
                   const char *source,
                   const char *target)
    {
        try
        {
            if (!inputText || !source || !target)
            {
                throw runtime_error("Invalid translation request.");
            }

            string text = inputText;
            string sourceLanguage = source;
            string targetLanguage = target;

            if (text.empty())
            {
                throw runtime_error("Please enter some text.");
            }

            // MyMemory documents a maximum of 500 bytes for q.
            if (text.size() > 500)
            {
                throw runtime_error(
                    "Text exceeds MyMemory's 500-byte limit.");
            }

            if (!((sourceLanguage == "en" && targetLanguage == "fa") ||
                  (sourceLanguage == "fa" && targetLanguage == "en")))
            {
                throw runtime_error("Unsupported language pair.");
            }

            string url =
                "https://api.mymemory.translated.net/get?q=" +
                urlEncode(text) +
                "&langpair=" +
                sourceLanguage + "%7C" + targetLanguage;

            emscripten_fetch_attr_t attributes;
            emscripten_fetch_attr_init(&attributes);

            // requestMethod is a fixed-size character array.
            strncpy(attributes.requestMethod, "GET",
                    sizeof(attributes.requestMethod) - 1);

            attributes.requestMethod[
                sizeof(attributes.requestMethod) - 1
            ] = '\0';

            attributes.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
            attributes.onsuccess = onFetchSuccess;
            attributes.onerror = onFetchError;
            attributes.timeoutMSecs = 30000;

            emscripten_fetch(&attributes, url.c_str());
        }
        catch (const exception &e)
        {
            notifyError(e.what());
        }
    }
}
