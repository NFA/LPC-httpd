#include "include/request.h"

#include "debug.c"

private int request_method;
private string request_uri;
private string request_protocol;
private mapping header;
private string request_content;

/* Interface */
public int parse_request(string req);
public mapping query_request_headers() { return copy(header); }
public int query_request_method() { return request_method; }
public string query_request_uri() { return request_uri; }
public string query_request_protocol() { return request_protocol; }
public string query_request_content() { return request_content; }



private int parse_request_line(string str);
private void parse_method(string str);
private int parse_headers(string *headers);



void create()
{
    request_method = 0;
    request_uri = "";
    request_protocol = "";
    header = ([ ]);
    
    request_content = "";
}


public int parse_request(string req)
{
    /* Only thing we can be certain of, is that the client will send a 
     * request-line, and if it's a POST request it will send a Content-Length
     * header
     */
    string *headers;
    string *tmp;

    tmp = explode(req, "\r\n\r\n"); // 2 linebreaks marks divider
    
    if (sizeof(tmp) == 1) // content was ""
        tmp += ({ "" });
    
    headers = explode(tmp[0], "\r\n");
    request_content = tmp[1];
    
    // parse the request line
    parse_request_line(headers[0]); 
        
    // parse the headers
    parse_headers(headers[1..]);
    
    return 1;
}

private int parse_request_line(string str)
{
    string *request;

    // Example "GET / HTTP/1.1"
    request = explode(str, " ");

    if (sizeof(request) != 3) return 0;    
    
    parse_method(request[0]);
    
    request_uri = request[1];
    
    request_protocol = request[2];
    
    return 1;
}

private void parse_method(string str)
{
    switch (str) {
        case "GET":
            request_method = GET; break;
        case "POST":
            request_method = POST; break;
        case "HEAD":
            request_method = HEAD; break;
        case "PUT":
            request_method = PUT; break;
        case "DELETE":
            request_method = DELETE; break;
        case "TRACE":
            request_method = TRACE; break;
        case "CONNECT":
            request_method = CONNECT; break;
        default:
            request_method = UNDEF_REQUEST; break;        
    }   
}

private void parse_headers(string *headers)
{
    int sz;
    
    sz = sizeof(headers);

    for (int i=0;i<sz;i++) {
        int divider = 0;
        string field, value;
        
        while (headers[i][divider] != 58) //ASCII(58) = ":"
            divider++;
        
        field = headers[i][0..divider-1];
        value = headers[i][divider+2..];
            
        header[field] = value; 
    }
}




