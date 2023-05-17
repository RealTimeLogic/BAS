/*
** LuaSQL, SQLite driver
** Author: Tiago Dionizio, Eduardo Quintao
** See Copyright Notice in license.html

** $Id: ls_sqlite3.c 5434 2023-05-03 01:35:55Z wini $
*/

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#ifndef BA_LIB
#define BA_LIB
#endif
#ifndef LUA_LIB
#define LUA_LIB
#endif


#define BASQLWEAKKEY "_BASQL3WK"

#include "balua.h"
#include "sqlite3.h"
#include "lua.h"
#include "lauxlib.h"
#include "luasql.h"



/*
  The BA_BUILD macro, when enabled, releases the BA mutex when calling
  SQLite functions. This makes it possible to have multiple concurrent
  BA requests working on the database.
*/ 
#ifndef BA_BUILD
#define BA_BUILD
#endif

#ifdef BA_BUILD
#define GET_BAMUTEX ThreadMutex* m = baluaENV_getmutex(L)
#else
#define GET_BAMUTEX
#endif



#define LUASQL_ENVIRONMENT_SQLITE "SQLite3 environment"
#define LUASQL_CONNECTION_SQLITE "SQLite3 connection"
#define LUASQL_CURSOR_SQLITE "SQLite3 cursor"

/*
** because we are running in a web server we want to make sure that
** we collect each sql ennvironment when the request environment is
** ended. so we have changed the original luasql code to store the
** env,conn and cursor handles in the request environment rather than
** the registry. This means that these handles SHOULD collect when the
** http request is completed.
*/

#define LUASQL_REF(L) luaL_ref((L), SQL_ENV_IX);
#define LUASQL_UNREF(L,i) luaL_unref((L), SQL_ENV_IX, (i));
#define LUASQL_GETREF(L,i) lua_rawgeti(L, SQL_ENV_IX, (i));

typedef struct
{
      short       closed;
} env_data;


typedef struct
{
      short        closed;
      int          env;                /* reference to environment */
      short        auto_commit;        /* 0 for manual commit */
      unsigned int cur_counter;
      sqlite3      *sql_conn;
} connl_data;


typedef struct
{
      short       closed;
      int         conn;               /* reference to connection */
      int         numcols;            /* number of columns */
      int         colnames, coltypes; /* reference to column information tables */
      sqlite3_stmt  *sql_vm;
} cur_data;

typedef struct
{
      short       closed;
      sqlite3_blob* sql_blob;
      connl_data *conn;
      int         connref;               /* reference to connection */
} blob_data;

#define LUASQLITE_BLOB "LUASQLITE BLOB"

LUASQL_API int luaopen_luasqlsqlite3(lua_State *L);


/*
** Check for valid environment.
*/
static env_data *getenvironment(lua_State *L) {
   env_data *env = (env_data *)luaL_checkudata(L, 1, LUASQL_ENVIRONMENT_SQLITE);
   luaL_argcheck(L, env != NULL, 1, LUASQL_PREFIX"environment expected");
   luaL_argcheck(L, !env->closed, 1, LUASQL_PREFIX"environment is closed");
   return env;
}


/*
** Check for valid connection.
*/
static connl_data *getconnection(lua_State *L) {
   connl_data *conn = (connl_data *)luaL_checkudata (L, 1, LUASQL_CONNECTION_SQLITE);
   luaL_argcheck(L, conn != NULL, 1, LUASQL_PREFIX"connection expected");
   luaL_argcheck(L, !conn->closed, 1, LUASQL_PREFIX"connection is closed");
   return conn;
}


static const char* beginTrans(lua_State *L, connl_data *conn,
                              int strix, const char* optSql)
{
   luaL_Buffer b;
   const char* transType = "DEFERRED";
   if (conn->auto_commit == 0)
      transType = luaL_optstring(L, strix, transType);
   luaL_buffinit(L, &b);
   if(optSql)
      luaL_addstring(&b, optSql);
   luaL_addstring(&b, "BEGIN ");
   luaL_addstring(&b, transType);
   luaL_addstring(&b, " TRANSACTION");
   luaL_pushresult(&b);
   return lua_tostring(L, -1);
}

/*
** Check for valid cursor.
*/
static cur_data *getcursor(lua_State *L) {
   cur_data *cur = (cur_data *)luaL_checkudata (L, 1, LUASQL_CURSOR_SQLITE);
   luaL_argcheck(L, cur != NULL, 1, LUASQL_PREFIX"cursor expected");
   luaL_argcheck(L, !cur->closed, 1, LUASQL_PREFIX"cursor is closed");
   return cur;
}

