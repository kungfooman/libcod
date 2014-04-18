#ifndef _GSC_MYSQL_HPP_
#define _GSC_MYSQL_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gsc functions */
#include "gsc.hpp"

/* offsetof */
#include <stddef.h>

//#pragma GCC visibility push(hidden)

//#define HIDDEN __attribute__((__visibility__("hidden")))

#define HIDDEN

void gsc_mysql_init();
void gsc_mysql_real_connect();
void gsc_mysql_close();
void gsc_mysql_query();
void gsc_mysql_errno();
void gsc_mysql_error();
void gsc_mysql_affected_rows();
void gsc_mysql_store_result();
void gsc_mysql_num_rows();
void gsc_mysql_num_fields();
void gsc_mysql_field_seek();
void gsc_mysql_fetch_field();
void gsc_mysql_fetch_row();
void gsc_mysql_free_result();
void gsc_mysql_real_escape_string();
void gsc_mysql_async_create_query();
void gsc_mysql_async_create_query_nosave();
void gsc_mysql_async_getdone_list();
void gsc_mysql_async_getresult_and_free();
void gsc_mysql_async_initializer();
void gsc_mysql_reuse_connection();

/*HIDDEN*/ int gsc_mysql_stmt_init();
/*HIDDEN*/ int gsc_mysql_stmt_close();
/*HIDDEN*/ int gsc_mysql_stmt_get_stmt_id();
/*HIDDEN*/ int gsc_mysql_stmt_get_prefetch_rows();
/*HIDDEN*/ int gsc_mysql_stmt_get_param_count();
/*HIDDEN*/ int gsc_mysql_stmt_get_field_count();
/*HIDDEN*/ int gsc_mysql_stmt_prepare();
/*HIDDEN*/ int gsc_mysql_stmt_bind_param();
/*HIDDEN*/ int gsc_mysql_stmt_bind_result();
/*HIDDEN*/ int gsc_mysql_stmt_execute();
/*HIDDEN*/ int gsc_mysql_stmt_store_result();
/*HIDDEN*/ int gsc_mysql_stmt_fetch();

// test scenaries directly in cod2, no second... third ... fourth single project
int gsc_mysql_test_0();
int gsc_mysql_test_1();

//#pragma GCC visibility pop

#ifdef __cplusplus
}
#endif

#endif