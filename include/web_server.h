
#define MAX_QUEUE_LEN     4

#define REQUEST_TIMEOUT 10 /* 10 seconds */

void web_server_main(int *arg);
int handle_connection(int fd);
int send_test_html(int fd);
int parse_url(int fd, int *cmd, int *arg);
int get_request(int fd);
int get_line(int fd, char *buf, int maxlen);
int put_line(int fd, char *buf, int len);
int parse_line(char *buf, int first);
int remove_trailing_space(char *buf);
int send_header(int fd, int type);
int send_file(char *filename, int fd);
int send_spectrum_list(int fd);
int send_spectrum_from_handler(int num, int fd);
void load_spec_table_from_ODB();
int GetIDfromAddress(int address);
int GetIDfromName(const char* name);
void decodeurl(char *dst, const char *src);

#define WEBPORT           9094
#define ROOTDIR "/home/iris/anaIris"

#define SPEC_LENGTH 8192
#define NHITS 6
#define MAXLEN 1024
#define MAXODBPATHS 128
#define MAXSPECNAMES 128
#define SIZE_OF_ODB_MSC_TABLE 2
#define NSPECS (SIZE_OF_ODB_MSC_TABLE)

// Declaration of the spectrum store
 extern int spec_store_address[NSPECS];
 extern int spec_store_type[NSPECS]; // 0=energy, 1=time, 2=waveform, 3=pulse height, 4=hitpattern
 extern char spec_store_Ename[NSPECS][32];
 extern char spec_store_Tname[NSPECS][32];
 extern char spec_store_Wname[NSPECS][32];
 extern char spec_store_Pname[NSPECS][32];
 extern char spec_store_Zname[NSPECS][32];
 extern char spec_store_Lname[NSPECS][32];
 extern char spec_store_Sname[NSPECS][32];
 extern int spec_store_Edata[NSPECS][SPEC_LENGTH];
 extern int spec_store_Tdata[NSPECS][SPEC_LENGTH];
 extern int spec_store_Wdata[NSPECS][SPEC_LENGTH];
 extern int spec_store_Pdata[NSPECS][SPEC_LENGTH];
 extern int spec_store_Zdata[NSPECS][SPEC_LENGTH];
 extern int spec_store_Ldata[NSPECS][SPEC_LENGTH];
 extern int spec_store_Sdata[NSPECS][SPEC_LENGTH];
 extern int spec_store_hit_type[NHITS];
 extern char spec_store_hit_name[NHITS][32];
 extern int spec_store_hit_data[NHITS][SIZE_OF_ODB_MSC_TABLE];
 extern char spec_store_sum_name[NHITS][32];
extern int spec_store_sum_data[NHITS][SPEC_LENGTH];



