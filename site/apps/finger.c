object data(object response, string *args)
{
    string info;
    info = "/secure/handlers/finger.c"->finger_info(args[0]);
    info = "/net/web/handlers/color_format.c"->LpcTextToHtmlStyle(info, 0, 1);
    return response->http_response(info);
}
