#include "include/request.h"
#include "include/http_codes.h"

#include "debug.c"

/* Interface */

public void set_response_code(string _code);
public void set_content_type(string _ct);
public void set_connection(string _con);

public object http_response(string _html);


private string response_code;
private string connection;
private string content_type;


private string content;

void create()
{
	
	response_code = HTTP_OK;
	content_type = "text/html";
	connection = "close";
	
	content = 0;
}


public void set_response_code(string _code)
{
	response_code = _code;
}

public void set_content_type(string _ct)
{
	content_type = _ct;
}
public void set_connection(string _con)
{
	connection = _con;
}

public object http_response(string _content)
{
	content = _content;
	return this_object();
}

public string send()
{
	
	string pkg;
	
	pkg = "HTTP/1.0 ";
    pkg += response_code;
    pkg += "\r\n";
    pkg += "Connection: " + connection + "\r\n";
    pkg += "Server: vweb 0.1\r\n";
    pkg += "Content-Type: " + content_type + "\r\n";
    pkg += "\r\n";
    
    pkg += content;

	
	return pkg;
}


