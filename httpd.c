#include <socket.h>
#include <socket_errors.h>

#include "include/http.h"
#include "include/request.h"

#include "debug.c"

private int port, allow_static;
private mixed *url_patterns;
private string server_root, www_root;

class connection_c
{
    int time;
    int ttl;
    string ip;
}

protected void init_vweb();
private Response resolve_view(Response res, string path, string *args);

private nosave mapping connections;


void create()
{
    seteuid("Net");
    
    connections = ([ ]);
    port = 0;
    allow_static = 0;
    url_patterns = ({ });
    
    server_root = file_name(this_object());
    server_root = server_root[0..strsrch(server_root, "/", -1)];
    www_root = server_root + "www_root/";
    
    this_object()->setup();
    call_out("init_vweb", 0);
}

protected void set_port(int _port)
{
    port = _port;
}

protected void add_url_pattern(string pattern, string func)
{
    url_patterns += ({ ({ pattern, func }) });
}

protected void allow_static_pages() { allow_static = 1; }

/*********************/
protected void init_vweb()
{
	int fd, err;
	
	if (!port) {
        sys_write("vweb: No port configured - Closing server.\n");
        return;
	}
	
	fd = socket_create(STREAM, "read_callback", "close_callback");
	
	if (fd <= 0) {
        sys_write("vweb: Error in creating socket.\n");
        return;
	}
	
	err = socket_bind(fd, port);
	
	if (err != EESUCCESS) {
        sys_write("vweb: Error (%d) in binding socket", err);
        socket_close(fd);
        return;
	}
	
	err = socket_listen(fd, "listen_callback");
	
	if (err != EESUCCESS) {
        sys_write("vweb: Error (%d) in listening to socket", err);
        socket_close(fd);
        return;
	}
}

void read_callback(int fd, mixed message)
{
	//sys_write("vweb: incoming read_callback from socket.\n");
	//sys_write("vweb: contents:\n%s\n", message);
}

void close_callback(int fd)
{
    //sys_write("vweb: close_callback.\n");
    map_delete(connections, fd);
}

void listen_callback(int fd)
{
    int new_fd;
    class connection_c connection = new(class connection_c);
    
	//sys_write("vweb: Incoming request from %s.\n", socket_address(fd));
	
	new_fd = socket_accept(fd, "vweb_read", "vweb_write");
	
	if (new_fd < 0) {
        sys_write("vweb: Couldn't perform socket_accept.\n");
        socket_close(new_fd);
        return;
	}
	
    connection->time = time();
    connection->ttl  = 5;
    connection->ip   = socket_address(new_fd);
	
    connections[new_fd] = connection;
}

void vweb_read(int fd, string msg)
{
    Request req;
    Response res;
    int err, url_match = 0;
    
    //sys_write("vweb: Incoming message on socket.\n");
    
    req = new(REQ_OBJ);
    err = req->parse_request(msg);
    
    res = new(RES_OBJ);
    
    
    sys_write("URI Resource: %s\n", 
        "%^BOLD%^" + req->query_request_uri() + "%^RESET%^");

    foreach(mixed *arr in url_patterns) {
        string pattern = arr[0];
        string func = arr[1];
        
        sys_write("Checking %s against pattern %s. ", 
            "%^BOLD%^" + req->query_request_uri() + "%^RESET%^",
            "%^BOLD%^" + pattern + "%^RESET%^");
            
        if (pcre_match(req->query_request_uri(), pattern)) {
            string *args = pcre_extract(req->query_request_uri(), pattern);
            
            res = resolve_view(res, func, args);
 
            url_match = 1;
            sys_write("%s! Ignoring remaining rules.\n", 
                "%^YELLOW%^Match%^RESET%^");
            break;
        }
        sys_write("No match, trying next.\n");
    }
    
    if (allow_static && !url_match) {
        string path = req->query_request_uri();
        
        path = www_root + path[1..];
        sys_write("Checking static page: %s\n", path);
        
        if (file_size(path) > 0) {
        	// add media handler checks
            res->http_response(read_file(path));
            url_match = 1;
        }

    }
   
    if (!url_match) {
    	string pkg ="";
        sys_write("Found no matches. Sending default HTTP response.\n\n");
        pkg += "<pre>";
        pkg += "Undefined.<br>";
        pkg += "</pre>";
        res->http_response(pkg);
    }
    
    socket_write(fd, res->send());
    socket_close(fd);
    //sys_write("vweb: Closed socket intentionally without response.\n");
}

private Response resolve_view(Response res, string path, string *args)
{
	string *tmp = explode(path, ".");
	object ob;
	
    switch (sizeof(tmp)) {
    	case 1: // add_pattern_url(patt, "func")
    		res = call_other(this_object(), path, res, args);
    		break;
    	case 2: // add_pattern_url(patt, "file.func")
    		ob = load_object(server_root + tmp[0]);
    		res = call_other(ob, tmp[1], res, args);
    		break;
    	case 3: // add_pattern_url(patt, "dir.file.func")
    		ob = load_object(server_root + tmp[0] + "/" + tmp[1]);
    		res = call_other(ob, tmp[2], res, args);
    		break;
    	default:
    		res = res->http_response("Couldn't resolve path.\n");
    		break;
    }
    
    return res;	
}

void write_callback2(int fd)
{
    sys_write("vweb: socket is ready to be written to.\n");

}


