#include <iostream>
#include <curl/curl.h>
#include <json/json.h>
#include <sstream>
#include <chrono>
#include <thread>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(reinterpret_cast<char*>(contents), total_size);
    return total_size;
}

std::string sendRequest(const std::string& assetId) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return "Failed to initialize curl";
    }

    std::string url = "https://thumbnails.roblox.com/v1/assets?assetIds=" + assetId +
        "&returnPolicy=PlaceHolder&size=420x420&format=Png&isCircular=false";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return "Failed to perform request: " + std::string(curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);

    Json::CharReaderBuilder builder;
    Json::CharReader* jsonReader = builder.newCharReader();
    Json::Value root;
    std::istringstream jsonStream(response);

    if (!Json::parseFromStream(builder, jsonStream, &root, nullptr)) {
        return "Failed to parse JSON";
    }

    const std::string state = root["data"][0]["state"].asString();
    if (state == "Completed") {
        return "Completed";
    }
    else if (state == "Blocked") {
        return "Blocked";
    }
    else {
        return "Unknown state: " + state;
    }
}

int main() {
    std::string assetId;
    std::cout << "Enter your ID: ";
    std::cin >> assetId;

    std::string result = sendRequest(assetId);
    std::cout << "Result: " << result << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}
