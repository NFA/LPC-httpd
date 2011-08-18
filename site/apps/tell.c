object data(object response, string *args)
{
    string player, msg;
    object ob;
    
    player = args[0];
    msg    = args[1];
    
    ob = find_player(player);
    
    if (!ob) 
    	return response->http_response("No player by that name.\n");

    
    ob->event_person_tell(this_object(), "The Internet tells you: ", msg, "common");
    return response->http_response(sprintf("You tell %s: %s\n", ob->query_short(), msg));
    
}

string query_name() { return "WEB"; } //just a hack