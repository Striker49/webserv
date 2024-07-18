#ifndef HTTP_HPP
#define HTTP_HPP

#include "methods/http_methods.hpp"
#include "request/http_request.hpp"
#include "response/http_response.hpp"

bool upload_binary_file(const http_request &request, std::string &upload_location);
bool upload_text_file(const http_request &request, std::string &upload_location);
void method_router(const std::string &method, http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index);
void process_unknown_request(http_response &response);;
void handle_get_request(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index);
void handle_get_request_for_autoindexing(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index);
bool handle_post_request(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index);
void handle_delete_request(const http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index);

#endif
