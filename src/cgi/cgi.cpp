#include "../../include/webserv.hpp"
#include <dirent.h>
#include <time.h>
#include <sys/types.h>

bool is_cgi_extension(std::vector<server_config> configs, http_request &request, std::string extension)
{
	char buff[100];
    std::string uri = request.get_uri();
	std::string pwd(getcwd(buff, 100));
    size_t pos;
    if ((pos =uri.rfind(extension, 100)) != std::string::npos)
    {
        if(uri.compare(pos, extension.size() + 1, extension) == 0)
        {
			const std::vector<location_config>& locations = configs[0].get_locations(); //[current server] replace by the current server
    		for (std::vector<location_config>::const_iterator it = locations.begin(); it != locations.end(); ++it)
			{
            	if(it->get_path() == "/cgi")
				{
					std::map<std::string, std::string> cgi = it->get_cgi();
					request.set_cgi_path(trim_whitespace(cgi[extension]));
					request.set_file_path(pwd + (it->get_root()) + uri);
				}
			}
			request.set_cgi_extension(extension);
            return(true);
        }
        return(false);
    }
    return(false);

}

bool is_cgi(std::vector<server_config> configs, http_request &request)
{
    if(is_cgi_extension(configs, request, ".php") == true && request.get_method() == "GET")
        return(true);
    if(is_cgi_extension(configs, request, ".py") == true && request.get_method() == "GET")
        return(true);
    if(is_cgi_extension(configs, request, ".sh") == true && request.get_method() == "GET")
        return(true);
    return(false);
}

std::string find_replace(std::string readed, std::string s1, std::string s2)
{
	size_t found = 0;
	
	while(found != std::string::npos)
	{
		found = readed.find(s1, found);
		if (found != std::string::npos)
		{
			readed.erase(found, s1.length());
			readed.insert(found, s2);
			found += s2.length();
		}
	}
	return (readed);
}

int	autoindex_check(http_request &request, const std::vector<server_config>& configs, int config_index)
{
	size_t uri_length;
	const std::vector<location_config>& locations = configs[config_index].get_locations(); //[current server] replace by the current server
	if (request.get_uri() == "/")
	{
		for (std::vector<location_config>::const_iterator it = locations.begin(); it != locations.end(); ++it)
		{
			if (it->get_path() == "/")
			{
				if (it->get_autoindex() == true)
					return (true);
				return (false); //listing_directory
			}
		}
	}
	if (request.get_uri().back() == '/')
		uri_length = request.get_uri().size() - 1;
	else
		uri_length = request.get_uri().size();
	for (std::vector<location_config>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		if (uri_length < it->get_path().size())
			continue;
		if(request.get_uri().compare(uri_length - it->get_path().size(), it->get_path().size(), it->get_path()) == 0)
		{
			if (it->get_autoindex() == true)
				return (true);
			else
				return (false); //listing_directory
		}
	}
	return (false); //autoindexing
}

int autoindexing(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index)
{
	server_config serv = configs[config_index];
	std::vector<location_config> locations = serv.get_locations();
	std::string uri = request.get_uri();
	if(uri != "/")
		uri = uri.erase(uri.size() - 1, uri.size());
	for(std::vector<location_config>::iterator it = locations.begin(); it != locations.end(); it++)
	{
		if(it->get_root() == uri || it->get_path() == uri)
		{

			request.set_uri(request.get_uri() +  it->get_index());
			handle_get_request_for_autoindexing(request, response, configs, config_index);
			return(true);
		}
	}
	return(false);
}

