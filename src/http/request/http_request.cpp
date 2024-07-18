#include "../../../include/webserv.hpp"
# include <sys/stat.h>

http_request::http_request() : method_(""), uri_(""), http_version_(""), mime_type_("application/octet-stream") {}

const std::string& http_request::get_method() const {
    return method_;
}

const std::string& http_request::get_cgi_path() const {
    return cgi_path_;
}

const std::string& http_request::get_root() const {
    return root_;
}

const std::string& http_request::get_index() const {
    return index_;
}

bool http_request::get_autoindex() const {
    return autoindex_;
}

void http_request::set_cgi_path(const std::string& cgi_path) {
    cgi_path_ = cgi_path;
}

void http_request::set_root(const std::string& root) {
    root_ = root;
}

void http_request::set_index(const std::string& index) {
    index_ = index;
}

void http_request::set_autoindex(bool autoindex) {
    autoindex_ = autoindex;
}

void http_request::set_file_path(const std::string& file_path) {
    file_path_ = file_path;
}

const std::string& http_request::get_uri() const {
    return uri_;
}

const std::string& http_request::get_http_version() const {
    return http_version_;
}

const std::map<std::string, std::string>& http_request::get_headers() const {
    return headers_;
}

std::string http_request::get_header_value(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator it = headers_.find(key);
    if (it != headers_.end()) {
        log_instance.debug("Found header: " + key + " with value: " + it->second);
        return it->second;
    }
    log_instance.debug("Header not found: " + key);
    return "";
}

const std::string& http_request::get_body() const {
    return body_;
}

const std::string& http_request::get_bin_body() const {
    return bin_body_;
}

const std::string& http_request::get_mime_type() const {
    return mime_type_;
}

const std::map<std::string, std::string>& http_request::get_query_params() const {
    return query_params_;
}


void http_request::set_method(const std::string& method) {
    method_ = method;
}

void http_request::extract_query_params() {
    size_t pos = uri_.find('?');
    if (pos != std::string::npos) {
        std::string query_string = uri_.substr(pos + 1);
        std::istringstream query_stream(query_string);
        std::string param;
        while (std::getline(query_stream, param, '&')) {
            size_t eq_pos = param.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = param.substr(0, eq_pos);
                std::string value = param.substr(eq_pos + 1);
                query_params_[key] = value;
            }
        }
        uri_ = uri_.substr(0, pos);
    }
}

void http_request::extract_body_query_params() {
    size_t pos = body_.find('\n');
    if (pos != std::string::npos) {
        std::string query_string = body_.substr(pos + 1);
        std::istringstream query_stream(query_string.substr(0, query_string.find('\n')));
        std::string param;
        while (std::getline(query_stream, param, ';')) {
            size_t eq_pos = param.find('=');
            if (eq_pos != std::string::npos)
            {
                size_t eq_pos = param.find('\n');
                param.substr(0, eq_pos);
            }
            if (eq_pos != std::string::npos) {
                std::string key = trim_whitespace(param.substr(0, eq_pos));
                std::string value = trim_whitespace(param.substr(eq_pos + 1));
                value = trim_dquotes(param.substr(eq_pos + 1));
                query_params_[key] = value;
            }
        }
    }
}

void http_request::set_uri(const std::string& uri) {
    uri_ = uri;
    extract_query_params();
}

void http_request::set_http_version(const std::string& http_version) {
    http_version_ = http_version;
}

void http_request::add_header(const std::string& key, const std::string& value) {
    log_instance.debug("Adding header: " + key + ": " + value);
    headers_[key] = value;
}

void http_request::set_body(const std::string& body) {
    body_ = body;
}

void http_request::set_bin_body(const std::string& body) {
    bin_body_ = body;
}

void http_request::set_cgi_extension(const std::string& cgi_extension) {
    cgi_extension_ = cgi_extension;
}

void http_request::set_mime_type(const std::string& mime_type) {
    mime_type_ = mime_type;
}

bool http_request::parse_request_line(const std::string& line) {
    std::istringstream line_stream(line);
    line_stream >> method_ >> uri_ >> http_version_;
    return !line_stream.fail() && !method_.empty() && !uri_.empty() && !http_version_.empty();
}