static const char* sherr[] = {
   "OK"        , /*  SQLITE_OK           0   Successful result */
   "ERROR"     , /*  SQLITE_ERROR        1   SQL error or missing database */
   "INTERNAL"  , /*  SQLITE_INTERNAL     2   NOT USED. Internal logic error in SQLite */
   "PERM"      , /*  SQLITE_PERM         3   Access permission denied */
   "ABORT"     , /*  SQLITE_ABORT        4   Callback routine requested an abort */
   "BUSY"      , /*  SQLITE_BUSY         5   The database file is locked */
   "LOCKED"    , /*  SQLITE_LOCKED       6   A table in the database is locked */
   "NOMEM"     , /*  SQLITE_NOMEM        7   A malloc() failed */
   "READONLY"  , /*  SQLITE_READONLY     8   Attempt to write a readonly database */
   "INTERRUPT" , /*  SQLITE_INTERRUPT    9   Operation terminated by sqlite3_interrupt()*/
   "IOERR"     , /*  SQLITE_IOERR       10   Some kind of disk I/O error occurred */
   "CORRUPT"   , /*  SQLITE_CORRUPT     11   The database disk image is malformed */
   "NOTFOUND"  , /*  SQLITE_NOTFOUND    12   NOT USED. Table or record not found */
   "FULL"      , /*  SQLITE_FULL        13   Insertion failed because database is full */
   "CANTOPEN"  , /*  SQLITE_CANTOPEN    14   Unable to open the database file */
   "PROTOCOL"  , /*  SQLITE_PROTOCOL    15   NOT USED. Database lock protocol error */
   "EMPTY"     , /*  SQLITE_EMPTY       16   Database is empty */
   "SCHEMA"    , /*  SQLITE_SCHEMA      17   The database schema changed */
   "TOOBIG"    , /*  SQLITE_TOOBIG      18   String or BLOB exceeds size limit */
   "CONSTRAINT", /*  SQLITE_CONSTRAINT  19   Abort due to contraint violation */
   "MISMATCH"  , /*  SQLITE_MISMATCH    20   Data type mismatch */
   "MISUSE"    , /*  SQLITE_MISUSE      21   Library used incorrectly */
   "NOLFS"     , /*  SQLITE_NOLFS       22   Uses OS features not supported on host */
   "AUTH"      , /*  SQLITE_AUTH        23   Authorization denied */
   "FORMAT"    , /*  SQLITE_FORMAT      24   Auxiliary database format error */
   "RANGE"     , /*  SQLITE_RANGE       25   2nd parameter to sqlite3_bind out of range */
   "NOTADB"     /*  SQLITE_NOTADB      26   File opened that is not a database file */
};

static const char* getsherr(int i)
{
   if (i >=0 && i <=26) return sherr[i];
   if (i == SQLITE_ROW) return "ROW";
   if(i == SQLITE_DONE) return "DONE";
   return "?";
}

static int pusherr(lua_State* L, int err, const char* errmsg)
{
   lua_pushnil(L);
   lua_pushstring(L, getsherr(err));
   lua_pushstring(L, errmsg);
   return 3;
}


static int
doSqliteExec(lua_State *L, connl_data *conn, const char* sql)
{
   int tryAgain=TRUE;
   int res;
   char *errmsg = 0;
   GET_BAMUTEX;
  L_tryAgain:
   balua_releasemutex(m); 
   res = sqlite3_exec(conn->sql_conn, sql, NULL, NULL, &errmsg);
   balua_setmutex(m);
   if(res == SQLITE_BUSY && tryAgain)
   {
      /* One reason for getting busy is if the Lua code failed to close a cursor object.
         Do a full GC as an attempt to close lingering objects
      */
      tryAgain=FALSE;
      lua_gc(L, LUA_GCCOLLECT, 0);
      goto L_tryAgain;
   }
   if(res != SQLITE_OK)
   {
      int ret = pusherr(L, res, errmsg ? errmsg : "?");
      if(errmsg)
         sqlite3_free(errmsg);
      return ret;
   }
   lua_pushboolean(L, 1);
   return 1;
}



/*
** Finalizes the vm
** Return nil + errmsg or nil in case of sucess
*/
static int finalize(lua_State *L, cur_data *cur)
{
   const char *errmsg;
   int res;
   if (cur->sql_vm && sqlite3_finalize(cur->sql_vm) != SQLITE_OK) {
      errmsg = sqlite3_errmsg(sqlite3_db_handle(cur->sql_vm));
      res    = sqlite3_errcode(sqlite3_db_handle(cur->sql_vm));
      cur->sql_vm = NULL;
      return pusherr(L, res, errmsg);
   }
   cur->sql_vm = NULL;
   lua_pushnil(L);
   return 1;
}

static void pushcol(lua_State* L, sqlite3_stmt *vm, int i)
{
   int len;
   switch (sqlite3_column_type(vm,i)) {
      case SQLITE_BLOB:
         len = sqlite3_column_bytes(vm,i);
         lua_pushlstring(L, (char*)sqlite3_column_blob(vm,i), len);
         break;
      case SQLITE_NULL:
         lua_pushnil(L);
         break;
      default:
         lua_pushstring(L, (char*)sqlite3_column_text(vm, i));
   }
}

/*
** Get another row of the given cursor.
*/
static int cur_fetch (lua_State *L) {
   cur_data *cur = getcursor(L);
   sqlite3_stmt *vm = cur->sql_vm;
   int res;
   GET_BAMUTEX;

   if (vm == NULL)
      return 0;

   balua_releasemutex(m); 
   res = sqlite3_step(vm);
   balua_setmutex(m);
   if(cur->closed)
      return luaL_error(L,LUASQL_PREFIX"cur closed");

   /* no more results? */
   if (res == SQLITE_DONE)
      return finalize(L, cur);

   if (res != SQLITE_ROW)
      return finalize(L, cur);

   if (lua_istable (L, 2))
   {
      int i;
      const char *opts = luaL_optstring(L, 3, "n");

      if (strchr(opts, 'n') != NULL)
      {
         /* Copy values to numerical indices */
         for (i = 0; i < cur->numcols;)
         {
            pushcol(L, vm, i);
            lua_rawseti(L, 2, ++i);
         }
      }
      if (strchr(opts, 'a') != NULL)
      {
         /* Copy values to alphanumerical indices */
         LUASQL_GETREF(L, cur->colnames);

         for (i = 0; i < cur->numcols; i++)
         {
            lua_rawgeti(L, -1, i+1);
            pushcol(L, vm, i);
            lua_rawset (L, 2);
         }
      }
      lua_pushvalue(L, 2);
      return 1; /* return table */
   }
   else
   {
      int i;
      luaL_checkstack (L, cur->numcols, LUASQL_PREFIX"too many columns");
      for (i = 0; i < cur->numcols; ++i)
         pushcol(L, vm, i);
      return cur->numcols; /* return #numcols values */
   }
}


