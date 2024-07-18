#include "../../../include/webserv.hpp"

http_methods::http_methods() {}

http_methods::http_methods(const http_methods& other) {}

http_methods& http_methods::operator=(const http_methods& other) {
    if (this != &other) {}
    return *this;
}

http_methods::~http_methods() {}

bool http_methods::is_valid_method(const std::string& method, const std::vector<std::string>& allowed_methods) const {
    if (std::find(allowed_methods.begin(), allowed_methods.end(), method) != allowed_methods.end())
        return true;
    return false;
}

void http_methods::add_method(std::vector<std::string>& allowed_methods, const std::string& method) {
    if (std::find(allowed_methods.begin(), allowed_methods.end(), method) == allowed_methods.end()) {
        allowed_methods.push_back(method);
    }
}

void http_methods::remove_method(std::vector<std::string>& allowed_methods, const std::string& method) {
    std::vector<std::string>::iterator it = std::find(allowed_methods.begin(), allowed_methods.end(), method);
    if (it != allowed_methods.end()) {
        allowed_methods.erase(it);
    }
}

bool upload_binary_file(const http_request &request, std::string &upload_location)
{
	std::string file;
    file.resize(request.get_body().size());
	std::string newfile = upload_location + request.get_uri();
	std::ofstream outfile(newfile, std::ios::binary);

	size_t filesize = file.size();

	//if it doesn't work, maybe this is missing before assigning body value
	// file.resize(filesize);

	outfile.write(file.data(), filesize);
	outfile.close();
	return (200);
}

bool upload_text_file(const http_request &request, std::string &upload_location)
{
	std::string file = request.get_body();
	const std::map<std::string, std::string>& query = request.get_query_params();
	std::string newfile;
    try {newfile = upload_location + query.at("filename");}
	catch (std::out_of_range &e)
	{ newfile = upload_location + newfile;}
	std::ofstream outfile(newfile);

	size_t filesize = file.size();

	outfile.write(file.data(), filesize);
	outfile.close();
	return (200);
}


const location_config* find_location_config(const std::vector<location_config>& locations, const std::string& uri) {
    print_message(STDOUT_FILENO, "Finding location config for URI: " + uri, COLOR_BLUE);
    for (std::vector<location_config>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        if (uri.compare(it->get_path()) == 0) {
            print_message(STDOUT_FILENO, "Location matched: " + it->get_path(), COLOR_BLUE);
            return &(*it);
        }
    }
    print_message(STDOUT_FILENO, "No matching location found.", COLOR_BLUE);
    return NULL; 
}

void method_router(const std::string &method, http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index) {
    const server_config& server_conf = configs[config_index];
    const location_config* loc_conf = find_location_config(server_conf.get_locations(), request.get_uri());

    std::vector<std::string> allowed_methods;

    if (loc_conf && !loc_conf->get_allowed_methods().empty()) {
        allowed_methods = loc_conf->get_allowed_methods();
        print_message(STDOUT_FILENO, "Allowed methods for location: " + loc_conf->get_path(), COLOR_YELLOW);
    }
    else if (request.get_uri() == "/upload.py" || method == "DELETE"){
        loc_conf = find_location_config(server_conf.get_locations(), "/upload");
        allowed_methods = loc_conf->get_allowed_methods();
        print_message(STDOUT_FILENO, "Allowed methods to upload: " + loc_conf->get_path(), COLOR_YELLOW);
    } else {
        allowed_methods = server_conf.get_default_methods();
    }

    size_t i = 0;
    while (i < allowed_methods.size()) {
        if(request.get_method() == allowed_methods[i])
            break;
        i++;
    }


    http_methods methods;
    if (!methods.is_valid_method(method, allowed_methods) || i == allowed_methods.size()) {
        std::string error_page_path = get_error_page_path(configs[config_index], 405);
        serve_error_page(response, error_page_path, 405, "Internal Server Error.");
        return;
    }

    if (method == "GET") {
        handle_get_request(request, response, configs, config_index);
    } else if (method == "POST") {
        handle_post_request(request, response, configs, config_index);
    } else if (method == "DELETE") {
        handle_delete_request(request, response, configs, config_index);
    } else {
        std::string error_page_path = get_error_page_path(configs[config_index], 405);
        serve_error_page(response, error_page_path, 405, "Internal Server Error.");
    }
}
