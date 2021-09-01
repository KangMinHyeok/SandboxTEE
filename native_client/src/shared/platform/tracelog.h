#ifndef TRACELOG_H
#define TRACELOG_H

struct TraceLog {
    int total_len;
    int msg_type;
    int date_len;
    char *date;
    int time_len;
    char *time;
    int svc_id_len;
    char *svc_id;
    int agent_id_len;
    char *agent_id;
    int device_id_len;
    char *device_id;
    int file_name_len;
    const char* file_name;
    int io_mode;
    int result;
} trace_log;


#endif