/*
** Close the cursor on top of the stack.
** Return 1
*/
static int cur_close(lua_State *L)
{
   connl_data *conn;
   cur_data *cur = (cur_data *)luaL_checkudata(L, 1, LUASQL_CURSOR_SQLITE);

   luaL_argcheck(L, cur != NULL, 1, LUASQL_PREFIX"cursor expected");
   if (cur->closed) {
      lua_pushboolean(L, 0);
      return 1;
   }

   /* Nullify structure fields. */
   cur->closed = 1;
   if (cur->sql_vm)
      sqlite3_finalize(cur->sql_vm);

   cur->sql_vm = NULL;

   /* Decrement cursor counter on connection object */
   LUASQL_GETREF(L, cur->conn);
   conn = lua_touserdata (L, -1);
   conn->cur_counter--;

/* because the cursor table is weak keyed/valued we should not
   need to remove it from the table - maybe.
   but the conn:close() might call this function while iterating
   so we trash the key to avoid infinite loops */

   lua_getuservalue(L, -1); /* env of the conn userdata */
   lua_pushvalue(L,1);
   lua_pushnil(L);
   lua_rawset(L, -3);
   lua_pop(L,1);

   LUASQL_UNREF(L,  cur->conn);
   if (cur->colnames >= 0)
      LUASQL_UNREF(L,  cur->colnames);
   if (cur->coltypes >= 0)
      LUASQL_UNREF(L,  cur->coltypes);

   lua_pushboolean(L, 1);
   return 1;
}


/*
** Return the list of field names.
*/
static int cur_getcolnames(lua_State *L)
{
   cur_data *cur = getcursor(L);
   LUASQL_GETREF(L, cur->colnames);
   return 1;
}


/*
** Return the list of field types.
*/
static int cur_getcoltypes(lua_State *L)
{
   cur_data *cur = getcursor(L);
   LUASQL_GETREF(L, cur->coltypes);
   return 1;
}


static cur_data* init_cursor(lua_State *L, int o, connl_data *conn,
                             sqlite3_stmt *sql_vm)
{
   cur_data *cur = (cur_data*)lua_newuserdata(L, sizeof(cur_data));

   luasql_setmeta (L, LUASQL_CURSOR_SQLITE);

   /* add to connection env */
   lua_getuservalue(L,o); /* we just happen to know this is the connection */
   lua_pushvalue(L,-2);    /* key is the userdata */
   lua_pushlightuserdata(L,cur);
   lua_rawset(L, -3);
   lua_pop(L,1);

   /* increment cursor count for the connection creating this cursor */
   conn->cur_counter++;

   /* fill in structure */
   cur->closed = 0;
   cur->numcols = -1;
   cur->colnames = LUA_NOREF;
   cur->coltypes = LUA_NOREF;
   cur->sql_vm = sql_vm;

   lua_pushvalue(L, o);
   cur->conn = LUASQL_REF(L);
   return cur;
}

static void create_namesntypes(lua_State *L, /* userdata cur is TOS */
                               sqlite3_stmt *sql_vm, int numcols, cur_data* cur)
{
   int i;
   /* fill in structure */
   cur->numcols = numcols;

   /* create table with column names */
   lua_createtable(L,numcols,0);
   for (i = 0; i < numcols;)  {
      lua_pushstring(L, sqlite3_column_name(sql_vm, i));
      lua_rawseti(L, -2, ++i);
   }
   cur->colnames = LUASQL_REF(L);

   /* create table with column types */
   lua_createtable(L,numcols,0);
   for (i = 0; i < numcols;)  {
      lua_pushstring(L, sqlite3_column_decltype(sql_vm, i));
      lua_rawseti(L, -2, ++i);
   }
   cur->coltypes = LUASQL_REF(L);
}

/*
** Create a new Cursor object and push it on top of the stack.
*/
static int create_cursor(lua_State *L, int o, connl_data *conn,
                         sqlite3_stmt *sql_vm, int numcols)
{
   cur_data *cur = (cur_data*)init_cursor(L, o, conn, sql_vm);
   create_namesntypes(L, sql_vm, numcols, cur);
   return 1;
}

#if 0
/*
** Collect a Connection object.
*/
static int conn_gc(lua_State *L)
{
   connl_data *conn = (connl_data *)luaL_checkudata(L, 1, LUASQL_CONNECTION_SQLITE);
   luaL_argcheck (L, conn != NULL, 1, LUASQL_PREFIX"connection expected");
   if (conn->closed) {
      lua_pushboolean(L, 0);
      return 1;
   }

   if (conn->cur_counter > 0){
      /* return luaL_error (L, LUASQL_PREFIX"there are open cursors"); */
      lua_pushboolean(L, 0);
      return 1;
   }

   /* Nullify structure fields. */
   conn->closed = 1;
   LUASQL_UNREF(L, conn->env);
   balua_releasemutex(m); 
   sqlite3_close(conn->sql_conn);
   balua_setmutex(m);
   lua_pushboolean(L, 1);
   return 1;
}
#endif

