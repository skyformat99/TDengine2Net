/*
 * Copyright (c) 2019 TAOS Data, Inc. <jhtao@taosdata.com>
 *
 * This program is free software: you can use, redistribute, and/or modify
 * it under the terms of the GNU Affero General Public License, version 3
 * or later ("AGPL"), as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TDENGINE_TAOS_H
#define TDENGINE_TAOS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void    TAOS;
typedef void**  TAOS_ROW;
typedef void    TAOS_RES;
typedef void    TAOS_SUB;
typedef void    TAOS_STREAM;
typedef void    TAOS_STMT;

#define TSDB_DATA_TYPE_NULL       0
#define TSDB_DATA_TYPE_BOOL       1     // 1 bytes
#define TSDB_DATA_TYPE_TINYINT    2     // 1 byte
#define TSDB_DATA_TYPE_SMALLINT   3     // 2 bytes
#define TSDB_DATA_TYPE_INT        4     // 4 bytes
#define TSDB_DATA_TYPE_BIGINT     5     // 8 bytes
#define TSDB_DATA_TYPE_FLOAT      6     // 4 bytes
#define TSDB_DATA_TYPE_DOUBLE     7     // 8 bytes
#define TSDB_DATA_TYPE_BINARY     8     // string
#define TSDB_DATA_TYPE_TIMESTAMP  9     // 8 bytes
#define TSDB_DATA_TYPE_NCHAR      10    // multibyte string

typedef enum {
  TSDB_OPTION_LOCALE,
  TSDB_OPTION_CHARSET,
  TSDB_OPTION_TIMEZONE,
  TSDB_OPTION_CONFIGDIR,
  TSDB_OPTION_SHELL_ACTIVITY_TIMER,
  TSDB_OPTION_SOCKET_TYPE,
  TSDB_MAX_OPTIONS
} TSDB_OPTION;

typedef struct taosField {
  char  name[64];
  short bytes;
  char  type;
} TAOS_FIELD;

#ifdef _TD_GO_DLL_
  #define DLL_EXPORT    __declspec(dllexport)
#else
  #define DLL_EXPORT 
#endif

DLL_EXPORT void  taos_init();
DLL_EXPORT int   taos_options(TSDB_OPTION option, const void *arg, ...);
DLL_EXPORT TAOS *taos_connect(const char *ip, const char *user, const char *pass, const char *db, uint16_t port);
DLL_EXPORT void  taos_close(TAOS *taos);

typedef struct TAOS_BIND {
  int            buffer_type;
  void *         buffer;
  unsigned long  buffer_length;  // unused
  unsigned long *length;
  int *          is_null;
  int            is_unsigned;  // unused
  int *          error;        // unused
} TAOS_BIND;

TAOS_STMT *taos_stmt_init(TAOS *taos);
int        taos_stmt_prepare(TAOS_STMT *stmt, const char *sql, unsigned long length);
int        taos_stmt_bind_param(TAOS_STMT *stmt, TAOS_BIND *bind);
int        taos_stmt_add_batch(TAOS_STMT *stmt);
int        taos_stmt_execute(TAOS_STMT *stmt);
TAOS_RES * taos_stmt_use_result(TAOS_STMT *stmt);
int        taos_stmt_close(TAOS_STMT *stmt);

DLL_EXPORT int taos_query(TAOS *taos, const char *sql);
DLL_EXPORT TAOS_RES *taos_use_result(TAOS *taos);
DLL_EXPORT TAOS_ROW taos_fetch_row(TAOS_RES *res);
DLL_EXPORT int taos_result_precision(TAOS_RES *res);  // get the time precision of result
DLL_EXPORT void taos_free_result(TAOS_RES *res);
DLL_EXPORT int taos_field_count(TAOS *taos);
DLL_EXPORT int taos_num_fields(TAOS_RES *res);
DLL_EXPORT int taos_affected_rows(TAOS *taos);
DLL_EXPORT TAOS_FIELD *taos_fetch_fields(TAOS_RES *res);
DLL_EXPORT int taos_select_db(TAOS *taos, const char *db);
DLL_EXPORT int taos_print_row(char *str, TAOS_ROW row, TAOS_FIELD *fields, int num_fields);
DLL_EXPORT void taos_stop_query(TAOS_RES *res);

int taos_fetch_block(TAOS_RES *res, TAOS_ROW *rows);
int taos_validate_sql(TAOS *taos, const char *sql);

// TAOS_RES   *taos_list_tables(TAOS *mysql, const char *wild);
// TAOS_RES   *taos_list_dbs(TAOS *mysql, const char *wild);

// TODO: the return value should be `const`
DLL_EXPORT char *taos_get_server_info(TAOS *taos);
DLL_EXPORT char *taos_get_client_info();
DLL_EXPORT char *taos_errstr(TAOS *taos);

DLL_EXPORT int taos_errno(TAOS *taos);

DLL_EXPORT void taos_query_a(TAOS *taos, const char *sql, void (*fp)(void *param, TAOS_RES *, int code), void *param);
DLL_EXPORT void taos_fetch_rows_a(TAOS_RES *res, void (*fp)(void *param, TAOS_RES *, int numOfRows), void *param);
DLL_EXPORT void taos_fetch_row_a(TAOS_RES *res, void (*fp)(void *param, TAOS_RES *, TAOS_ROW row), void *param);

DLL_EXPORT TAOS_SUB *taos_subscribe(const char *host, const char *user, const char *pass, const char *db, const char *table, int64_t time, int mseconds);
DLL_EXPORT TAOS_ROW taos_consume(TAOS_SUB *tsub);
DLL_EXPORT void taos_unsubscribe(TAOS_SUB *tsub);
DLL_EXPORT int taos_subfields_count(TAOS_SUB *tsub);
DLL_EXPORT TAOS_FIELD *taos_fetch_subfields(TAOS_SUB *tsub);

DLL_EXPORT TAOS_STREAM *taos_open_stream(TAOS *taos, const char *sql, void (*fp)(void *param, TAOS_RES *, TAOS_ROW row),
                              int64_t stime, void *param, void (*callback)(void *));
DLL_EXPORT void taos_close_stream(TAOS_STREAM *tstr);

DLL_EXPORT int taos_load_table_info(TAOS *taos, const char* tableNameList);

#ifdef __cplusplus
}
#endif

#endif

Java_com_taosdata_jdbc_TSDBJNIConnector_closeConnectionImp
Java_com_taosdata_jdbc_TSDBJNIConnector_connectImp
Java_com_taosdata_jdbc_TSDBJNIConnector_consumeImp
Java_com_taosdata_jdbc_TSDBJNIConnector_dumpMemoryLeakImp
Java_com_taosdata_jdbc_TSDBJNIConnector_executeQueryImp
Java_com_taosdata_jdbc_TSDBJNIConnector_fetchRowImp
Java_com_taosdata_jdbc_TSDBJNIConnector_freeResultSetImp
Java_com_taosdata_jdbc_TSDBJNIConnector_getAffectedRowsImp
Java_com_taosdata_jdbc_TSDBJNIConnector_getErrCodeImp
Java_com_taosdata_jdbc_TSDBJNIConnector_getErrMsgImp
Java_com_taosdata_jdbc_TSDBJNIConnector_getResultSetImp
Java_com_taosdata_jdbc_TSDBJNIConnector_getSchemaMetaDataImp
Java_com_taosdata_jdbc_TSDBJNIConnector_getTsCharset
Java_com_taosdata_jdbc_TSDBJNIConnector_initImp
Java_com_taosdata_jdbc_TSDBJNIConnector_setAllocModeImp
Java_com_taosdata_jdbc_TSDBJNIConnector_setOptions
Java_com_taosdata_jdbc_TSDBJNIConnector_subscribeImp
Java_com_taosdata_jdbc_TSDBJNIConnector_unsubscribeImp
Java_com_taosdata_jdbc_TSDBJNIConnector_validateCreateTableSqlImp

taos_affected_rows
=taos_close
taos_close_stream
=taos_connect
taos_consume
=taos_errno
=taos_errstr
taos_fetch_block
=taos_fetch_fields
=taos_fetch_row
taos_fetch_row_a
taos_fetch_rows_a
taos_fetch_subfields
=taos_field_count
=taos_free_result
=taos_get_client_info
=taos_get_server_info
=taos_init                 
taos_num_fields
taos_open_stream
=taos_options
taos_print_row
=taos_query
taos_query_a
taos_result_precision
taos_select_db
taos_stop_query
taos_subfields_count
taos_subscribe
taos_unsubscribe
=taos_use_result

