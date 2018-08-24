typedef struct st_mysql_res {
my_ulonglong row_count;	           // 结果集的行数
unsigned int	field_count, current_field;  // 结果集的列数，当前列
MYSQL_FIELD	*fields;	               // 结果集的列信息
MYSQL_DATA	*data;	               // 结果集的数据
MYSQL_ROWS	*data_cursor;	           // 结果集的光标
MEM_ROOT	field_alloc;              // 内存结构
MYSQL_ROW	row;	                   // 非缓冲的时候用到
MYSQL_ROW	current_row;            //mysql_store_result时会用到。当前行
unsigned long *lengths;	               //每列的长度
MYSQL	*handle;	                   // mysql_use_result会用。
my_bool	eof;	                       //是否为行尾
} MYSQL_RES;
 
typedef char **MYSQL_ROW;	/* 返回的每一行的值，全部用字符串来表示*/
typedef struct st_mysql_rows {
struct st_mysql_rows *next;	/* list of rows */
  MYSQL_ROW data;
} MYSQL_ROWS;              //mysql的数据的链表节点。可见mysql的结果集是链表结构
 
 
typedef struct st_mysql_data {
  my_ulonglong rows;
  unsigned int fields;
  MYSQL_ROWS *data;
  MEM_ROOT alloc;
} MYSQL_DATA; // 数据集的结构
 
typedef struct st_mysql_field {
  char *name;					  /* Name of column */
  char *table;	                      /* Table of column if column was a field */
  char *def;	                      /* Default value (set by mysql_list_fields) */
  enum enum_field_types type;	      /* Type of field. Se mysql_com.h for types */
  unsigned int length;	              /* Width of column */
  unsigned int max_length;	          /* Max width of selected set */
  unsigned int flags;	              /* Div flags */
  unsigned int decimals;	          /* Number of decimals in field */
} MYSQL_FIELD;  //列信息的结构
 typedef struct st_used_mem {    	/* struct for once_alloc */
struct st_used_mem *next;	    /* Next block in use */
  unsigned int left;             	/* memory left in block  */
  unsigned int size;             	/* Size of block */
} USED_MEM; //内存结构
 
typedef struct st_mem_root {
  USED_MEM *free;
  USED_MEM *used;
  USED_MEM *pre_alloc;
  unsigned int	min_malloc;
  unsigned int	block_size;
  void (*error_handler)(void);
} MEM_ROOT;  //内存结构
之所以列出这些结构，是想让大家对mysql的结果集有更清晰的认识。

 

3.以下是取得数据的相关API:

 

（1）mysql_store_result() //将结果集实际读取到客户端

MYSQL_RES *mysql_store_result(MYSQL *mysql)

对于成功检索了数据的每个查询（SELECT、SHOW、DESCRIBE、EXPLAIN、CHECK TABLE等），必须调用mysql_store_result()或mysql_use_result() 。

对于其他查询，不需要调用mysql_store_result()或mysql_use_result()，但是如果在任何情况下均调用了mysql_store_result()，它也不会导致任何伤害或性能降低。通过检查mysql_store_result()是否返回0，可检测查询是否没有结果集（以后会更多）。

 

如果希望了解查询是否应返回结果集，可使用mysql_field_count()进行检查。

mysql_store_result()将查询的全部结果读取到客户端，分配1个MYSQL_RES结构，并将结果置于该结构中。如果查询未返回结果集，mysql_store_result()将返回Null指针。

如果读取结果集失败，mysql_store_result()还会返回Null指针。通过检查mysql_error()是否返回非空字符串，mysql_errno()是否返回非0值，或mysql_field_count()是否返回0，可以检查是否出现了错误。

如果未返回行，将返回空的结果集。（空结果集设置不同于作为返回值的空指针）。

 

一旦调用了mysql_store_result()并获得了不是Null指针的结果，可调用mysql_num_rows()来找出结果集中的行数。

 

可以调用mysql_fetch_row()来获取结果集中的行，或调用mysql_row_seek()和mysql_row_tell()来获取或设置结果集中的当前行位置。

一旦完成了对结果集的操作，必须调用mysql_free_result()。

 

（2） mysql_use_result()//初始化结果集检索

MYSQL_RES *mysql_use_result(MYSQL *mysql)

对于成功检索数据的每个查询（SELECT、SHOW、DESCRIBE、EXPLAIN），必须用mysql_store_result()或mysql_use_result()。

mysql_use_result()将初始化结果集检索，但并不像mysql_store_result()那样将结果集实际读取到客户端。它必须通过对mysql_fetch_row()的调用，对每一行分别进行检索。这将直接从服务器读取结果，而不会将其保存在临时表或本地缓冲区内，与mysql_store_result()相比，速度更快而且使用的内存也更少。客户端仅为当前行和通信缓冲区分配内存，分配的内存可增加到max_allowed_packet字节。

 

