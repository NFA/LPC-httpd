#include <handlers.h>

object data(object response, string *args)
{
	mapping data = ([ ]);
	
	foreach (object user in filter(users(), (: $1->is_player() :)))
		data[user->query_name()] = ([ 
			"gender": user->query_gender_string(),
			"race":   user->query_real_race_name(),
			"title": GROUP_TITLE_HANDLER->query_title(user->query_name())
			]);
	
	response->set_content_type("application/json");
    return response->http_response(encodeJSON(data));
    
}
