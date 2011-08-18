inherit "/w/volothamp/vweb/httpd";

void setup()
{
    set_port(5555);
    allow_static_pages();
    add_url_pattern("^/finger/(\\w+)", "apps.finger.data");
    add_url_pattern("^/tell/(\\w+)/(\\w+)", "apps.tell.data");
    add_url_pattern("^/who", "apps.who.data");
}