/*
** Close a Connection object.
*/
static int conn_close(lua_State *L)
{
   connl_data *conn=(connl_data *)luaL_checkudata(L,1,LUASQL_CONNECTION_SQLITE);
   int rc,envix;
   GET_BAMUTEX;

   luaL_argcheck (L, conn != NULL, 1, LUASQL_PREFIX"connection expected");
   if (conn->closed) {
      lua_pushboolean(L, 0);
      return 1;
   }
   lua_getuservalue(L, 1); /* env of the userdata */
   envix = lua_gettop(L);
   lua_pushvalue(L,BA_ENV_IX);
   lua_pushvalue(L,SQL_ENV_IX);
   lua_pushcclosure(L, cur_close,2);
   while (lua_pushnil(L), lua_next(L, envix)) {
      /* uses 'key' (at index -2) and 'value' (at index -1) */
      lua_pushvalue(L, -3); /* the closure */
      lua_pushvalue(L, -3); /* the key - (userdata) */
      lua_pcall(L, 1, 0, 0);
      lua_settop(L,envix + 1);
   }
   lua_settop(L,envix - 1);

   /* if this happens in a GC then we are in trouble because
      Lua has collected the object and we have no way of
      calling close again. Also the sqlite3_close will also
      fail and return SQLITE_BUSY.
   */
   if (conn->cur_counter > 0)
      return luaL_error (L, LUASQL_PREFIX"there are open cursors");

   /* Nullify structure fields. */
   conn->closed = 1;
   LUASQL_UNREF(L, conn->env);
   balua_releasemutex(m); 
   rc = sqlite3_close(conn->sql_conn);
   balua_setmutex(m);
   lua_pushboolean(L, rc == SQLITE_OK ? 1 : 0);
   return 1;
}

/*
** Parsee an SQL statement.
** Return a Cursor object
*/
static int conn_prepare(lua_State *L)
{
   connl_data *conn = getconnection(L);
   const char *statement = luaL_checkstring(L, 2);
   int res;
   sqlite3_stmt *vm;
   const char *tail;
   GET_BAMUTEX;

   balua_releasemutex(m); /* release and pushes the acquire */
   res = sqlite3_prepare_v2(conn->sql_conn, statement, -1, &vm, &tail);
   balua_setmutex(m);/* acquire the mutex*/

   if (res != SQLITE_OK)
      return pusherr(L, res,
                     sqlite3_errmsg(conn->sql_conn));

   init_cursor(L, 1, conn, vm);
   return 1;
}

/*
**  ::bind{ {"BLOB","X"|n},{"TEXT","X"},{"INTEGER",1},... }
**  FLOAT, NULL
*/
static int stmt_bind(lua_State *L)
{
   cur_data *cur = getcursor(L);
   sqlite3_stmt *vm = cur->sql_vm;
   int bcount = sqlite3_bind_parameter_count(vm);
   int i;

   if(1 == lua_gettop(L))
   {
      lua_pushinteger(L, bcount);
      return 1;
   }

   luaL_checktype(L, 2, LUA_TTABLE);

   sqlite3_reset(vm);

   cur->numcols = -1;
   if (cur->colnames != LUA_NOREF) {
      LUASQL_UNREF(L,  cur->colnames);
      cur->colnames = LUA_NOREF;
   }
   if (cur->coltypes != LUA_NOREF) {
      LUASQL_UNREF(L,  cur->coltypes);
      cur->coltypes = LUA_NOREF;
   }

   for (i=1; i <= bcount; ++i) {
      const char* tp;

      lua_rawgeti(L,2, i);
      luaL_checktype(L, -1, LUA_TTABLE);
      lua_rawgeti(L, -1, 1); /* type  */
      luaL_checktype(L, -1, LUA_TSTRING);
      lua_rawgeti(L, -2, 2); /* value */
      lua_remove(L,-3);

      tp = lua_tostring(L, -2);
      if (strcmp("BLOB",tp) == 0) {
         if (lua_isnumber(L,-1)) { /* treat as zero blob */
            int n = (int)lua_tointeger(L, -1);
            sqlite3_bind_zeroblob(vm, i, n);
         }
         else {
            size_t l;
            const void* val = lua_tolstring(L, -1, &l);
            sqlite3_bind_blob(vm, i, val, (int)l, SQLITE_TRANSIENT);
         }
      }
      else if (strcmp("TEXT",tp) == 0) {
         size_t l;
         const char* val = lua_tolstring(L, -1, &l);
         sqlite3_bind_text(vm, i, val, (int)l, SQLITE_TRANSIENT);
      }
      else if (strcmp("INTEGER",tp) == 0) {
         lua_Integer val = lua_tointeger(L, -1);
         sqlite3_bind_int64(vm, i, (sqlite3_int64)val);
      }
#ifndef SQLITE_OMIT_FLOATING_POINT
      else if (strcmp("FLOAT",tp) == 0) {
         lua_Number val = lua_tonumber(L, -1);
         sqlite3_bind_double(vm, i, val);
      }
#endif
      else if (strcmp("NULL",tp) == 0) {
         sqlite3_bind_null(vm, i);
      }
      else {
         luaL_error(L, "invalid bind type parameter #u",i);
         return 0;
      }
      lua_pop(L,2);
   }
   lua_settop(L,1);
   return 1;
}