int listing_directory(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index)
{
    DIR *dirp;
	std::string dir_name = request.get_root();
	if (dir_name.back() == '/')
	{
		dir_name.erase(dir_name.size() - 1, dir_name.size());
		std::cout << "removed /: " << dir_name << std::endl;
	}
	if (dir_name.front() == '/')
		dir_name.erase(0, 1);
	if (dir_name.find("www") == std::string::npos)
		dir_name = "www/" + dir_name;
    const char *dirname = dir_name.c_str();
    dirp = opendir(dirname);
    if (!dirp)
    {
        perror("opendir: ");
        return (1);
    }
    struct dirent *dir_entry;
    std::string body = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Directory Listing</title><style>body {font-family: Arial, sans-serif;margin: 20px;}h1 {color: #333;}ul {list-style-type: none;padding: 0;}li {margin: 5px 0;}a {text-decoration: none;color: #0066cc;}a:hover {text-decoration: underline;}</style></head><body><h1>Directory Listing</h1><ul id=\"directory-listing\">";
    dir_entry = readdir(dirp);
    while (dir_entry != NULL)
    {
        std::string entry_name = dir_entry->d_name;
		if (entry_name == ".." || entry_name == ".")
		{
	        dir_entry = readdir(dirp);
			continue;
		}
        std::string full_path = request.get_uri() + entry_name;
		if (full_path.find("www") == std::string::npos)
			full_path = "www" + full_path;
        body += "<li><code><strong><a href=\"/" + full_path + "\">" + entry_name + "</a></strong></code></li>";
        dir_entry = readdir(dirp);
    }
	body += "</ul></body></html>";
    response.set_status_code(200);
    response.set_body(body);
    response.add_header("Content-Length", std::to_string(response.get_body().size()));
    closedir(dirp);
    return (200);
}

int	autoindexing_n_listing_directory(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index) {
	int pid;
	int status;
	int fd;

    char buff[100];
	int is_auto = autoindex_check(request, configs, config_index);
	if (is_auto == 404)
		return (404);
	else if (is_auto == false) //listing directory
		return(listing_directory(request, response, configs, config_index));
	else if (is_auto == true)
		return(autoindexing(request, response, configs, config_index));
	return(0);
}

int cgi_handler(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index)
{
	int pid;
	int status;
	int fd;
	std::string root_cgi = "/www/cgi_bin";
	std::string path_cgi = "/cgi_bin";
	std::string root_html = "www/public/html";
	const std::map<std::string, std::string>& query = request.get_query_params();
    server_config server_conf = configs[config_index];
	std::vector<location_config> locations = server_conf.get_locations(); 
	for (std::vector<location_config>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        if (it->get_path()== "/cgi") 
			root_cgi = it->get_root();
	}
	char buff[100];
	std::string name;
    std::string filepath(getcwd(buff, 100));
	if (request.get_uri().find("www") == std::string::npos)
		filepath = filepath + "/" + root_cgi + request.get_uri();
	else
		filepath = filepath + path_cgi + "/" + request.get_uri();
	std::cout << "filepath" << filepath << std::endl;
	std::string cgipath = request.get_cgi_path();
    //find name in querymap
	try { name = query.at("name");}
	catch (std::out_of_range &e)
	{name = "newfile";}
	char *env[4];
	env[0] = new char[cgipath.length() + 1];
	std::strcpy(env[0], cgipath.c_str());
	env[1] = new char[filepath.length() + 1];
	std::strcpy(env[1], filepath.c_str());
	env[2] = new char[name.length() + 1];
	std::strcpy(env[2], name.c_str());
	env[3] = NULL;
	log_instance.debug("CGI script" + filepath + " will be run with \'" + name + "\' as argument");
	std::string temp_file = root_cgi + "/temp.txt";
	if((pid = fork()) == -1)
		return(500);
	if (pid == 0)
	{
		if((fd = open(temp_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
		{
			std::cout << "open in child failed" << std::endl;
			return(500);
		}
		if (dup2(fd, 1) == -1)
		{
			std::cout << "dup2 failed" << std::endl;
			return (500);
		}
		execve(cgipath.c_str(), env, NULL);
		perror("execve");
		for (int i = 0; i < 4; i++)
			delete env[i];
		exit(500);
	}
	for (int i = 0; i < 4; i++)
		delete env[i];
	clock_t start = clock() / CLOCKS_PER_SEC;
	int wait = 0;
	do {
		clock_t current = clock() / CLOCKS_PER_SEC;
		if (current- start > 5)
			break;
		wait = waitpid(pid, &status, WNOHANG);
	}
	while (wait == 0);
	if (wait == 0)
		return (500);
	if (status != 0)
	{
		log_instance.debug("Writing script output in temp file failed");
		return (500);
	}
	std::string readed;
	std::string template_file = root_cgi + "/template.html";
	std::fstream htmlTemplate(template_file);
	if(htmlTemplate.is_open() == 0)
	{
		std::cout << "template not open" << std::endl;
		return (500);
	}

	std::string html = "html";
	for (std::vector<location_config>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        if (it->get_path().size() >= html.size() && it->get_path().find(html) != std::string::npos) 
			root_html = it->get_root();
	}
	std::ofstream cgi_html(root_html+ "/cgi.html");
	std::fstream ToReplaceFile(temp_file);
	std::string ToReplaceString;
	getline(ToReplaceFile, ToReplaceString);
	while(getline(htmlTemplate, readed))
	{
		cgi_html << find_replace(readed, "(ToReplace)", ToReplaceString);
		if(htmlTemplate.peek() == std::ifstream::traits_type::eof())
			break ;
		cgi_html << std::endl;
	}
	log_instance.info("Html temp file was created with content: " + name);
	for (std::vector<location_config>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        if (it->get_path().size() >= html.size() && it->get_path().find(html) != std::string::npos) 
			request.set_uri(it->get_root() + "/cgi.html");
		std::cout << "it->get_path(): " << it->get_path() << std::endl;
	}		
	if (!file_exists(request.get_uri()))
		return (500);
	htmlTemplate.close();
	ToReplaceFile.close();
	cgi_html.close();
	// It's time to process the get method function.
	print_message(STDOUT_FILENO, "cgi handler response code :" + std::string(std::to_string(200)), COLOR_GREEN);
	handle_get_request(request, response, configs, config_index);
	return(200);
}

int upload_python(http_request &request, std::string upload_location, const server_config& server_conf)
{
	int pid;
	int status;
	int fd;
		
	std::string root_cgi;
	std::string python = "/usr/bin/python3";
	std::vector<location_config> locations = server_conf.get_locations();
	for(std::vector<location_config>::iterator it = locations.begin(); it != locations.end(); it++)
	{
		if(it->get_path() == "/cgi")
		{
			root_cgi = it->get_root();
    		std::map<std::string, std::string> cgi = it->get_cgi();
			python = cgi[".py"];
			python = trim_whitespace(python);
		}
	}
	const std::map<std::string, std::string>& query = request.get_query_params();
    char buff[100];
	std::string name;
	std::string data;
    std::string filepath(getcwd(buff, 100));
	std::string cgipath = python;

	try { name = filepath +  "/" + upload_location + query.at("filename");}
	catch (std::out_of_range &e)
	{name = filepath + "/" + upload_location + "newfile";}
	filepath = filepath + "/" +root_cgi + request.get_uri();
	data = request.get_body();
	char *env[5];
	env[0] = new char[cgipath.length() + 1];
	std::strcpy(env[0], cgipath.c_str());
	env[1] = new char[filepath.length() + 1];
	std::strcpy(env[1], filepath.c_str());
	env[2] = new char[name.length() + 1];
	std::strcpy(env[2], name.c_str());
	env[3] = new char[data.length() + 1];
	std::strcpy(env[3], data.c_str());
	env[4] = NULL;
	log_instance.debug("CGI script" + filepath + " will be run with \'" + name + "\' as argument and body: <" + data + ">");
	if((pid = fork()) == -1)
		return(500);
	if (pid == 0)
	{
		execve(cgipath.c_str(), env, NULL);
		perror("execve:");
		for (int i = 0; i < 5; i++)
			delete env[i];
		exit(500);
	}
	for (int i = 0; i < 5; i++)
		delete env[i];
	clock_t start = clock() / CLOCKS_PER_SEC;
	int wait = 0;
	do {
		clock_t current = clock() / CLOCKS_PER_SEC;
		if (current- start > 5)
			break;
		wait = waitpid(pid, &status, WNOHANG);
	}
	while (wait == 0);
	if (wait == 0)
		return (500);
	if (status != 0)
	{
		log_instance.debug("File upload with python script failed");
		return (500);
	}
	log_instance.info("File \'" + name + "\' successfully uploaded with python script");
	return (200);
}
