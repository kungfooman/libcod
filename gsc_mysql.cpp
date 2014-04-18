#include "gsc_mysql.hpp"

#if COMPILE_MYSQL == 1

/*
	Had the problem, that a query failed but no mysql_errno() was set
	Reason: mysql_query() didnt even got executed, because the str was undefined
	So the function quittet with stackReturnInt(0)
	Now its undefined, and i shall test it every time
*/

#include <mysql/mysql.h>
#include <thread>
#include <unistd.h>

struct mysql_async_task
{
	mysql_async_task *prev;
	mysql_async_task *next;
	int id;
	MYSQL_RES *result;
	bool done;
	bool started;
	bool save;
	char query[COD2_MAX_STRINGLENGTH + 1];
};

struct mysql_async_connection
{
	mysql_async_connection *prev;
	mysql_async_connection *next;
	bool in_use;
	MYSQL *connection;
};

mysql_async_connection *first_async_connection = NULL;
mysql_async_task *first_async_task = NULL;
MYSQL *cod_mysql_connection = NULL;

void mysql_async_execute_query(mysql_async_task *q, mysql_async_connection *c) //cannot be called from gsc, is threaded.
{
	int res = mysql_query(c->connection, q->query);
	if(res)
			printf("scriptengine> Error in MySQL connection for async query. Cannot recover\n"); //keep mysql connection available for now, might recover in the future?
	else
	{
		if(q->save)
		{
			MYSQL_RES *result = mysql_store_result(c->connection);
			q->result = result;
		}
		c->in_use = false;
		q->done = true;
	}
}

void mysql_async_query_handler() //is threaded after initialize
{
	static bool started = false;
	if(started)
	{
		printf("scriptengine> async handler already started. Returning\n");
		return;
	}
	started = true;
	mysql_async_connection *c = first_async_connection;
	if(c == NULL)
	{
		printf("scriptengine> async handler started before any connection was initialized\n"); //this should never happen
		started = false;
		return;
	}
	mysql_async_task *q;
	while(true)
	{
		q = first_async_task;
		c = first_async_connection;
		while(q != NULL)
		{
			if(!q->started)
			{
				while(c != NULL && c->in_use)
					c = c->next;
				if(c == NULL)
				{
					//out of free connections
					break;
				}
				q->started = true;
				c->in_use = true;
				std::thread async_query(mysql_async_execute_query, q, c);
				async_query.detach();
				c = c->next;
			}
			q = q->next;
		}
		usleep(10000);
	}
}

int mysql_async_query_initializer(char *sql, bool save) //cannot be called from gsc, helper function
{
	static int id = 0;
	id++;
	mysql_async_task *current = first_async_task;
	while(current != NULL && current->next != NULL)
		current = current->next;
	mysql_async_task *newtask = new mysql_async_task;
	newtask->id = id;
	strncpy(newtask->query, sql, COD2_MAX_STRINGLENGTH);
	newtask->prev = current;
	newtask->result = NULL;
	newtask->save = save;
	newtask->done = false;
	newtask->next = NULL;
	newtask->started = false;
	if(current != NULL)
		current->next = newtask;
	else
		first_async_task = newtask;
	stackPushInt(id);
	return id;
}

void gsc_mysql_async_create_query_nosave()
{
	char *sql;
	if ( ! stackGetParams("s", &sql))
	{
		printf("scriptengine> wrongs args for create_mysql_async_query(...);\n");
		stackPushUndefined();
		return;
	}
	int id = mysql_async_query_initializer(sql, false);
	stackPushInt(id);
	return;
}

void gsc_mysql_async_create_query()
{
	char *sql;
	if ( ! stackGetParams("s", &sql))
	{
		printf("scriptengine> wrongs args for create_mysql_async_query(...);\n");
		stackPushUndefined();
		return;
	}
	int id = mysql_async_query_initializer(sql, true);
	stackPushInt(id);
	return;
}

void gsc_mysql_async_getdone_list()
{
	mysql_async_task *current = first_async_task;
	stackPushArray();
	while(current != NULL)
	{
		if(current->done)
		{
			stackPushInt((int)current->id);
			stackPushArrayLast();
		}
		current = current->next;
	}
}