static int stmt_unbind(lua_State *L)
{
   cur_data *cur = getcursor(L);
   sqlite3_stmt *vm = cur->sql_vm;


   int rc = sqlite3_clear_bindings(vm);
   if (rc != SQLITE_OK) {
      connl_data *conn;
      LUASQL_GETREF(L, cur->conn);
      conn = lua_touserdata (L, -1);
      return pusherr(L, rc, sqlite3_errmsg(conn->sql_conn));
   }
   lua_pushboolean(L,1);
   return 1;
}


static int stmt_execute(lua_State *L)
{
   cur_data *cur = getcursor(L);
   sqlite3_stmt *vm = cur->sql_vm;
   connl_data *conn;
   int numcols,res;
   const char *errmsg;
   GET_BAMUTEX;

   LUASQL_GETREF(L, cur->conn);
   conn = lua_touserdata (L, -1);

   balua_releasemutex(m); 
   /* process first reirst result to retrive query information and type */
   res = sqlite3_step(vm);
   numcols = sqlite3_column_count(vm);
   balua_setmutex(m);

   /* real query? if empty, must have numcols!=0 */
   if ((res == SQLITE_ROW) || ((res == SQLITE_DONE) && numcols)) {
      sqlite3_reset(vm);
      create_namesntypes(L, vm, numcols, cur);
      lua_pushvalue(L,1);
      return 1;
   }

   if (res == SQLITE_DONE) { /* and numcols==0, INSERT,UPDATE,DELETE statement */
//    sqlite3_finalize(vm);
//    cur->sql_vm = NULL;
      /* return number of columns changed */
      lua_pushinteger(L, sqlite3_changes(conn->sql_conn));
      return 1;
   }
   /* error */
   errmsg = sqlite3_errmsg(conn->sql_conn);
   if (cur->sql_vm) {
      sqlite3_finalize(vm);
      cur->sql_vm = NULL;
   }
   return pusherr(L, res, errmsg);
}

/*
** Execute an SQL statement.
** Return a Cursor object if the statement is a query, otherwise
** return the number of tuples affected by the statement.
*/
static int conn_execute(lua_State *L)
{
   connl_data *conn = getconnection(L);
   const char *statement;
   int res;
   sqlite3_stmt *vm;
   const char *errmsg;
   int numcols;
   const char *tail;
   GET_BAMUTEX;

   statement = luaL_checkstring(L, 2);
   balua_releasemutex(m); 
   res = sqlite3_prepare_v2(conn->sql_conn, statement, -1, &vm, &tail);
   balua_setmutex(m);
   if (res != SQLITE_OK)
   {
      return pusherr(L, res, sqlite3_errmsg(conn->sql_conn));
   }
   balua_releasemutex(m); 
   /* process first result to retrive query information and type */
   res = sqlite3_step(vm);
   numcols = sqlite3_column_count(vm);
   balua_setmutex(m);

   /* real query? if empty, must have numcols!=0 */
   if ((res == SQLITE_ROW) || ((res == SQLITE_DONE) && numcols))
   {
      sqlite3_reset(vm);
      return create_cursor(L, 1, conn, vm, numcols);
   }

   if (res == SQLITE_DONE) /* and numcols==0, INSERT,UPDATE,DELETE statement */
   {
      sqlite3_finalize(vm);
      /* return number of columns changed */
      lua_pushinteger(L, sqlite3_changes(conn->sql_conn));
      return 1;
   }

   /* error */
   errmsg = sqlite3_errmsg(conn->sql_conn);
   sqlite3_finalize(vm);
   return pusherr(L, res, errmsg);
}


/*
** Commit the current transaction.
*/
static int conn_commit(lua_State *L)
{
   connl_data *conn=getconnection(L);
   return doSqliteExec(L, conn, beginTrans(L,conn,2,"COMMIT;"));
}


/*
** Rollback the current transaction.
*/
static int conn_rollback(lua_State *L)
{
   connl_data *conn=getconnection(L);
   return doSqliteExec(L, conn, beginTrans(L,conn,2,"ROLLBACK;"));
}

/*
** muli statement execute
*/

struct cbdata {
      lua_State* L;
      const char* opt;
      int ix;
};

static int execcallback(void* vl,int ncols, char** data, char** names)  /* Callback function */
{
   struct cbdata* cb = (struct cbdata*)vl;
   lua_State *L = (lua_State*)cb->L;
   char opt = cb->opt[0];
   int i;
   //GET_BAMUTEX;

   //balua_setmutex(m);/* Was released by conn_multiexec */

   lua_newtable(L);
   for (i=0; i < ncols; i++, names++, data++) {
      if (opt == 'a') {
         lua_pushstring(L, *names);
         lua_pushstring(L, *data);
         lua_rawset(L, -3);
      }
      else {
         int n = (int)lua_rawlen(L,-1) + 1;
         lua_pushstring(L, *data);
         lua_rawseti(L, -2, n);
      }
   }
   i = (int)lua_rawlen(L,cb->ix) + 1;
   lua_rawseti(L, cb->ix, i);

   //balua_releasemutex(m); 

   return 0;
}

