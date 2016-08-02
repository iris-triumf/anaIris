
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

#define SPEC_LENGTH 4096
#define SPEC_LENGTH_2D 128
#define NHITS 32
#define NCHANNELS 128
#define MAXLEN 1024
#define MAXODBPATHS 128
#define MAXSPECNAMES 128
#define SIZE_OF_ODB_MSC_TABLE 1024
#define NSPECS (SIZE_OF_ODB_MSC_TABLE)

// Declaration of the spectrum store
extern int spec_store_address[NSPECS];
extern int spec_store_type[NSPECS]; // 0=energy, 1=time, 2=waveform, 3=pulse height, 4=hitpattern
extern char spec_store_eName[NSPECS][32];
extern char spec_store_tName[NSPECS][32];
extern char spec_store_energyName[NSPECS][32];
extern char spec_store_adcName[NSPECS][32];
extern char spec_store_2dhitName[NSPECS][32];
extern char spec_store_pidName[NSPECS][32];
extern char spec_store_Sname[NSPECS][32];
extern int spec_store_eData[NSPECS][SPEC_LENGTH];
extern int spec_store_tData[NSPECS][SPEC_LENGTH];
extern int spec_store_energyData[NSPECS][SPEC_LENGTH];
extern int spec_store_adcData[NSPECS][SPEC_LENGTH];
extern int spec_store_2dhitData[NSPECS][NCHANNELS*SPEC_LENGTH_2D];
extern int spec_store_pidData[NSPECS][SPEC_LENGTH];
extern int spec_store_Sdata[NSPECS][SPEC_LENGTH];
extern int spec_store_hit_type[NHITS];
extern char spec_store_hitName[NHITS][32];
extern int spec_store_hitData[NHITS][NCHANNELS];
extern char spec_store_sum_name[NHITS][32];
extern int spec_store_sum_data[NHITS][SPEC_LENGTH];



