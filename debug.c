varargs void sys_write(string format, mixed *arg...)
{
	object ob;
	string str;
	
	ob = find_player("volothamp");
	if(!ob) return;
	
	str = sprintf(format, arg...);
	
	ob->msg(str);
}