void gsc_mysql_async_getresult_and_free() //same as above, but takes the id of a function instead and returns 0 (not done), undefined (not found) or the mem address of result
{
	int id;
	if(!stackGetParams("i", &id))
	{
		printf("scriptengine> wrong args for mysql_async_getresult_and_free_id\n");
		stackPushUndefined();
		return;
	}
	mysql_async_task *c = first_async_task;
	if(c != NULL)
	{
		while(c != NULL && c->id != id)
			c = c->next;
	}
	if(c != NULL)
	{
		if(!c->done)
		{
			stackPushUndefined(); //not done yet
			return;
		}
		if(c->next != NULL)
			c->next->prev = c->prev;
		if(c->prev != NULL)
			c->prev->next = c->next;
		else
			first_async_task = c->next;
		if(c->save)
		{
			int ret = (int)c->result;
			stackPushInt(ret);
		}
		else
			stackPushInt(0);
		delete c;
		return;
	}
	else
	{
		printf("scriptengine> mysql async query id not found\n");
		stackPushUndefined();
		return;
	}
}

void gsc_mysql_async_initializer()//returns array with mysql connection handlers
{
	if(first_async_connection != NULL)
	{
		printf("scriptengine> Async mysql already initialized. Returning before adding additional connections\n");
		stackPushUndefined();
		return;
	}
	int port, connection_count;
	char *host, *user, *pass, *db;

	if ( ! stackGetParams("ssssii", &host, &user, &pass, &db, &port, &connection_count))
	{ 
		printf("scriptengine> wrongs args for mysql_async_initializer(...);\n");
		stackPushUndefined();
		return;
	}
	if(connection_count <= 0)
	{
		printf("Need a positive connection_count in mysql_async_initializer\n");
		stackPushUndefined();
		return;
	}
	int i;
	stackPushArray();
	mysql_async_connection *current = first_async_connection;
	for(i = 0; i < connection_count; i++)
	{
		mysql_async_connection *newconnection = new mysql_async_connection;
		newconnection->next = NULL;
		newconnection->connection = mysql_init(NULL);
		newconnection->connection = mysql_real_connect((MYSQL*)newconnection->connection, host, user, pass, db, port, NULL, 0);
		my_bool reconnect = true;
		mysql_options(newconnection->connection, MYSQL_OPT_RECONNECT, &reconnect);
		newconnection->in_use = false;
		if(current == NULL)
		{
			newconnection->prev = NULL;
			first_async_connection = newconnection;
		}
		else
		{
			while(current->next != NULL)
				current = current->next;
			current->next = newconnection;
			newconnection->prev = current;
		}
		current = newconnection;
		stackPushInt((int)newconnection->connection);
		stackPushArrayLast();
	}
	std::thread async_query(mysql_async_query_handler);
	async_query.detach();
}

void gsc_mysql_init() {
	#if DEBUG_MYSQL
	printf("gsc_mysql_init()\n");
	#endif
	MYSQL *my = mysql_init(NULL);
	stackReturnInt((int) my);
}

void gsc_mysql_reuse_connection()
{
	if(cod_mysql_connection == NULL)
	{
		stackPushUndefined();
		return;
	}
	else
	{
		stackPushInt((int) cod_mysql_connection);
		return;
	}
}