static int conn_multiexec(lua_State *L)
{
   char *errmsg;
   connl_data *conn = getconnection(L);
   const char* s = luaL_checkstring(L,2);
   const char* opt = luaL_optstring(L,3,"n");
   int res;
   struct cbdata cb;

   /* Disabled for now: The execcallback must set the BA mutex, but the
    * internal SQLite mutex is set at this point. This can lead to a
    * deadlock since not all sqlite3_xxx calls in this file releases
    * the mutex. The code can be re-enabled if all sqlite3_xxx calls
    * are designed as: release; sqlite3_xxx; set;
    */
   //GET_BAMUTEX;

   cb.L = L;
   cb.opt = opt;
   lua_settop(L,3);
   lua_newtable(L);
   cb.ix=lua_gettop(L);

   //balua_releasemutex(m); 
   res = sqlite3_exec(conn->sql_conn, s, execcallback, &cb, &errmsg);
   //balua_setmutex(m);

   if (res != SQLITE_OK) {
      int ret = pusherr(L, res, errmsg);
      sqlite3_free(errmsg);
      return ret;
   }
   lua_pushnil(L);  /* first key */
   if (lua_next(L, cb.ix) == 0) {
      lua_pushboolean(L,1);
      return 1;
   }
   lua_pop(L,1);
   lua_pushboolean(L,1);
   lua_pushvalue(L,cb.ix);
   return 2;
}


static int conn_getlastautoid(lua_State *L)
{
   connl_data *conn = getconnection(L);
   lua_pushinteger(L, sqlite3_last_insert_rowid(conn->sql_conn));
   return 1;
}

static int conn_setbusytimeout(lua_State *L)
{
   connl_data *conn = getconnection(L);
   int ms = (int)(luaL_checknumber(L,2));
   luaL_argcheck(L, ms >= 0, 2, "invalid timeout (secs)");
   sqlite3_busy_timeout(conn->sql_conn, ms);
   return 0;
}

static int conn_tablelist(lua_State *L) {
   const char* sql = "SELECT name FROM sqlite_master "
      "WHERE type='table' ORDER BY name;";
   int rc,res;
   cur_data *cur;
   sqlite3_stmt *vm;
   int i=0;

   lua_settop(L,1);
   lua_pushstring(L, sql);
   rc = conn_execute(L);
   if (rc!=1 || !lua_isuserdata(L,lua_gettop(L))) return rc;

   lua_replace(L,1);
   lua_settop(L,1);
   lua_newtable(L);
   cur = getcursor(L);
   vm = cur->sql_vm;
   if (vm == NULL)  return 0;

   for(;;) {
      res = sqlite3_step(vm);
      /* no more results? */
      if (res == SQLITE_DONE)  break;
      if (res != SQLITE_ROW)   break;
      lua_pushstring(L, (char*)sqlite3_column_text(vm, 0));
      lua_rawseti(L, 2, ++i);
   }
   cur_close(L);
   lua_settop(L,2);
   return 1;
}



/*
** Set "auto commit" property of the connection.
** If 'true', then rollback current transaction.
** If 'false', then start a new transaction.
*/
static int conn_setautocommit(lua_State *L)
{
   connl_data *conn = getconnection(L);
   int isBool=lua_isboolean(L,2);
   if(isBool && lua_toboolean(L, 2))
   {
      conn->auto_commit = 1;
      return doSqliteExec(L, conn, "ROLLBACK");
   }
   conn->auto_commit = 0;
   return doSqliteExec(L, conn, beginTrans(L,conn,3, 0));
}


static int blob_close(lua_State* L)
{
   blob_data *blob = (blob_data *)luaL_checkudata(L, 1, LUASQLITE_BLOB);
   int rc;
   sqlite3_blob* sql_blob;

   luaL_argcheck (L, blob != NULL, 1, LUASQL_PREFIX"blob expected");

   sql_blob = blob->sql_blob;
   blob->closed = 1;

   if (!sql_blob) {
      lua_pushboolean(L, 0);
      return 1;
   }

   LUASQL_UNREF(L, blob->connref);

   /* Nullify structure fields. */
   blob->sql_blob = NULL;

   rc = sqlite3_blob_close(sql_blob);
   lua_pushboolean(L, rc == SQLITE_OK ? 1 : 0);
   return 1;
}

static int blob_write(lua_State* L)
{
   blob_data *blob = (blob_data *)luaL_checkudata(L, 1, LUASQLITE_BLOB);
   int rc;
   sqlite3_blob* sql_blob;
   size_t l;
   const char* p = luaL_checklstring(L,2,&l);
   int bofs = (int)luaL_optinteger(L,3,0);

   luaL_argcheck (L, blob != NULL&& blob->sql_blob != NULL, 1,
                  LUASQL_PREFIX"blob expected");

   sql_blob = blob->sql_blob;

   rc = sqlite3_blob_write(sql_blob, p, (int)l, bofs);

   if (rc != SQLITE_OK) {
      lua_settop(L,2);
      return pusherr(L, rc, sqlite3_errmsg(blob->conn->sql_conn));
   }

   lua_pushinteger(L, l);

   return 1;
}

static int blob_read(lua_State* L)
{
   blob_data *blob = (blob_data *)luaL_checkudata(L, 1, LUASQLITE_BLOB);
   int rc;
   sqlite3_blob* sql_blob;
   luaL_Buffer b;
   size_t n = (size_t)luaL_checkinteger(L,2);  /* how much to read */
   size_t bofs = (size_t)luaL_checkinteger(L,3);  /* where to read */
   size_t rlen = LUAL_BUFFERSIZE;  /* try to read that much each time */

   luaL_argcheck (L, blob != NULL&& blob->sql_blob != NULL, 1,
                  LUASQL_PREFIX"blob expected");

   luaL_buffinit(L, &b);

   sql_blob = blob->sql_blob;

   do {
      char *p = luaL_prepbuffer(&b);
      if (rlen > n) rlen = n;  /* cannot read more than asked */
      rc =  sqlite3_blob_read(sql_blob, p, (int)rlen, (int)bofs);
      if (rc != SQLITE_OK) {
         lua_settop(L,2);
         return pusherr(L, rc, sqlite3_errmsg(blob->conn->sql_conn));
      }
      luaL_addsize(&b, rlen);
      n -= rlen;  /* still have to read `n' chars */
      bofs += rlen;
   } while (n > 0 && rc == SQLITE_OK);  /* until end of count or eof */
   luaL_pushresult(&b);  /* close buffer */
   return 1;
}

