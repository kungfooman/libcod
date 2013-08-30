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

/*HIDDEN*/ int gsc_mysql_init();
/*HIDDEN*/ int gsc_mysql_real_connect();
/*HIDDEN*/ int gsc_mysql_close();
/*HIDDEN*/ int gsc_mysql_query();
/*HIDDEN*/ int gsc_mysql_errno();
/*HIDDEN*/ int gsc_mysql_error();
/*HIDDEN*/ int gsc_mysql_affected_rows();
/*HIDDEN*/ int gsc_mysql_store_result();
/*HIDDEN*/ int gsc_mysql_num_rows();
/*HIDDEN*/ int gsc_mysql_num_fields();
/*HIDDEN*/ int gsc_mysql_field_seek();
/*HIDDEN*/ int gsc_mysql_fetch_field();
/*HIDDEN*/ int gsc_mysql_fetch_row();
/*HIDDEN*/ int gsc_mysql_free_result();
int gsc_mysql_real_escape_string();

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