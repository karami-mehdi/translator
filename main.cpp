
#include <iostream>
#include <fstream>
#include <string>

#include <httplib.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <stdexcept>

using namespace std;
using json = nlohmann::json;

// Receive HTTP response data
size_t WriteCallback(void *contents, size_t size,
                     size_t nmemb, string *output)
{
    size_t total = size * nmemb;
    output->append(static_cast<char *>(contents), total);
    return total;
}

// MyMemory translation function
string translateText(const string &text,
                     const string &source,
                     const string &target)
{

    CURL *curl = curl_easy_init();

    if (!curl)
        throw runtime_error("Could not initialize CURL.");

    char *encoded = curl_easy_escape(
        curl, text.c_str(), static_cast<int>(text.length()));

    if (!encoded)
    {
        curl_easy_cleanup(curl);
        throw runtime_error("Could not encode input text.");
    }

    string url =
        "https://api.mymemory.translated.net/get?q=" +
        string(encoded) +
        "&langpair=" + source + "%7C" + target;

    curl_free(encoded);

    string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Translator/1.0");

    CURLcode result = curl_easy_perform(curl);

    long status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);

    curl_easy_cleanup(curl);

    if (result != CURLE_OK)
        throw runtime_error(curl_easy_strerror(result));

    if (status != 200)
        throw runtime_error("MyMemory HTTP error: " + to_string(status));

    json data = json::parse(response);

    if (data.value("responseStatus", 0) != 200)
        throw runtime_error(
            data.value("responseDetails", "Translation failed."));

    return data["responseData"]["translatedText"].get<string>();
}

int main()
{

    curl_global_init(CURL_GLOBAL_DEFAULT);

    httplib::Server server;
    const string allowedOrigin = "https://karami-mehdi.github.io";

    server.Options("/translate",
                   [&allowedOrigin](const httplib::Request &,
                                    httplib::Response &res)
                   {
                       res.set_header("Access-Control-Allow-Origin",
                                      allowedOrigin);
                       res.set_header("Access-Control-Allow-Methods",
                                      "POST, OPTIONS");
                       res.set_header("Access-Control-Allow-Headers",
                                      "Content-Type");

                       res.status = 204;
                   });

    server.Post("/translate",
                [&allowedOrigin](const httplib::Request &req,
                                 httplib::Response &res)
                {
                    res.set_header("Access-Control-Allow-Origin", allowedOrigin);
                    res.set_header("Cache-Control", "no-store");

                    try
                    {
                        json request = json::parse(req.body);

                        string text = request.at("text").get<string>();
                        string source = request.at("source").get<string>();
                        string target = request.at("target").get<string>();

                        if (text.empty() || text.size() > 4000)
                        {
                            throw runtime_error(
                                "Please enter between 1 and 4000 bytes of text.");
                        }

                        if (!((source == "en" && target == "fa") ||
                              (source == "fa" && target == "en")))
                        {
                            throw runtime_error("Unsupported language pair.");
                        }

                        string translated =
                            translateText(text, source, target);

                        json result;
                        result["translation"] = translated;

                        res.set_content(
                            result.dump(),
                            "application/json; charset=utf-8");
                    }
                    catch (const exception &e)
                    {

                        res.status = 400;

                        json error;
                        error["error"] = e.what();

                        res.set_content(
                            error.dump(),
                            "application/json; charset=utf-8");
                    }
                });

    const char *portEnv = std::getenv("PORT");
    int port = portEnv ? std::stoi(portEnv) : 8080;

    cout << "Server listening on port: " << port << endl;

    bool started = server.listen("0.0.0.0", port);

    curl_global_cleanup();

    return started ? 0 : 1;
}