static int blob_len(lua_State* L)
{
   blob_data *blob = (blob_data *)luaL_checkudata(L, 1, LUASQLITE_BLOB);
   if (blob != NULL && blob->sql_blob)
      lua_pushinteger(L,sqlite3_blob_bytes(blob->sql_blob));
   else
      lua_pushinteger(L,0);
   return 1;
}

static int conn_blobzero(lua_State* L)
{
   connl_data *conn = getconnection(L);
   const char *table=luaL_checkstring(L,2);
   const char *col=luaL_checkstring(L,3);
   sqlite3_int64 row = luaL_checkinteger(L,4);
   size_t sz  = (size_t)luaL_checkinteger(L,5);
   sqlite3_stmt *vm;
   const char *errmsg;
   const char *tail;
   int rc;
   char* stmt;

   stmt = sqlite3_mprintf("update %Q set %Q = ? where _ROWID_ = %llu;",
                          table, col, row);

   rc = sqlite3_prepare_v2(conn->sql_conn, stmt, -1, &vm, &tail);
   sqlite3_free(stmt);

   if (rc != SQLITE_OK) {
      return pusherr(L, rc, sqlite3_errmsg(conn->sql_conn));
   }
   rc = sqlite3_bind_zeroblob(vm, 1, (int)sz);
   if (rc != SQLITE_OK) {
      errmsg = sqlite3_errmsg(conn->sql_conn);
      if (vm) sqlite3_finalize(vm);
      return pusherr(L, rc, errmsg);
   }
   rc = sqlite3_step(vm);
   if (rc != SQLITE_DONE) {
      errmsg = sqlite3_errmsg(conn->sql_conn);
      if (vm) sqlite3_finalize(vm);
      return pusherr(L, rc, errmsg);
   }
   lua_pushinteger(L, sz);
   if (vm) sqlite3_finalize(vm);
   return 1;
}

static int conn_openblob(lua_State* L)
{
   blob_data *blob;
   connl_data *conn = getconnection(L);
   const char *db=NULL;
   const char *table=luaL_checkstring(L,2);
   const char *col=luaL_checkstring(L,3);
   sqlite3_int64 row = luaL_checkinteger(L,4);
   int flags=0;
   sqlite3_blob *sql_blob;
   int rc,top;

   top = lua_gettop(L);
   if (top >= 5) {
      if (lua_toboolean(L,5) == 1)
         flags = 1;
   }
   rc = sqlite3_blob_open(conn->sql_conn,
                          db, table, col, row, flags, &sql_blob);

   if (rc != SQLITE_OK) {
      const char* errmsg = sqlite3_errmsg(conn->sql_conn);
      return pusherr(L, rc, errmsg);
   }
   blob = (blob_data*)lua_newuserdata(L, sizeof(blob_data));
   luasql_setmeta(L, LUASQLITE_BLOB);
   blob->sql_blob = sql_blob;
   blob->closed = 0;
   lua_pushvalue (L, 1);
   blob->conn=conn;
   blob->connref = LUASQL_REF(L);
   return 1;
}

/*
** Create a new Connection object and push it on top of the stack.
*/
static int create_connection(lua_State *L, int env, sqlite3 *sql_conn)
{
   connl_data *conn = (connl_data*)lua_newuserdata(L, sizeof(connl_data));
   luasql_setmeta(L, LUASQL_CONNECTION_SQLITE);

   /* set a new environment for the userdata
      this replaces the defaul env which is by default the env
      of the currently executing closure, if one exists else
      from the current thread
      This is used to hold psuedo references to any created cursors */
   lua_createtable(L,0,0);
   luaL_getmetatable(L, BASQLWEAKKEY);
   lua_setmetatable(L,-2);
   lua_setuservalue(L, -2);

   /* fill in structure */
   conn->closed = 0;
   conn->env = LUA_NOREF;
   conn->auto_commit = 1;
   conn->sql_conn = sql_conn;
   conn->cur_counter = 0;
   lua_pushvalue (L, env);
   conn->env = LUASQL_REF(L);

   /* set a reasonable default busy timeout */
   sqlite3_busy_timeout(sql_conn, 500);

   return 1;
}


/*
** Connects to a data source.
*/

static int env_connect(lua_State *L)
{
   const char *sourcename;
   const char *p2;
   sqlite3* conn=NULL;
   int res, flags;

   getenvironment(L);  /* validate environment */
   sourcename = luaL_checkstring(L, 2);

   p2 = luaL_optstring(L, 3, "");

   if (strcmp(p2,"") == 0)
      flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
   else if (strcmp(p2,"READONLY") == 0)
      flags = SQLITE_OPEN_READONLY;
   else if (strcmp(p2,"NOCREATE") == 0)
      flags = SQLITE_OPEN_READWRITE;
   else {
      luaL_argerror(L,2, "Invalid open option");
      return 0;
   }

   res = sqlite3_open_v2(sourcename, &conn, flags, NULL);
   if (res != SQLITE_OK) {
      int ret = pusherr(L, res, sqlite3_errmsg(conn));
      if (conn) sqlite3_close(conn);
      return ret;
   }
   return create_connection(L, 1, conn);
}


