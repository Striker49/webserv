#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <map>

class http_request {
public:
    http_request();

    bool parse_request(const std::string& raw_request);

    const std::string& get_method() const;
    const std::string& get_cgi_path() const;
    const std::string& get_file_path() const;
    const std::string& get_uri() const;
    const std::string& get_root() const;
    const std::string& get_index() const;
    bool  get_autoindex() const;
    const std::string& get_http_version() const;
    const std::map<std::string, std::string>& get_headers() const;
    std::string get_header_value(const std::string& key) const;
    const std::string& get_body() const;
    const std::string& get_bin_body() const;
    const std::string& get_cgi_extension() const;
    const std::string& get_mime_type() const;

    void set_method(const std::string& method);
    void set_cgi_path(const std::string& cgi_path);
    void set_file_path(const std::string& file_path);
    void set_uri(const std::string& uri);
    void set_http_version(const std::string& http_version);
    void add_header(const std::string& key, const std::string& value);
    void set_body(const std::string& body);
    void set_bin_body(const std::string& body);
    void set_root(const std::string& root);
    void set_index(const std::string& index);
    void set_autoindex(bool body);
    void set_cgi_extension(const std::string& body);
    void set_mime_type(const std::string& mime_type);
    bool parse_request_line(const std::string& line);
    bool parse_header_line(const std::string& line);
    void extract_query_params();
    void extract_body_query_params();
    void parse_query_string();
    std::string get_mime_type_from_path(const std::string &file_path) const;
    const std::map<std::string, std::string>& get_query_params() const;

    bool        is_dir;

private:
    std::string file_path_;
    std::string root_;
    std::string index_;
    bool autoindex_;
    std::string cgi_path_;
    std::string method_;
    std::string uri_;
    std::string http_version_;
    std::string body_;
    std::string bin_body_;
    std::string cgi_extension_;
    std::string mime_type_;
    std::map<std::string, std::string> query_params_;
    std::map<std::string, std::string> headers_;
};

std::string trim_whitespace(const std::string& str);
std::string trim_dquotes(std::string str);
void find_upload_path(const server_config& server_conf, std::string &upload_location);

#endif