void gsc_mysql_real_connect() {
	int mysql, port;
	char *host, *user, *pass, *db;

	if ( ! stackGetParams("issssi", &mysql, &host, &user, &pass, &db, &port)) {
		printf("scriptengine> wrongs args for mysql_real_connect(...);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_real_connect(mysql=%d, host=\"%s\", user=\"%s\", pass=\"%s\", db=\"%s\", port=%d)\n", mysql, host, user, pass, db, port);
	#endif

	mysql = (int) mysql_real_connect((MYSQL *)mysql, host, user, pass, db, port, NULL, 0);
	my_bool reconnect = true;
	mysql_options((MYSQL*)mysql, MYSQL_OPT_RECONNECT, &reconnect);
	if(cod_mysql_connection == NULL)
		cod_mysql_connection = (MYSQL*) mysql;
	stackReturnInt(mysql);
}

void gsc_mysql_close() {
	int mysql;
	
	if ( ! stackGetParams("i", &mysql)) {
		printf("scriptengine> wrongs args for mysql_close(mysql);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_close(%d)\n", mysql);
	#endif

	mysql_close((MYSQL *)mysql);
	stackReturnInt(0);
}

void gsc_mysql_query() {
	int mysql;
	char *sql;

	if ( ! stackGetParams("is", &mysql, &sql)) {
		printf("scriptengine> wrongs args for mysql_query(...);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_query(%d, \"%s\")\n", mysql, sql);
	#endif
	
	int ret = mysql_query((MYSQL *)mysql, sql);	
	stackReturnInt(ret);
}

void gsc_mysql_errno() {
	int mysql;

	if ( ! stackGetParams("i", &mysql)) {
		printf("scriptengine> wrongs args for mysql_errno(mysql);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_errno(%d)\n", mysql);
	#endif
	
	int ret = mysql_errno((MYSQL *)mysql);
	stackReturnInt(ret);
}

void gsc_mysql_error() {
	int mysql;

	if ( ! stackGetParams("i", &mysql)) {
		printf("scriptengine> wrongs args for mysql_error(mysql);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_error(%d)\n", mysql);
	#endif
	
	char *ret = (char *)mysql_error((MYSQL *)mysql);
	stackPushString(ret);
}

void gsc_mysql_affected_rows() {
	int mysql;

	if ( ! stackGetParams("i", &mysql)) {
		printf("scriptengine> wrongs args for mysql_affected_rows(mysql);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_affected_rows(%d)\n", mysql);
	#endif
	
	int ret = mysql_affected_rows((MYSQL *)mysql);
	stackReturnInt(ret);
}

void gsc_mysql_store_result() {
	int mysql;

	if ( ! stackGetParams("i", &mysql)) {
		printf("scriptengine> wrongs args for mysql_store_result(mysql);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_store_result(%d)\n", mysql);
	#endif
		
	MYSQL_RES *result = mysql_store_result((MYSQL *)mysql);
	stackReturnInt((int) result);
}

void gsc_mysql_num_rows() {
	int result;

	if ( ! stackGetParams("i", &result)) {
		printf("scriptengine> wrongs args for mysql_num_rows(result);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_num_rows(result=%d)\n", result);
	#endif
	
	int ret = mysql_num_rows((MYSQL_RES *)result);
	stackReturnInt(ret);
}

void gsc_mysql_num_fields() {
	int result;

	if ( ! stackGetParams("i", &result)) {
		printf("scriptengine> wrongs args for mysql_num_fields(result);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_num_fields(result=%d)\n", result);
	#endif

	int ret = mysql_num_fields((MYSQL_RES *)result);
	stackReturnInt(ret);
}

void gsc_mysql_field_seek() {
	int result;
	int offset;

	if ( ! stackGetParams("ii", &result, &offset)) {
		printf("scriptengine> wrongs args for mysql_field_seek(result, offset);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_field_seek(result=%d, offset=%d)\n", result, offset);
	#endif

	int ret = mysql_field_seek((MYSQL_RES *)result, offset);
	stackReturnInt(ret);
}

void gsc_mysql_fetch_field() {
	int result;

	if ( ! stackGetParams("i", &result)) {
		printf("scriptengine> wrongs args for mysql_fetch_field(result);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_fetch_field(result=%d)\n", result);
	#endif
	
	MYSQL_FIELD *field = mysql_fetch_field((MYSQL_RES *)result);
	if (field == NULL) {
		stackPushUndefined();
		return;
	}
	char *ret = field->name;
	stackPushString(ret);
}

void gsc_mysql_fetch_row() {
	int result;

	if ( ! stackGetParams("i", &result)) {
		printf("scriptengine> wrongs args for mysql_fetch_row(result);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_fetch_row(result=%d)\n", result);
	#endif
	
	MYSQL_ROW row = mysql_fetch_row((MYSQL_RES *)result);
	if (!row)
	{
		#if DEBUG_MYSQL
		printf("row == NULL\n");
		#endif
		stackPushUndefined();
		return;
	}

	int ret = alloc_object_and_push_to_array();
	
	int numfields = mysql_num_fields((MYSQL_RES *)result);
	for (int i=0; i<numfields; i++)
	{
		if (row[i] == NULL)
			stackPushUndefined();
		else
			stackPushString(row[i]);
		
		#if DEBUG_MYSQL
		printf("row == \"%s\"\n", row[i]);
		#endif
		push_previous_var_in_array_sub();
	}
}

void gsc_mysql_free_result() {
	int result;

	if ( ! stackGetParams("i", &result)) {
		printf("scriptengine> wrongs args for mysql_free_result(result);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_free_result(result=%d)\n", result);
	#endif
	if(result == NULL)
	{
		printf("scriptengine> Error in mysql_free_result: input is a NULL-pointer\n");
		stackPushUndefined();
		return;
	}
	mysql_free_result((MYSQL_RES *)result);
	stackPushUndefined();
}

void gsc_mysql_real_escape_string() {
	int mysql;
	char *str;

	if ( ! stackGetParams("is", &mysql, &str)) {
		printf("scriptengine> wrongs args for mysql_real_escape_string(...);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MYSQL
	printf("gsc_mysql_real_escape_string(%d, \"%s\")\n", mysql, str);
	#endif
	
	char *to = (char *) malloc(strlen(str) * 2 + 1);
	int ret = mysql_real_escape_string((MYSQL *)mysql, to, str, strlen(str));	
	stackPushString(to);
	free(to);
}

/* FUNCTIONS OF PREPARED STATEMENTS */




int gsc_mysql_stmt_init()
{
	int mysql;
	
	int helper = 0;
	helper += stackGetParamInt(1, &mysql);
	
	#if DEBUG_MYSQL
	printf("gsc_mysql_stmt_init(mysql=%d)\n", mysql); // print as %d, cause i cant print it with hex in .gsc
	#endif
	
	if (helper != 1)
	{
		printf("scriptengine> wrongs args for gsc_mysql_stmt_init(mysql);\n");
		return stackReturnInt(0);
	}

	MYSQL_STMT *stmt;
	stmt = mysql_stmt_init((MYSQL*)mysql);
	
	int ret = (int) stmt;
	return stackReturnInt(ret);
}

int gsc_mysql_stmt_close()
{
	int mysql_stmt;
	
	int helper = 0;
	helper += stackGetParamInt(1, &mysql_stmt);
	
	#if DEBUG_MYSQL
	printf("gsc_mysql_stmt_close(mysql_stmt=%d)\n", mysql_stmt);
	#endif
	
	if (helper != 1)
	{
		printf("scriptengine> wrongs args for gsc_mysql_stmt_close(mysql_stmt);\n");
		return stackReturnInt(0);
	}

	int ret = (int) mysql_stmt_close((MYSQL_STMT*)mysql_stmt);
	return stackReturnInt(ret);
}

int gsc_mysql_stmt_get_stmt_id()
{
	int mysql_stmt;
	
	int helper = 0;
	helper += stackGetParamInt(1, &mysql_stmt);
	
	#if DEBUG_MYSQL
	printf("gsc_mysql_stmt_get_stmt_id(mysql_stmt=%d)\n", mysql_stmt);
	#endif
	
	if (helper != 1)
	{
		printf("scriptengine> wrongs args for gsc_mysql_stmt_get_stmt_id(mysql_stmt);\n");
		return stackReturnInt(0);
	}

	int ret = ((MYSQL_STMT*)mysql_stmt)->stmt_id;
	return stackReturnInt(ret);
}
int gsc_mysql_stmt_get_prefetch_rows()
{
	int mysql_stmt;
	
	int helper = 0;
	helper += stackGetParamInt(1, &mysql_stmt);
	
	#if DEBUG_MYSQL
	printf("gsc_mysql_stmt_get_prefetch_rows(mysql_stmt=%d)\n", mysql_stmt);
	#endif
	
	if (helper != 1)
	{
		printf("scriptengine> wrongs args for gsc_mysql_stmt_get_prefetch_rows(mysql_stmt);\n");
		return stackReturnInt(0);
	}

	int ret = ((MYSQL_STMT*)mysql_stmt)->prefetch_rows;
	return stackReturnInt(ret);
}
int gsc_mysql_stmt_get_param_count()
{
	int mysql_stmt;
	
	int helper = 0;
	helper += stackGetParamInt(1, &mysql_stmt);
	
	#if DEBUG_MYSQL
	printf("gsc_mysql_stmt_get_param_count(mysql_stmt=%d)\n", mysql_stmt);
	#endif
	
	if (helper != 1)
	{
		printf("scriptengine> wrongs args for gsc_mysql_stmt_get_param_count(mysql_stmt);\n");
		return stackReturnInt(0);
	}

	int ret = ((MYSQL_STMT*)mysql_stmt)->param_count;
	return stackReturnInt(ret);
}
int gsc_mysql_stmt_get_field_count()
{
	int mysql_stmt;
	
	int helper = 0;
	helper += stackGetParamInt(1, &mysql_stmt);
	
	#if DEBUG_MYSQL
	printf("gsc_mysql_stmt_get_field_count(mysql_stmt=%d)\n", mysql_stmt);
	#endif
	
	if (helper != 1)
	{
		printf("scriptengine> wrongs args for gsc_mysql_stmt_get_field_count(mysql_stmt);\n");
		return stackReturnInt(0);
	}

	int ret = ((MYSQL_STMT*)mysql_stmt)->field_count;
	return stackReturnInt(ret);
}
int gsc_mysql_stmt_prepare()
{
	int mysql_stmt;
	char *sql;
	int len;
	
	int helper = 0;
	helper += stackGetParamInt(1, &mysql_stmt);
	helper += stackGetParamString(2, &sql);
	helper += stackGetParamInt(3, &len);
	
	#if DEBUG_MYSQL
	printf("gsc_mysql_stmt_prepare(mysql_stmt=%d, sql=%s)\n", mysql_stmt, sql);
	#endif
	
	if (helper != 3)
	{
		printf("scriptengine> wrongs args for gsc_mysql_stmt_prepare(mysql_stmt);\n");
		return stackReturnInt(0);
	}

	int ret = mysql_stmt_prepare((MYSQL_STMT*)mysql_stmt, sql, len);
	return stackReturnInt(ret);
}
int gsc_mysql_stmt_bind_param()
{
	int mysql_stmt;
	int param;
	
	int helper = 0;
	helper += stackGetParamInt(1, &mysql_stmt);
	helper += stackGetParamInt(2, &param);
	
	#if DEBUG_MYSQL
	printf("gsc_mysql_stmt_bind_param(mysql_stmt=%d, param=%d)\n", mysql_stmt, param);
	#endif
	
	if (helper != 2)
	{
		printf("scriptengine> wrongs args for gsc_mysql_stmt_bind_param(mysql_stmt, param);\n");
		return stackReturnInt(0);
	}

	int ret = mysql_stmt_bind_param((MYSQL_STMT*)mysql_stmt, (MYSQL_BIND*)param);
	return stackReturnInt(ret);
}
int gsc_mysql_stmt_bind_result()
{
	int mysql_stmt;
	int result;
	
	int helper = 0;
	helper += stackGetParamInt(1, &mysql_stmt);
	helper += stackGetParamInt(2, &result);
	
	#if DEBUG_MYSQL
	printf("gsc_mysql_stmt_bind_result(mysql_stmt=%d, result=%d)\n", mysql_stmt, result);
	#endif
	
	if (helper != 2)
	{
		printf("scriptengine> wrongs args for gsc_mysql_stmt_bind_result(mysql_stmt, result);\n");
		return stackReturnInt(0);
	}

	int ret = mysql_stmt_bind_result((MYSQL_STMT*)mysql_stmt, (MYSQL_BIND*)result);
	return stackReturnInt(ret);
}
int gsc_mysql_stmt_execute()
{
	int mysql_stmt;
	int result;
	
	int helper = 0;
	helper += stackGetParamInt(1, &mysql_stmt);
	
	#if DEBUG_MYSQL
	printf("gsc_mysql_stmt_execute(mysql_stmt=%d)\n", mysql_stmt);
	#endif
	
	if (helper != 1)
	{
		printf("scriptengine> wrongs args for gsc_mysql_stmt_execute(mysql_stmt);\n");
		return stackReturnInt(0);
	}

	int ret = mysql_stmt_execute((MYSQL_STMT*)mysql_stmt);
	return stackReturnInt(ret);
}
int gsc_mysql_stmt_store_result()
{
	int mysql_stmt;
	int result;
	
	int helper = 0;
	helper += stackGetParamInt(1, &mysql_stmt);
	
	#if DEBUG_MYSQL
	printf("gsc_mysql_stmt_store_result(mysql_stmt=%d)\n", mysql_stmt);
	#endif
	
	if (helper != 1)
	{
		printf("scriptengine> wrongs args for gsc_mysql_stmt_store_result(mysql_stmt);\n");
		return stackReturnInt(0);
	}

	int ret = mysql_stmt_store_result((MYSQL_STMT*)mysql_stmt);
	return stackReturnInt(ret);
}
int gsc_mysql_stmt_fetch()
{
	int mysql_stmt;
	int result;
	
	int helper = 0;
	helper += stackGetParamInt(1, &mysql_stmt);
	
	#if DEBUG_MYSQL
	printf("gsc_mysql_stmt_fetch(mysql_stmt=%d)\n", mysql_stmt);
	#endif
	
	if (helper != 1)
	{
		printf("scriptengine> wrongs args for gsc_mysql_stmt_fetch(mysql_stmt);\n");
		return stackReturnInt(0);
	}

	int ret = mysql_stmt_fetch((MYSQL_STMT*)mysql_stmt);
	return stackReturnInt(ret);
}

// execute, store_result, fetch


/*

## ##
## ##
# # #
# # #
#   #

*/

// call this tests with:
// set closer 180
// set closer 181


#if COMPILE_MYSQL_TESTS == 1

int gsc_mysql_test_0()
{
	printf("c-mysql 22.02.2012 by 123123231\n");
	printf("modified to gsc 10.05.2012 by 123123231\n");
	MYSQL *my;
	my = mysql_init(NULL);
	if (my == NULL)
	{
		printf("ERROR: mysql_init()\n");
		return stackReturnInt(0);
	}
	
	my = mysql_real_connect(
		my,
		"127.0.0.1",
		"kung",
		"zetatest",
		"kung_zeta",
		3306,
		NULL,
		0
	);
	
	if (my == NULL)
	{
		printf("ERROR: mysql_real_connect(): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
		mysql_close(my);
		return stackReturnInt(0);
	}
	
	//int ret = mysql_query(my, "SELECT 1 as first,2 as second,3  as third UNION SELECT 11,22,33");
	int ret = mysql_query(my, "SELECT * FROM players");
	
	if (ret != 0)
	{
		printf("ERROR: mysql_query(): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
		mysql_close(my);
		return stackReturnInt(0);
	}
	
	printf("affected rows: %d\n", mysql_affected_rows(my));
	
	MYSQL_RES *query;
	query = mysql_store_result(my);
	
	if (mysql_errno(my) != 0) /* do not check query==NULL, because thats also the case for like INSERT */
	{
		printf("ERROR: mysql_store_result(my): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
		mysql_close(my);
		return stackReturnInt(0);
	}
	
	printf("query-num rows: %d\n", mysql_num_rows(query));
	
	printf("spalten im query: %d\n", mysql_num_fields(query));
	
	
	
	int numfields = mysql_num_fields(query);
	
	
	MYSQL_FIELD *field;
	int i;
	mysql_field_seek(query, 0);
	for (i=0; i<numfields; i++)
	{
		field = mysql_fetch_field(query);
		printf("%s(%s), ", field->name, field->table);
	}
	printf("\n");
	

	MYSQL_ROW row;
	while (row = mysql_fetch_row(query))
	{
		int i;
		
		for (i=0; i<numfields; i++)
			printf("%s, ", row[i]);
		printf("\n");
	}
	
	
	
	
	mysql_free_result(query);
	
	
	mysql_close(my);
	
	return stackReturnInt(1);
}

int gsc_mysql_test_1()
{
	printf("test 1 10.05.2012 ;D\n");
	
	MYSQL *my;
	my = mysql_init(NULL);
	if (my == NULL)
	{
		printf("ERROR: mysql_init()\n");
		return stackReturnInt(0);
	}
	
	my = mysql_real_connect(
		my,
		"127.0.0.1",
		"kung",
		"zetatest",
		"kung_zeta",
		3306,
		NULL,
		0
	);
	
	if (my == NULL)
	{
		printf("ERROR: mysql_real_connect(): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
		mysql_close(my);
		return stackReturnInt(0);
	}
	
	int id = 10;
	
	{
		const char *sql = "SELECT 1 + ?,1 UNION SELECT 2+?,1 UNION SELECT 3,2 UNION SELECT 4,3";
		int ret;
		
		MYSQL_STMT *stmt;
		stmt = mysql_stmt_init(my);
		
		printf("stmt->stmt_id = %d\n", stmt->stmt_id);
		printf("stmt->prefetch_rows = %d\n", stmt->prefetch_rows);
		printf("stmt->param_count = %d\n", stmt->param_count);
		printf("stmt->field_count = %d\n", stmt->field_count);
		
		if (stmt == NULL)
		{
			printf("ERROR: mysql_stmt_init(my): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
			mysql_close(my);
			return stackReturnInt(0);
		}
		ret = mysql_stmt_prepare(stmt, sql, strlen(sql));
		if (ret != 0)
		{
			printf("ERROR: mysql_stmt_prepare(stmt, sql, strlen(sql)): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
			mysql_close(my);
			return stackReturnInt(0);
		}
		
		// set by mysql_stmt_prepare, before they are just default-values (0,1,0,0)
		// 
		printf("stmt->stmt_id = %d\n", stmt->stmt_id);
		printf("stmt->prefetch_rows = %d\n", stmt->prefetch_rows); // i dont know
		printf("stmt->param_count = %d\n", stmt->param_count); // "SELECT ?,?" param_count=2
		printf("stmt->field_count = %d\n", stmt->field_count); // "SELECT 1,2,3" field_count=3
		
		MYSQL_BIND param[2], result[2];
		memset(param, 0, sizeof(param));
		memset(result, 0, sizeof(result));
		
		int myNumAddresses[2];
		int paramIntA;
		int paramIntB;
		int resultIntA;
		int resultIntB;
		my_bool is_null[2];
		
		
		// JUST TO GET THE INFOS FAST :)
		printf("sizeof(MYSQL_BIND)=%d\n", sizeof(MYSQL_BIND));
		printf("MYSQL_TYPE_LONG=%d\n", MYSQL_TYPE_LONG);
		printf("offsetof(MYSQL_BIND, buffer_type)=%d\n", offsetof(MYSQL_BIND, buffer_type));
		printf("offsetof(MYSQL_BIND, buffer)=%d\n", offsetof(MYSQL_BIND, buffer));
		printf("offsetof(MYSQL_BIND, is_unsigned)=%d\n", offsetof(MYSQL_BIND, is_unsigned));
		printf("offsetof(MYSQL_BIND, is_null)=%d\n", offsetof(MYSQL_BIND, is_null));
		printf("offsetof(MYSQL_BIND, length)=%d\n", offsetof(MYSQL_BIND, length));
		
		// BIND PARAM
		
		param[0].buffer_type = MYSQL_TYPE_LONG;
		param[0].buffer = (void*) &paramIntA;
		param[0].is_unsigned = 0;
		param[0].is_null = 0;
		param[0].length = 0;
		
		param[1].buffer_type = MYSQL_TYPE_LONG;
		param[1].buffer = (void*) &paramIntB;
		param[1].is_unsigned = 0;
		param[1].is_null = 0;
		param[1].length = 0;
		
		ret = mysql_stmt_bind_param(stmt, param);
		if (ret != 0)
		{
			printf("ERROR: mysql_stmt_bind_param(stmt, param): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
			mysql_close(my);
			return stackReturnInt(0);
		}
		
		// BIND RESULT
		
		result[0].buffer_type = MYSQL_TYPE_LONG;
		result[0].buffer = (void*) &resultIntA;
		result[0].is_unsigned = 0;
		result[0].is_null = 0; //&is_null[0];
		result[0].length = 0;
		
		result[1].buffer_type = MYSQL_TYPE_LONG;
		result[1].buffer = (void*) &resultIntB;
		result[1].is_unsigned = 0;
		result[1].is_null = 0; //&is_null[1];
		result[1].length = 0;
		
		ret = mysql_stmt_bind_result(stmt, result);
		if (ret != 0)
		{
			printf("ERROR: mysql_stmt_bind_result(stmt, result): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
			mysql_close(my);
			return stackReturnInt(0);
		}
		
		{
			paramIntA = 10;
			paramIntB = 20;
			
			ret = mysql_stmt_execute(stmt);
			if (ret != 0)
			{
				printf("ERROR: mysql_stmt_execute(stmt): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
				mysql_close(my);
				return stackReturnInt(0);
			}
			
			ret = mysql_stmt_store_result(stmt);
			if (ret != 0)
			{
				printf("ERROR: mysql_stmt_store_result(stmt): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
				mysql_close(my);
				return stackReturnInt(0);
			}
			
			int i=0;
			while (mysql_stmt_fetch(stmt) == 0)
			{
				printf("row[%d] resultIntA=%d resultIntB=%d\n", i, resultIntA, resultIntB);
				i++;
			}
			printf("READED ROWS=%d\n", i);
			
			mysql_stmt_free_result(stmt);
		}
		
		#if 0
		{
			paramIntA = 100;
			paramIntB = 200;
			
			ret = mysql_stmt_execute(stmt);
			if (ret != 0)
			{
				printf("ERROR: mysql_stmt_execute(stmt): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
				mysql_close(my);
				return stackReturnInt(0);
			}
			
			ret = mysql_stmt_store_result(stmt);
			if (ret != 0)
			{
				printf("ERROR: mysql_stmt_store_result(stmt): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
				mysql_close(my);
				return stackReturnInt(0);
			}
			
			int i=0;
			while (mysql_stmt_fetch(stmt) == 0)
			{
				printf("row[%d] numAddresses[0] = %d\n", i, myNumAddresses[0]);
				printf("row[%d] numAddresses[1] = %d\n", i, myNumAddresses[1]);
				i++;
			}
			printf("READED ROWS=%d\n", i);
			
			mysql_stmt_free_result(stmt);
		}
		#endif
		
		mysql_stmt_close(stmt);
		
		printf("all went ok till here! :)\n");
	}
	
	
	
	
	//int ret = mysql_query(my, "SELECT 1 as first,2 as second,3  as third UNION SELECT 11,22,33");
	int ret = mysql_query(my, "SELECT * FROM players");
	
	if (ret != 0)
	{
		printf("ERROR: mysql_query(): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
		mysql_close(my);
		return stackReturnInt(0);
	}
	
	printf("affected rows: %d\n", mysql_affected_rows(my));
	
	MYSQL_RES *query;
	query = mysql_store_result(my);
	
	if (mysql_errno(my) != 0) /* do not check query==NULL, because thats also the case for like INSERT */
	{
		printf("ERROR: mysql_store_result(my): %d->\"%s\"\n", mysql_errno(my), mysql_error(my));
		mysql_close(my);
		return stackReturnInt(0);
	}
	
	printf("query-num rows: %d\n", mysql_num_rows(query));
	
	printf("spalten im query: %d\n", mysql_num_fields(query));
	
	
	
	int numfields = mysql_num_fields(query);
	
	
	MYSQL_FIELD *field;
	int i;
	mysql_field_seek(query, 0);
	for (i=0; i<numfields; i++)
	{
		field = mysql_fetch_field(query);
		printf("%s(%s), ", field->name, field->table);
	}
	printf("\n");
	

	MYSQL_ROW row;
	while (row = mysql_fetch_row(query))
	{
		int i;
		
		for (i=0; i<numfields; i++)
			printf("%s, ", row[i]);
		printf("\n");
	}
	
	
	
	
	mysql_free_result(query);
	
	
	mysql_close(my);
	
	return stackReturnInt(1337);
}
#endif // compile mysql tests 

#endif // compile mysql