/*
** Close environment object.
*/
static int env_close (lua_State *L)
{
   env_data *env = (env_data *)luaL_checkudata(L, 1, LUASQL_ENVIRONMENT_SQLITE);
   luaL_argcheck(L, env != NULL, 1, LUASQL_PREFIX"environment expected");
   if (env->closed) {
      lua_pushboolean(L, 0);
      return 1;
   }

   env->closed = 1;
   lua_pushboolean(L, 1);
   return 1;
}

static int get_version(lua_State *L)
{
   lua_pushstring(L, sqlite3_libversion());
   return 1;
}

static int get_memory(lua_State *L)
{
   lua_pushinteger(L, sqlite3_memory_used());
   lua_pushinteger(L, sqlite3_memory_highwater(0));
   return 2;
}

/* add single quotes and dup single quotes */
static int quote_str(lua_State *L)
{
   char* s2 = sqlite3_mprintf("%Q", luaL_checkstring(L,1));
   lua_pushstring(L,s2);
   sqlite3_free(s2);
   return 1;
}


/*
** Create metatables for each class of object.
*/
static void create_metatables(lua_State *L)
{
   struct luaL_Reg environment_methods[] = {
      {"__gc", env_close},
      {"close", env_close},
      {"__close", env_close},
      {"connect", env_connect},
      {"version", get_version},
      {"memory", get_memory},
      {"quotestr", quote_str},
      {NULL, NULL},
   };
   struct luaL_Reg connection_methods[] = {
      {"__gc", conn_close},
      {"close", conn_close},
      {"__close", conn_close},
      {"execute", conn_execute},
      {"commit", conn_commit},
      {"rollback", conn_rollback},
      {"setautocommit", conn_setautocommit},
      {"getlastautoid", conn_getlastautoid},
      {"lastid", conn_getlastautoid},
      {"setbusytimeout", conn_setbusytimeout},
      {"tables",  conn_tablelist},
      {"prepare",  conn_prepare},
      {"openblob",  conn_openblob},
      {"zeroblob",  conn_blobzero},
      {"mexec",  conn_multiexec},
      {NULL, NULL},
   };
   struct luaL_Reg cursor_methods[] = {
      {"__gc", cur_close},
      {"close", cur_close},
      {"__close", cur_close},
      {"fetch", cur_fetch},
      {"getcolnames", cur_getcolnames},
      {"getcoltypes", cur_getcoltypes},
      {"bind",     stmt_bind},
      {"execute",     stmt_execute},
      {"unbind",   stmt_unbind},
      {NULL, NULL},
   };
   struct luaL_Reg blob_methods[] = {
      {"__gc", blob_close},
      {"close", blob_close},
      {"__close", blob_close},
      {"read",  blob_read},
      {"write", blob_write},
      {"__len", blob_len},
      {NULL, NULL},
   };
   luasql_createmeta(L, LUASQL_ENVIRONMENT_SQLITE, environment_methods);
   luasql_createmeta(L, LUASQL_CONNECTION_SQLITE, connection_methods);

   luasql_createmeta(L, LUASQL_CURSOR_SQLITE, cursor_methods);
   luasql_createmeta(L, LUASQLITE_BLOB, blob_methods);
   lua_pop (L, 4);
}

/*
** Creates an Environment and returns it.
*/
static int create_environment (lua_State *L)
{
   env_data *env = (env_data *)lua_newuserdata(L, sizeof(env_data));
   luasql_setmeta(L, LUASQL_ENVIRONMENT_SQLITE);

   /* fill in structure */
   env->closed = 0;

   return 1;
}


/*
** Creates the metatables for the objects and registers the
** driver open method.
*/
static int
initSqlitePrivData(lua_State *L)
{
   struct luaL_Reg driver[] = {
      {"sqlite", create_environment},
      {NULL, NULL},
   };
   struct luaL_Reg globals[] = {
      {"quotestr", quote_str},
      {NULL, NULL},
   };


   create_metatables(L);

   lua_getglobal(L, LUASQL_TABLENAME);
   if(lua_isnil(L, -1))
   {
      lua_pop(L,1);
      lua_newtable(L);
      lua_pushvalue(L, -1);
      lua_setglobal(L, LUASQL_TABLENAME);
   }
   lua_pushvalue(L,BA_ENV_IX);
   lua_pushvalue(L,SQL_ENV_IX);
   luaL_setfuncs(L,driver,2);
   luaL_setfuncs(L,globals,0);
   lua_pop(L,1);

   luaL_newmetatable(L, BASQLWEAKKEY);
   lua_pushliteral(L, "__mode");
   lua_pushliteral(L, "k");
   lua_rawset(L,-3);
   lua_pop(L,1);

   return 0;
} 



/*
** Creates the metatables for the objects and registers the
** driver open method.
*/
LUASQL_API int luaopen_luasql_sqlite3(lua_State *L)
{
   if(sqlite3_threadsafe() == 0)
      luaL_error(L,"SQLite not thread safe");
      
   balua_pushbatab(L); /* BA_ENV_IX */
   lua_newtable(L); /* create (private) environment SQL_ENV_IX */
   lua_pushcclosure(L,initSqlitePrivData,2);
   lua_pcall(L,0,0,0);
   return 0;
}
