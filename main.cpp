#include <filesystem>
#include <vector>
#include <string>

#include "crow_all.h"

using std::vector;
using std::string;
using crow::json::wvalue;

namespace fs = std::filesystem;

const vector<string> extensions = { ".avi", ".mkv", ".mp4" };

// Full file paths for videos
vector<string> fileNames;

// Just the file names
wvalue::list jsonList;

bool updateFileList(string directory) {
    
    fileNames.clear();
    jsonList.clear();

    if (fs::is_directory(directory))
    {

        for (const auto& entry : fs::recursive_directory_iterator(directory))
        {
            for (string extension : extensions) {
                if (entry.is_regular_file() && entry.path().extension() == extension) {
                    fileNames.push_back(entry.path().string());
                    break;
                }
            }
        }

        //std::sort(fileNames.begin(), fileNames.end());
        for (string fileName : fileNames) {
            jsonList.push_back(fileName.substr(fileName.find_last_of("\\") + 1));
        }

        return true;
    }
    else {
        return false;
    }
}

int main() {
	crow::SimpleApp app;

	std::mutex mtx;
	std::unordered_set<crow::websocket::connection*> users;

	CROW_ROUTE(app, "/")([]() {
		
		auto page = crow::mustache::load_text("index.html");
		return page;

		});

    CROW_ROUTE(app, "/json")
        ([] {
        crow::json::wvalue x({ {"file list", jsonList} });
        return x;
            });

	CROW_WEBSOCKET_ROUTE(app, "/ws")
		.onopen([&](crow::websocket::connection& conn) {
		    CROW_LOG_INFO << "new websocket connection from " << conn.get_remote_ip();
		    std::lock_guard<std::mutex> _(mtx);
		    users.insert(&conn);
            conn.send_text(wvalue({ {"type", "file list"}, {"list" , jsonList} }).dump());
		})

		.onclose([&](crow::websocket::connection& conn, const std::string& reason, uint16_t) {
		    CROW_LOG_INFO << "websocket connection closed: " << reason;
		    std::lock_guard<std::mutex> _(mtx);
		    users.erase(&conn);
		})

		.onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary) {
		    std::lock_guard<std::mutex> _(mtx);

            string broadcast = "";

            if (data == "play/pause") {
                broadcast = wvalue({ {"type", "pause"} }).dump();
            }

            for (auto u : users) {

            }

		});


    if (!updateFileList("C:\\Users\\Aidan\\Downloads\\Torrents")) {
        CROW_LOG_ERROR << "Invalid directory";
    }

	app.port(3000).multithreaded().run();
}