std::string trim_whitespace(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::string trim_dquotes(std::string str) {
    size_t first = str.find_first_not_of('\"');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of('\"');
    return str.substr(first, (last - first + 1));
}

bool http_request::parse_header_line(const std::string& line) {
    static std::string last_header_key;

    std::size_t pos = line.find(":");
    if (pos == std::string::npos) {
        if (!last_header_key.empty() && (line[0] == ' ' || line[0] == '\t')) {
            headers_[last_header_key] += " " + trim_whitespace(line);
            return true;
        }
        return false;
    }

    std::string key = line.substr(0, pos);
    if (line.size() <= pos + 1) return false;
    std::string value = line.substr(pos + 1);

    key = trim_whitespace(key);
    value = trim_whitespace(value);

    last_header_key = key;
    add_header(key, value);
    return true;
}

std::string http_request::get_mime_type_from_path(const std::string &file_path) const {
    static std::map<std::string, std::string> mime_types;
    if (mime_types.empty()) {
        mime_types[".aac"] = "audio/aac";
        mime_types[".abw"] = "application/x-abiword";
        mime_types[".apng"] = "image/apng";
        mime_types[".arc"] = "application/x-freearc";
        mime_types[".avif"] = "image/avif";
        mime_types[".avi"] = "video/x-msvideo";
        mime_types[".azw"] = "application/vnd.amazon.ebook";
        mime_types[".bin"] = "application/octet-stream";
        mime_types[".bmp"] = "image/bmp";
        mime_types[".bz"] = "application/x-bzip";
        mime_types[".bz2"] = "application/x-bzip2";
        mime_types[".cda"] = "application/x-cdf";
        mime_types[".csh"] = "application/x-csh";
        mime_types[".css"] = "text/css";
        mime_types[".csv"] = "text/csv";
        mime_types[".doc"] = "application/msword";
        mime_types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        mime_types[".eot"] = "application/vnd.ms-fontobject";
        mime_types[".epub"] = "application/epub+zip";
        mime_types[".gz"] = "application/gzip";
        mime_types[".gif"] = "image/gif";
        mime_types[".htm"] = "text/html";
        mime_types[".html"] = "text/html";
        mime_types[".ico"] = "image/vnd.microsoft.icon";
        mime_types[".ics"] = "text/calendar";
        mime_types[".jar"] = "application/java-archive";
        mime_types[".jpeg"] = "image/jpeg";
        mime_types[".jpg"] = "image/jpeg";
        mime_types[".js"] = "text/javascript";
        mime_types[".json"] = "application/json";
        mime_types[".jsonld"] = "application/ld+json";
        mime_types[".mid"] = "audio/midi";
        mime_types[".midi"] = "audio/midi";
        mime_types[".mjs"] = "text/javascript";
        mime_types[".mp3"] = "audio/mpeg";
        mime_types[".mp4"] = "video/mp4";
        mime_types[".mpeg"] = "video/mpeg";
        mime_types[".mpkg"] = "application/vnd.apple.installer+xml";
        mime_types[".odp"] = "application/vnd.oasis.opendocument.presentation";
        mime_types[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
        mime_types[".odt"] = "application/vnd.oasis.opendocument.text";
        mime_types[".oga"] = "audio/ogg";
        mime_types[".ogv"] = "video/ogg";
        mime_types[".ogx"] = "application/ogg";
        mime_types[".opus"] = "audio/ogg";
        mime_types[".otf"] = "font/otf";
        mime_types[".png"] = "image/png";
        mime_types[".pdf"] = "application/pdf";
        mime_types[".php"] = "application/x-httpd-php";
        mime_types[".ppt"] = "application/vnd.ms-powerpoint";
        mime_types[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        mime_types[".rar"] = "application/vnd.rar";
        mime_types[".rtf"] = "application/rtf";
        mime_types[".sh"] = "application/x-sh";
        mime_types[".svg"] = "image/svg+xml";
        mime_types[".tar"] = "application/x-tar";
        mime_types[".tif"] = "image/tiff";
        mime_types[".tiff"] = "image/tiff";
        mime_types[".ts"] = "video/mp2t";
        mime_types[".ttf"] = "font/ttf";
        mime_types[".txt"] = "text/plain";
        mime_types[".vsd"] = "application/vnd.visio";
        mime_types[".wav"] = "audio/wav";
        mime_types[".weba"] = "audio/webm";
        mime_types[".webm"] = "video/webm";
        mime_types[".webp"] = "image/webp";
        mime_types[".woff"] = "font/woff";
        mime_types[".woff2"] = "font/woff2";
        mime_types[".xhtml"] = "application/xhtml+xml";
        mime_types[".xls"] = "application/vnd.ms-excel";
        mime_types[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        mime_types[".xml"] = "application/xml";
        mime_types[".xul"] = "application/vnd.mozilla.xul+xml";
        mime_types[".zip"] = "application/zip";
        mime_types[".3gp"] = "video/3gpp";
        mime_types[".3g2"] = "video/3gpp2";
        mime_types[".7z"] = "application/x-7z-compressed";
    }
    size_t dot_pos = file_path.find_last_of(".");
    if (dot_pos == std::string::npos) {
        return "text/plain";
    }
    std::string extension = file_path.substr(dot_pos);
    if (mime_types.find(extension) != mime_types.end()) {
        return mime_types[extension];
    }

    return "";
}

bool http_request::parse_request(const std::string& raw_request) {
    std::istringstream request_stream(raw_request);
    std::string line;
    bool request_line_parsed = false;
    while (std::getline(request_stream, line)) {
        if (line.back() == '\r') {
            line.pop_back();
        }

        if (!request_line_parsed) {
            if (!parse_request_line(line)) {
                return false;
            }
            request_line_parsed = true;
            extract_query_params(); 
        } else {
            if (line.empty()) {
                break;
            }
            if (!parse_header_line(line)) {
                return false;
            }
        }
    }

    std::ostringstream body_stream;
    while (std::getline(request_stream, line)) {
        if (line.back() == '\r') {
            line.pop_back();
        }
        body_stream << line;
        if (!request_stream.eof()) {
            body_stream << '\n';
        }
    }
    set_body(body_stream.str());
    return true;
}

int is_directory(http_request &request, std::string file_path) {
    struct stat	file_stat;

    if (file_path.find("index") != std::string::npos)
        return (0);
    std::string path;
    if (request.get_uri().find("www") == std::string::npos)
        path = "www" + request.get_uri();
    else
        path = request.get_uri();
	if (stat(path.c_str(), &file_stat) == -1)
    {
		return (0);
    }
	if (S_ISDIR(file_stat.st_mode))
	{
        if (path.back() != '/')
            request.set_uri(path + "/");
        request.is_dir = true;
        log_instance.debug("Request uri is a directory");
		return (1);
	}
	else
    {
        request.is_dir = false;
        log_instance.debug("Request uri is a file");
		return (2);
    }
	return (0);
}

//removing boundary and newlines
std::string trim_body(http_request &request, std::string body)
{
    int i;
    std::string boundary;
    i = body.find("Sec-Fetch-User");
    body.erase(0, i);
    i = body.find("\n");
    body.erase(0, i);
    i = body.find("-");
    body.erase(0, i);
    //find first boundary and erase everything until data
    i = body.find("\n");
    boundary = body.substr(0, i);
    i = body.find("\n", i + 1);
    i = body.find("\n", i + 1);
    if (request.get_method() == "POST")
        i = body.find("\n", i + 1);
    body.erase(0, i);
    i = body.find(boundary);
    //erase end boundary
    if (i != std::string::npos)
        body.erase(i, boundary.size());
    //remove -- at the end
    i = body.rfind("\n--");
    if (i != std::string::npos)
        body.erase(i, body.size());
    //remove the /n at the beginning
    body.erase(0, 1);
    return (body);
}

bool accepted_file_for_upload(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index)
{
    const std::map<std::string, std::string>& query = request.get_query_params();
    std::string filename;
    try {filename = query.at("filename");}
	catch (std::out_of_range &e)
	{ filename = "filename";}
    if (filename == ""){
        std::string error_page_path = get_error_page_path(configs[config_index], 400);
        serve_error_page(response, error_page_path, 400, "Bad request.");
        return(false);}
    size_t last_dot = filename.find_last_of('.', filename.size());
    if(last_dot == std::string::npos){
        std::string error_page_path = get_error_page_path(configs[config_index], 415);
        serve_error_page(response, error_page_path, 415, "Unsupported Media Type.");
        return(false);}
    std::string extension = request.get_mime_type_from_path(filename.substr(last_dot, 7));
    if(extension == ""){
        std::string error_page_path = get_error_page_path(configs[config_index], 415);
        serve_error_page(response, error_page_path, 415, "Unsupported Media Type.");
        return(false);}
    return(true);
}

void response_for_valid_upload(http_request &request, http_response &response, const std::map<std::string, std::string>& query)
{
    std::string filename;
    std::string mime_type;
    try
    {filename = query.at("filename");}
    catch (std::out_of_range &e)
    {filename = "filename.txt";}
    mime_type = request.get_mime_type_from_path(filename);
    response.set_status_code(200);
    response.set_body("Upload sucessful");
    response.add_header("Content-Type", "text/plain");
    response.add_header("Content-Length", std::to_string(response.get_body().size()));
}

void find_upload_path(const server_config& server_conf, std::string &upload_location)
{
    std::vector<location_config> locations = server_conf.get_locations();
    for(std::vector<location_config>::iterator it = locations.begin(); it != locations.end(); it++)
        if(it->get_path() == "/upload")
        {
            upload_location = it->get_root() + "/";
            
        }
    upload_location = "www/public/upload/";
}

bool    handle_post_request(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index) {
    
    const server_config& server_conf = configs[config_index];
    std::string upload_location;
    find_upload_path(server_conf, upload_location);
    std::string filename;
    std::string mime_type;
    const std::map<std::string, std::string>& query = request.get_query_params();
    request.extract_body_query_params();
    request.set_body(trim_body(request, request.get_bin_body()));
    int content_length;
    try {content_length = atoi(request.get_headers().at("Content-Length").c_str());}
    catch (std::out_of_range &e)
    {content_length = server_conf.get_client_max_body_size();}
    if(content_length == 0) {
        std::string error_page_path = get_error_page_path(configs[config_index], 404);
        serve_error_page(response, error_page_path, 404, "Not found.");
        return(404);
    }
    if (request.get_body().size() == 0)
    {
        response_for_valid_upload(request, response, query);
        return(200);
    }
    if (content_length > server_conf.get_client_max_body_size())
    {
        std::string error_page_path = get_error_page_path(configs[config_index], 413);
        serve_error_page(response, error_page_path, 413, "Payload Too Large");
        return(413);
    }
    if (request.get_uri() == "/upload.py")
    {
        if(accepted_file_for_upload(request, response, configs, config_index) == false)
            return(415);
    	if (request.get_body().size() > 262144)
        {
            std::string error_page_path = get_error_page_path(configs[config_index], 413);
            serve_error_page(response, error_page_path, 413, "Payload Too Large");
            return(413);
        }
        if (upload_python(request, upload_location, server_conf) != 200){
            std::cout << "problem with the python file" << std::endl;
            std::string error_page_path = get_error_page_path(configs[config_index], 500);
            serve_error_page(response, error_page_path, 500, "Internal Server Error.");
            return(500);
        }
        response_for_valid_upload(request, response, query);
    }
    if(accepted_file_for_upload(request, response, configs, config_index) == false)
        return(415);
    if (request.get_header_value("Content-Type").compare(0, 4, "text") < 0)
        upload_text_file(request, upload_location);
    response_for_valid_upload(request, response, query);
    return (200);
}

void handle_get_request(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index) {
    const server_config& server_conf = configs[config_index];
    std::string short_path = request.get_uri();
    if (short_path.find("www") != std::string::npos && short_path.find("www") != 0)
        request.set_uri(short_path.erase(0, short_path.find("www")));
    std::string file_path = get_file_path_from_uri(request, request.get_uri(), server_conf.get_locations(), configs);

    if (!request.get_query_params().empty()) {
        std::string query_log = "Query parameters: ";
        std::map<std::string, std::string>::const_iterator it;
        for (it = request.get_query_params().begin(); it != request.get_query_params().end(); ++it) {
            query_log += it->first + "=" + it->second + " ";
        }
        log_instance.debug(query_log);
    }
    if ((file_path.empty() || !file_exists(file_path)) && !file_exists(request.get_uri())) {
        std::cout << "file_path empty or !file" << std::endl;
        std::string error_page_path = get_error_page_path(configs[config_index], 404);
        serve_error_page(response, error_page_path, 404, "Not Found.");
    } else {
        if (is_cgi(configs, request) && !request.get_query_params().empty()) {
            if (cgi_handler(request, response, configs, config_index) != 200) {
                std::string error_page_path = get_error_page_path(configs[config_index], 500);
                serve_error_page(response, error_page_path, 500, "Internal Server Error.");
            }
        } else if(is_directory(request, file_path) && request.is_dir) {
            if (autoindexing_n_listing_directory(request, response, configs, config_index) == 404){
                std::string error_page_path = get_error_page_path(configs[config_index], 404);
                serve_error_page(response, error_page_path, 404, "Page Not Found");
                return;
            }
        } else {
            std::string file_content = read_file_content(file_path);
            std::string mime_type = request.get_mime_type_from_path(file_path);
            response.set_status_code(200);
            response.set_body(file_content);
            response.add_header("Content-Type", mime_type);
            response.add_header("Content-Length", std::to_string(response.get_body().size()));
        }
    }
}

void handle_get_request_for_autoindexing(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index) {
    const server_config& server_conf = configs[config_index];
    std::string short_path = request.get_uri();
    if (short_path.find("www") != std::string::npos && short_path.find("www") != 0)
        request.set_uri(short_path.erase(0, short_path.find("www")));
    std::string file_path = get_file_path_from_uri(request, request.get_uri(), server_conf.get_locations(), configs);

    if (!request.get_query_params().empty()) {
        std::string query_log = "Query parameters: ";
        std::map<std::string, std::string>::const_iterator it;
        for (it = request.get_query_params().begin(); it != request.get_query_params().end(); ++it) {
            query_log += it->first + "=" + it->second + " ";
        }
        log_instance.debug(query_log);
    }
    if ((file_path.empty() || !file_exists(file_path)) && !file_exists(request.get_uri())) {
        std::cout << "file_path empty or !file" << std::endl;
        std::string error_page_path = get_error_page_path(configs[config_index], 404);
        serve_error_page(response, error_page_path, 404, "Not Found.");
    } else {
            std::string file_content = read_file_content(file_path);
            std::string mime_type = request.get_mime_type_from_path(file_path);
            response.set_status_code(200);
            response.set_body(file_content);
            response.add_header("Content-Type", mime_type);
            response.add_header("Content-Length", std::to_string(response.get_body().size()));
    }
}

void handle_delete_request(const http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index) {
    const server_config& server_conf = configs[config_index];
    std::vector<location_config> locations = server_conf.get_locations();
    std::string file_path = "www" + request.get_uri();
    std::vector<std::string> default_method = server_conf.get_default_methods();
    std::vector<std::string> allowed_method;
    bool is_allowed = false;
    
    for(std::vector<std::string>::iterator it = default_method.begin(); it != default_method.end(); it ++)
    {
        if(*it == "DELETE")
            is_allowed = true;
    }
    std::cout << is_allowed << std::endl;
    for(std::vector<location_config>::iterator it = locations.begin(); it != locations.end(); it ++)
    {
        std::cout << it->get_root() << " " << it->get_root().compare(0, it->get_root().size() - 3, file_path) << it->get_root().size() << std::endl;
        std::cout << file_path << std::endl;
        if(file_path.find(it->get_root()) != std::string::npos)
        {
            allowed_method = it->get_allowed_methods();
            for(std::vector<std::string>::iterator it = allowed_method.begin(); it != allowed_method.end(); it ++)
            {
                if(*it == "DELETE")
                    is_allowed = true;
            }
        }
    }
    std::cout << is_allowed << std::endl;


    if (file_path.empty() || !file_exists(file_path)) {
        std::string error_page_path = get_error_page_path(configs[config_index], 404);
        serve_error_page(response, error_page_path, 404, "Not Found.");
    }
    else if (is_allowed == false) {
        std::cout << "is_allowed" << std::endl;
        std::string error_page_path = get_error_page_path(configs[config_index], 405);
        serve_error_page(response, error_page_path, 405, "Method Not Allowed.");
    } else {
        if (remove(file_path.c_str()) == 0) {
            response.set_status_code(200);
            response.set_body("File deleted successfully.");
            response.add_header("Content-Type", "text/plain");
        } else {
            std::string error_page_path = get_error_page_path(configs[config_index], 500);
            serve_error_page(response, error_page_path, 500, "Internal Server Error.");
        }
    }
    
    response.add_header("Content-Length", std::to_string(response.get_body().size()));
}

void process_unknown_request(http_response &response) {
    response.set_status_code(405);
    response.set_body("Method not allowed.");
    response.add_header("Content-Length", std::to_string(response.get_body().size()));
}

