//structs.h — 所有宏定义、枚举、结构体、DataStore

#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

 //容量上限
#define MAX_BUILDINGS    50
#define MAX_ROOMS       500
#define MAX_STUDENTS    500
#define MAX_MANAGERS     50
#define MAX_REPAIRS    1000
#define MAX_VISITORS   1000
#define MAX_HYGIENE    1000
#define MAX_VIOLATIONS 1000
#define MAX_BILLS      1000
#define MAX_ADMINS       10

//数据文件路径
#define FILE_BUILDINGS  "data/buildings.dat"
#define FILE_ROOMS      "data/rooms.dat"
#define FILE_STUDENTS   "data/students.dat"
#define FILE_MANAGERS   "data/managers.dat"
#define FILE_REPAIRS    "data/repairs.dat"
#define FILE_VISITORS   "data/visitors.dat"
#define FILE_HYGIENE    "data/hygiene.dat"
#define FILE_VIOLATIONS "data/violations.dat"
#define FILE_ELEC_BILLS "data/elec_bills.dat"
#define FILE_ADMINS     "data/admins.dat"

//报修状态枚举
#define REPAIR_PENDING    0   /* 待处理 */
#define REPAIR_ACCEPTED   1   /* 受理中 */
#define REPAIR_DONE       2   /* 已完成 */
#define REPAIR_REJECTED   3   /* 驳回   */

//基础结构体
//楼栋
typedef struct {
    int  building_id;
    char campus[32];//校区
    char building_name[32];//楼名
} Building;

//房间
typedef struct {
    int   room_id;
    int   building_id;
    char  room_number[8];
    int   capacity;
    int   current_count;
    float elec_balance;
    float elec_alert_threshold;
} Room;

//学生
typedef struct {
    char student_id[12];
    char name[16];
    int  room_id;       /* -1 = 未分配 */
    int  bed_number;
    char password[32];  /* Caesar +3 加密存储 */
} Student;

//宿管
typedef struct {
    char manager_id[12];
    char name[16];
    int  building_id;   /* -1 = 未分配 */
    char password[32];  /* Caesar +3 加密存储 */
} DormManager;

//报修申请
typedef struct {
    int  repair_id;
    char student_id[12];
    char description[128];
    int  status;        /* REPAIR_* 宏 */
    char submit_time[20];
    char update_time[20];
} RepairRequest;

//访客记录
typedef struct {
    int  visitor_id;
    char student_id[12];
    char visitor_name[16];
    char visit_time[20];
    char leave_time[20];
} VisitorRecord;

//卫生检查
typedef struct {
    int  hygiene_id;
    int  room_id;
    int  score;
    char remark[64];
    char check_time[20];
} HygieneRecord;

//违规记录
typedef struct {
    int  violation_id;
    char student_id[12];
    char type[32];
    char description[128];
    char record_time[20];
} ViolationRecord;

//电费账单
typedef struct {
    int   bill_id;
    int   room_id;
    float last_reading;
    float curr_reading;
    char  record_time[20];
} ElecBill;

//管理员
typedef struct {
    char username[20];
    char password[32];
    char name[16];
} Admin;

//自增 ID 计数器（随 DataStore 一起传递
typedef struct {
    int next_building_id;
    int next_room_id;
    int next_repair_id;
    int next_visitor_id;
    int next_hygiene_id;
    int next_violation_id;
    int next_bill_id;
} IdCounters;

//DataStore（代替全局变量
typedef struct {
    Building buildings[MAX_BUILDINGS];
    int building_count;

    Room rooms[MAX_ROOMS];
    int  room_count;

    Student students[MAX_STUDENTS];
    int student_count;

    DormManager managers[MAX_MANAGERS];
    int manager_count;

    RepairRequest repairs[MAX_REPAIRS];
    int repair_count;

    VisitorRecord visitors[MAX_VISITORS];
    int visitor_count;

    HygieneRecord hygiene[MAX_HYGIENE];
    int hygiene_count;

    ViolationRecord violations[MAX_VIOLATIONS];
    int violation_count;

    ElecBill bills[MAX_BILLS];
    int bill_count;

    Admin  admins[MAX_ADMINS];
    int admin_count;

    IdCounters ids;
} DataStore;

//密码加解密（偏移 +3
static inline void encrypt_password(const char* plain, char* cipher) {
    int i;
    for (i = 0; plain[i] != '\0'; i++) {
        cipher[i] = (char)(plain[i] + 3);
    }
    cipher[i] = '\0';
}

static inline int check_password(const char* plain, const char* cipher) {
    char enc[32];
    encrypt_password(plain, enc);
    return strcmp(enc, cipher) == 0;
}

#endif /* STRUCTS_H */
