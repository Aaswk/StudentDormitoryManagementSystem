#define _CRT_SECURE_NO_WARNINGS
//file.c — 文件读写实现（二进制 fread/fwrite）
#include "file.h"

 //通用 load/save 宏
#define LOAD_TABLE(file, arr, cnt, Type) do { \
    FILE *_f = fopen((file), "rb"); \
    if (!_f) { (cnt) = 0; break; } \
    if (fread(&(cnt), sizeof(int), 1, _f) != 1) { (cnt) = 0; fclose(_f); break; } \
    if ((cnt) < 0) { (cnt) = 0; fclose(_f); break; } \
    if ((cnt) > 0) { \
        if (fread((arr), sizeof(Type), (size_t)(cnt), _f) != (size_t)(cnt)) \
            (cnt) = 0; \
    } \
    fclose(_f); \
} while (0)

#define SAVE_TABLE(file, arr, cnt, Type) do { \
    FILE *_f = fopen((file), "wb"); \
    if (!_f) break; \
    fwrite(&(cnt), sizeof(int), 1, _f); \
    if ((cnt) > 0) fwrite((arr), sizeof(Type), (size_t)(cnt), _f); \
    fclose(_f); \
} while (0)

//重算自增 ID
static void recalc_ids(DataStore* ds) {
    int i;
    IdCounters* ids = &ds->ids;

    ids->next_building_id = 1;
    for (i = 0; i < ds->building_count; i++)
        if (ds->buildings[i].building_id >= ids->next_building_id)
            ids->next_building_id = ds->buildings[i].building_id + 1;

    ids->next_room_id = 1;
    for (i = 0; i < ds->room_count; i++)
        if (ds->rooms[i].room_id >= ids->next_room_id)
            ids->next_room_id = ds->rooms[i].room_id + 1;

    ids->next_repair_id = 1;
    for (i = 0; i < ds->repair_count; i++)
        if (ds->repairs[i].repair_id >= ids->next_repair_id)
            ids->next_repair_id = ds->repairs[i].repair_id + 1;

    ids->next_visitor_id = 1;
    for (i = 0; i < ds->visitor_count; i++)
        if (ds->visitors[i].visitor_id >= ids->next_visitor_id)
            ids->next_visitor_id = ds->visitors[i].visitor_id + 1;

    ids->next_hygiene_id = 1;
    for (i = 0; i < ds->hygiene_count; i++)
        if (ds->hygiene[i].hygiene_id >= ids->next_hygiene_id)
            ids->next_hygiene_id = ds->hygiene[i].hygiene_id + 1;

    ids->next_violation_id = 1;
    for (i = 0; i < ds->violation_count; i++)
        if (ds->violations[i].violation_id >= ids->next_violation_id)
            ids->next_violation_id = ds->violations[i].violation_id + 1;

    ids->next_bill_id = 1;
    for (i = 0; i < ds->bill_count; i++)
        if (ds->bills[i].bill_id >= ids->next_bill_id)
            ids->next_bill_id = ds->bills[i].bill_id + 1;
}

//初始化：内置默认管理员
static void init_default_admin(DataStore* ds) {
    if (ds->admin_count > 0) return;
    Admin* a = &ds->admins[0];
    memset(a, 0, sizeof(*a));
    strncpy(a->username, "admin", sizeof(a->username) - 1);
    memcpy(a->name, "系统管理员", 15);
    a->name[15] = '\0';
    //明文 "admin123" 加密存储
    encrypt_password("admin123", a->password);
    ds->admin_count = 1;
    SAVE_TABLE(FILE_ADMINS, ds->admins, ds->admin_count, Admin);
}

//公开接口
void load_all(DataStore* ds) {
    //MKDIR("data");
    LOAD_TABLE(FILE_BUILDINGS, ds->buildings, ds->building_count, Building);
    LOAD_TABLE(FILE_ROOMS, ds->rooms, ds->room_count, Room);
    LOAD_TABLE(FILE_STUDENTS, ds->students, ds->student_count, Student);
    LOAD_TABLE(FILE_MANAGERS, ds->managers, ds->manager_count, DormManager);
    LOAD_TABLE(FILE_REPAIRS, ds->repairs, ds->repair_count, RepairRequest);
    LOAD_TABLE(FILE_VISITORS, ds->visitors, ds->visitor_count, VisitorRecord);
    LOAD_TABLE(FILE_HYGIENE, ds->hygiene, ds->hygiene_count, HygieneRecord);
    LOAD_TABLE(FILE_VIOLATIONS, ds->violations, ds->violation_count, ViolationRecord);
    LOAD_TABLE(FILE_ELEC_BILLS, ds->bills, ds->bill_count, ElecBill);
    LOAD_TABLE(FILE_ADMINS, ds->admins, ds->admin_count, Admin);
    recalc_ids(ds);
    init_default_admin(ds);
}

