#define _CRT_SECURE_NO_WARNINGS
//utils.c — 工具函数实现
#include "utils.h"

 //终端辅助
void cls(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause_screen(void) {
    printf("\n  按 Enter 键继续..."); 
    fflush(stdout);
    getchar();
}

void print_line(char c, int n) {
    for (int i = 0; i < n; i++) putchar(c);
    putchar('\n');
}

void print_header(const char* title) {
    cls();
    print_line('=', 60);
    printf("    %s\n", title);
    print_line('=', 60);
}

//安全输入
void get_line(char* buf, int size) {
    if (fgets(buf, size, stdin) != NULL) {
        int len = (int)strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
    }
    else {
        buf[0] = '\0';
    }
}

int get_int(const char* prompt) {
    char buf[32];
    int val;
    while (1) {
        printf("%s", prompt); 
        fflush(stdout);
        get_line(buf, sizeof(buf));
        if (sscanf(buf, "%d", &val) == 1) return val;
        printf("  [!] 请输入有效整数\n");
    }
}

float get_float(const char* prompt) {
    char buf[32];
    float val;
    while (1) {
        printf("%s", prompt); fflush(stdout);
        get_line(buf, sizeof(buf));
        if (sscanf(buf, "%f", &val) == 1) return val;
        printf("  [!] 请输入有效数字\n");
    }
}

//时间工具
void get_current_time(char* buf, int size) {
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    strftime(buf, (size_t)size, "%Y-%m-%d %H:%M:%S", tm_info);
}
//状态字符串
const char* repair_status_str(int status) {
    switch (status) {
    case REPAIR_PENDING:    return "待处理";
    case REPAIR_ACCEPTED:   return "受理中";
    case REPAIR_DONE:       return "已完成";
    case REPAIR_REJECTED:   return "驳回  ";
    default:                return "未知  ";
    }
}

//排序实现

void sort_students_by_name(Student* arr, int n, int dir) {
    for (int i = 1; i < n; i++) {
        Student key = arr[i];
        int j = i - 1;
        while (j >= 0 && dir * strcmp(arr[j].name, key.name) > 0) {
            arr[j + 1] = arr[j]; j--;
        }
        arr[j + 1] = key;
    }
}

void sort_students_by_id(Student* arr, int n, int dir) {
    for (int i = 1; i < n; i++) {
        Student key = arr[i];
        int j = i - 1;
        while (j >= 0 && dir * strcmp(arr[j].student_id, key.student_id) > 0) {
            arr[j + 1] = arr[j]; j--;
        }
        arr[j + 1] = key;
    }
}

void sort_rooms_by_balance(Room* arr, int n, int dir) {
    for (int i = 1; i < n; i++) {
        Room key = arr[i];
        int j = i - 1;
        while (j >= 0 && dir * (arr[j].elec_balance > key.elec_balance ? 1 : -1) > 0) {
            arr[j + 1] = arr[j]; j--;
        }
        arr[j + 1] = key;
    }
}

void sort_hygiene_by_score(HygieneRecord* arr, int n, int dir) {
    for (int i = 1; i < n; i++) {
        HygieneRecord key = arr[i];
        int j = i - 1;
        while (j >= 0 && dir * (arr[j].score - key.score) > 0) {
            arr[j + 1] = arr[j]; j--;
        }
        arr[j + 1] = key;
    }
}

void sort_repairs_by_time(RepairRequest* arr, int n, int dir) {
    for (int i = 1; i < n; i++) {
        RepairRequest key = arr[i];
        int j = i - 1;
        while (j >= 0 && dir * strcmp(arr[j].submit_time, key.submit_time) > 0) {
            arr[j + 1] = arr[j]; j--;
        }
        arr[j + 1] = key;
    }
}

void sort_bills_by_id(ElecBill* arr, int n, int dir) {
    for (int i = 1; i < n; i++) {
        ElecBill key = arr[i];
        int j = i - 1;
        while (j >= 0 && dir * (arr[j].bill_id - key.bill_id) > 0) {
            arr[j + 1] = arr[j]; j--;
        }
        arr[j + 1] = key;
    }
}

//报表打印
void report_buildings(const DataStore* ds) {
    printf("\n  %-6s  %-20s  %s\n", "楼栋ID", "楼栋名称", "校区");
    print_line('-', 55);
    for (int i = 0; i < ds->building_count; i++) {
        const Building* b = &ds->buildings[i];
        printf("  %-6d  %-20s  %s\n",
            b->building_id, b->building_name, b->campus);
    }
    if (ds->building_count == 0) printf("  （暂无数据）\n");
    print_line('-', 55);
    printf("  共 %d 条记录\n", ds->building_count);
}

void report_rooms(const DataStore* ds) {
    printf("\n  %-6s  %-6s  %-8s  %-4s  %-4s  %-10s  %s\n",
        "房间ID", "楼栋ID", "房号", "容量", "人数", "余额(元)", "阈值(元)");
    print_line('-', 60);
    for (int i = 0; i < ds->room_count; i++) {
        const Room* r = &ds->rooms[i];
        const char* flag = "";
        if (r->elec_balance <= 0) flag = " [欠费]";
        else if (r->elec_balance <= r->elec_alert_threshold) flag = " [预警]";
        printf("  %-6d  %-6d  %-8s  %-4d  %-4d  %-10.2f  %.2f%s\n",
            r->room_id, r->building_id, r->room_number,
            r->capacity, r->current_count,
            r->elec_balance, r->elec_alert_threshold, flag);
    }
    if (ds->room_count == 0) printf("  （暂无数据）\n");
    print_line('-', 60);
    printf("  共 %d 条记录\n", ds->room_count);
}

void report_students(const DataStore* ds) {
    printf("\n  %-12s  %-10s  %-8s  %s\n", "学号", "姓名", "房间ID", "床号");
    print_line('-', 45);
    for (int i = 0; i < ds->student_count; i++) {
        const Student* s = &ds->students[i];
        if (s->room_id == -1) printf("  %-12s  %-10s  %-8s  —\n",s->student_id, s->name, "未分配");
        else printf("  %-12s  %-10s  %-8d  %d\n",s->student_id, s->name, s->room_id, s->bed_number);
    }
    if (ds->student_count == 0) printf("  （暂无数据）\n");
    print_line('-', 45);
    printf("  共 %d 条记录\n", ds->student_count);
}

void report_managers(const DataStore* ds) {
    printf("\n  %-12s  %-10s  %s\n", "工号", "姓名", "负责楼栋");
    print_line('-', 45);
    for (int i = 0; i < ds->manager_count; i++) {
        const DormManager* m = &ds->managers[i];
        /* 找楼栋名 */
        const char* bname = "未分配";
        for (int j = 0; j < ds->building_count; j++)
            if (ds->buildings[j].building_id == m->building_id) {
                bname = ds->buildings[j].building_name; break;
            }
        printf("  %-12s  %-10s  %s\n", m->manager_id, m->name, bname);
    }
    if (ds->manager_count == 0) printf("  （暂无数据）\n");
    print_line('-', 45);
    printf("  共 %d 条记录\n", ds->manager_count);
}

void report_repairs(const DataStore* ds) {
    printf("\n  %-5s  %-12s  %-8s  %-20s  %s\n",
        "单号", "学号", "状态", "提交时间", "描述（前25字）");
    print_line('-', 75);
    for (int i = 0; i < ds->repair_count; i++) {
        const RepairRequest* r = &ds->repairs[i];
        printf("  %-5d  %-12s  %-8s  %-20s  %.25s\n",
            r->repair_id, r->student_id,
            repair_status_str(r->status), r->submit_time,
            r->description);
    }
    if (ds->repair_count == 0) printf("  （暂无数据）\n");
    print_line('-', 75);
    printf("  共 %d 条记录\n", ds->repair_count);
}

void report_visitors(const DataStore* ds) {
    printf("\n  %-6s  %-12s  %-12s  %-20s  %s\n",
        "单号", "被访学号", "访客姓名", "来访时间", "离开时间");
    print_line('-', 70);
    for (int i = 0; i < ds->visitor_count; i++) {
        const VisitorRecord* v = &ds->visitors[i];
        printf("  %-6d  %-12s  %-12s  %-20s  %s\n",
            v->visitor_id, v->student_id,
            v->visitor_name, v->visit_time, v->leave_time);
    }
    if (ds->visitor_count == 0) printf("  （暂无数据）\n");
    print_line('-', 70);
    printf("  共 %d 条记录\n", ds->visitor_count);
}

void report_hygiene(const DataStore* ds) {
    printf("\n  %-6s  %-8s  %-6s  %-20s  %s\n",
        "记录ID", "房间ID", "评分", "检查时间", "备注");
    print_line('-', 60);
    for (int i = 0; i < ds->hygiene_count; i++) {
        const HygieneRecord* h = &ds->hygiene[i];
        printf("  %-6d  %-8d  %-6d  %-20s  %s\n",
            h->hygiene_id, h->room_id, h->score,
            h->check_time, h->remark);
    }
    if (ds->hygiene_count == 0) printf("  （暂无数据）\n");
    print_line('-', 60);
    printf("  共 %d 条记录\n", ds->hygiene_count);
}

void report_violations(const DataStore* ds) {
    printf("\n  %-6s  %-12s  %-16s  %-20s  %s\n",
        "编号", "学号", "类型", "时间", "描述（前25字）");
    print_line('-', 75);
    for (int i = 0; i < ds->violation_count; i++) {
        const ViolationRecord* v = &ds->violations[i];
        printf("  %-6d  %-12s  %-16s  %-20s  %.25s\n",
            v->violation_id, v->student_id,
            v->type, v->record_time, v->description);
    }
    if (ds->violation_count == 0) printf("  （暂无数据）\n");
    print_line('-', 75);
    printf("  共 %d 条记录\n", ds->violation_count);
}

void report_bills(const DataStore* ds) {
    printf("\n  %-6s  %-8s  %-10s  %-10s  %-10s  %s\n",
        "单号", "房间ID", "上期(度)", "本期(度)", "用电量", "抄表时间");
    print_line('-', 65);
    for (int i = 0; i < ds->bill_count; i++) {
        const ElecBill* b = &ds->bills[i];
        printf("  %-6d  %-8d  %-10.1f  %-10.1f  %-10.1f  %s\n",
            b->bill_id, b->room_id,
            b->last_reading, b->curr_reading,
            b->curr_reading - b->last_reading,
            b->record_time);
    }
    if (ds->bill_count == 0) printf("  （暂无数据）\n");
    print_line('-', 65);
    printf("  共 %d 条记录\n", ds->bill_count);
}
