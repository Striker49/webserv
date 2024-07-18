#ifndef CGI_HPP
#define CGI_HPP

# include "../webserv.hpp"
# include "../http/http.hpp"

bool is_cgi_extension(std::vector<server_config> configs, http_request &request, std::string extension);
bool is_cgi(std::vector<server_config> configs, http_request &request);
int cgi_handler(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index);
int upload_python(http_request &request, std::string upload_location, const server_config& server_conf);
int	autoindexing_n_listing_directory(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index);
#endif