另一方面，如果你正在客户端一侧为各行进行大量的处理操作，或者将输出发送到了用户可能会键入“^S”（停止滚动）的屏幕，就不应使用mysql_use_result()。这会绑定服务器，并阻止其他线程更新任何表（数据从这类表获得）。

 

使用mysql_use_result()时，必须执行mysql_fetch_row()，直至返回NULL值，否则，未获取的行将作为下一个检索的一部分返回。C API给出命令不同步错误，如果忘记了执行该操作，将不能运行该命令。

 

不能对结果集用mysql_data_seek()、mysql_row_seek()、mysql_row_tell()、mysql_num_rows()或mysql_affected_rows()，也不应发出其他查询，直至mysql_use_result()完成为止。（但是，提取了所有行后，mysql_num_rows()将准确返回提取的行数）。

一旦完成了对结果集的操作，必须调用mysql_free_result()。

 

返回值：MYSQL_RES结果结构。如果出现错误，返回NULL。

 

（3）mysql_fetch_row()  //取得一行的数据：

MYSQL_ROW mysql_fetch_row(MYSQL_RES *result)

检索结果集的下一行。在mysql_store_result()之后使用时，如果没有要检索的行，mysql_fetch_row()返回NULL。在mysql_use_result()之后使用时，如果没有要检索的行或出现了错误，mysql_fetch_row()返回NULL。

结果集的列数目由mysql_num_fields(result)给出。如果行中保存了调用mysql_fetch_row()返回的值，将按照row[0]到row[mysql_num_fields(result)-1]，访问这些值的指针。行中的NULL值由NULL指针指明。

 

可以通过调用mysql_fetch_lengths()来获得行中字段值的长度。对于空字段以及包含NULL的字段，长度为0。通过检查字段值的指针，能够区分它们。如果指针为NULL，字段为NULL，否则字段为空。



返回值：下一行的MYSQL_ROW结构。如果没有更多要检索的行或出现了错误，返回NULL。

 

示例代码：

//假设mysql已经连接好
 
 MYSQL_RES* result = mysql_store_result(mysql);
if (result  == NULL)
 return;
 
 MYSQL_ROW row;
unsigned int num_fields;
unsigned int i;
num_fields = mysql_num_fields(result);
while ((row = mysql_fetch_row(result)))
{
   unsigned long *lengths;
   lengths = mysql_fetch_lengths(result);
   for(i = 0; i < num_fields; i++)
   {
       printf("[%.*s] ", (int) lengths[i], row[i] ? row[i] : "NULL");
   }
   printf("/n");
}

（4）mysql_fetch_field()  //取得列的信息

MYSQL_FIELD *mysql_fetch_field(MYSQL_RES *result)

返回采用MYSQL_FIELD结构的结果集的列。重复调用该函数，以检索关于结果集中所有列的信息。未剩余字段时，mysql_fetch_field()返回NULL。

每次执行新的SELECT查询时，将复位mysql_fetch_field()，以返回关于第1个字段的信息。调用mysql_field_seek()也会影响mysql_fetch_field()返回的字段。

 

如果调用了mysql_query()以在表上执行SELECT，但未调用mysql_store_result()，如果调用了mysql_fetch_field()以请求BLOB字段的长度，MySQL将返回默认的Blob长度（8KB）。之所以选择8KB是因为MySQL不知道BLOB的最大长度。应在日后使其成为可配置的。一旦检索了结果集，field->max_length将包含特定查询中该列的最大值的长度。

 

返回值：当前列的MYSQL_FIELD结构。如果未剩余任何列，返回NULL，错误

 

示例：

MYSQL_FIELD *field;

while((field = mysql_fetch_field(result)))

{

    printf("field name %s/n", field->name);

}

 

（5）mysql_fetch_fields()//同4.

MYSQL_FIELD *mysql_fetch_fields(MYSQL_RES *result)

对于结果集，返回所有MYSQL_FIELD结构的数组。每个结构提供了结果集中1列的字段定义。

 

返回值：关于结果集所有列的MYSQL_FIELD结构的数组。

 

示例：

 

unsigned int num_fields;

unsigned int i;

MYSQL_FIELD *fields;

 

num_fields = mysql_num_fields(result);

fields = mysql_fetch_fields(result);

for(i = 0; i < num_fields; i++)

{

   printf("Field %u is %s/n", i, fields[i].name);

}

 

（6） mysql_free_result()  //释放结果集分配的内存

void mysql_free_result(MYSQL_RES *result)

释放由mysql_store_result()、mysql_use_result()、mysql_list_dbs()等为结果集分配的内存。完成对结果集的操作后，必须调用mysql_free_result()释放结果集使用的内存。释放完成后，不要尝试访问结果集。永远记得要释放结果集！！！