void save_all(const DataStore* ds) {
    SAVE_TABLE(FILE_BUILDINGS, ds->buildings, ds->building_count, Building);
    SAVE_TABLE(FILE_ROOMS, ds->rooms, ds->room_count, Room);
    SAVE_TABLE(FILE_STUDENTS, ds->students, ds->student_count, Student);
    SAVE_TABLE(FILE_MANAGERS, ds->managers, ds->manager_count, DormManager);
    SAVE_TABLE(FILE_REPAIRS, ds->repairs, ds->repair_count, RepairRequest);
    SAVE_TABLE(FILE_VISITORS, ds->visitors, ds->visitor_count, VisitorRecord);
    SAVE_TABLE(FILE_HYGIENE, ds->hygiene, ds->hygiene_count, HygieneRecord);
    SAVE_TABLE(FILE_VIOLATIONS, ds->violations, ds->violation_count, ViolationRecord);
    SAVE_TABLE(FILE_ELEC_BILLS, ds->bills, ds->bill_count, ElecBill);
    SAVE_TABLE(FILE_ADMINS, ds->admins, ds->admin_count, Admin);
}

void save_buildings(const DataStore* ds) { 
    SAVE_TABLE(FILE_BUILDINGS, ds->buildings, ds->building_count, Building); 
}
void save_rooms(const DataStore* ds) { 
    SAVE_TABLE(FILE_ROOMS, ds->rooms, ds->room_count, Room); 
}
void save_students(const DataStore* ds) { 
    SAVE_TABLE(FILE_STUDENTS, ds->students, ds->student_count, Student); 
}
void save_managers(const DataStore* ds) { 
    SAVE_TABLE(FILE_MANAGERS, ds->managers, ds->manager_count, DormManager); 
}
void save_repairs(const DataStore* ds) { 
    SAVE_TABLE(FILE_REPAIRS, ds->repairs, ds->repair_count, RepairRequest); 
}
void save_visitors(const DataStore* ds) { 
    SAVE_TABLE(FILE_VISITORS, ds->visitors, ds->visitor_count, VisitorRecord); 
}
void save_hygiene(const DataStore* ds) { 
    SAVE_TABLE(FILE_HYGIENE, ds->hygiene, ds->hygiene_count, HygieneRecord); 
}
void save_violations(const DataStore* ds) { 
    SAVE_TABLE(FILE_VIOLATIONS, ds->violations, ds->violation_count, ViolationRecord); 
}
void save_bills(const DataStore* ds) { 
    SAVE_TABLE(FILE_ELEC_BILLS, ds->bills, ds->bill_count, ElecBill); 
}
void save_admins(const DataStore* ds) { 
    SAVE_TABLE(FILE_ADMINS, ds->admins, ds->admin_count, Admin); 
}

//查找工具
Building* find_building(DataStore* ds, int id) {
    for (int i = 0; i < ds->building_count; i++) {
        if (ds->buildings[i].building_id == id) return &ds->buildings[i];
    }
    return NULL;
}

Room* find_room(DataStore* ds, int id) {
    for (int i = 0; i < ds->room_count; i++) {
        if (ds->rooms[i].room_id == id) return &ds->rooms[i];
    }
    return NULL;
}

Student* find_student(DataStore* ds, const char* sid) {
    for (int i = 0; i < ds->student_count; i++) {
        if (strcmp(ds->students[i].student_id, sid) == 0) return &ds->students[i];
    }
    return NULL;
}

DormManager* find_manager(DataStore* ds, const char* mid) {
    for (int i = 0; i < ds->manager_count; i++) {
        if (strcmp(ds->managers[i].manager_id, mid) == 0) return &ds->managers[i];
    }
    return NULL;
}
