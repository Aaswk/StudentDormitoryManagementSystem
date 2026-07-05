//utils.h — 工具函数声明（I/O、格式化、排序、报表）
#ifndef UTILS_H
#define UTILS_H

#include "structs.h"

 //终端辅助
void cls(void);
void pause_screen(void);
void print_line(char c, int n);
void print_header(const char* title);

//安全输入
void  get_line(char* buf, int size);
int   get_int(const char* prompt);
float get_float(const char* prompt);

//时间工具
void get_current_time(char* buf, int size);

//状态字符串
const char* repair_status_str(int status);

//排序（dir: 1=升序  -1=降序）
/* 学生按姓名排序 */
void sort_students_by_name(Student* arr, int n, int dir);
/* 学生按学号排序 */
void sort_students_by_id(Student* arr, int n, int dir);
/* 房间按余额排序 */
void sort_rooms_by_balance(Room* arr, int n, int dir);
/* 卫生记录按评分排序 */
void sort_hygiene_by_score(HygieneRecord* arr, int n, int dir);
/* 报修记录按提交时间排序 */
void sort_repairs_by_time(RepairRequest* arr, int n, int dir);
/* 电费账单按单号排序 */
void sort_bills_by_id(ElecBill* arr, int n, int dir);

//报表打印（格式化表头+数据）
void report_buildings(const DataStore* ds);
void report_rooms(const DataStore* ds);
void report_students(const DataStore* ds);
void report_managers(const DataStore* ds);
void report_repairs(const DataStore* ds);
void report_visitors(const DataStore* ds);
void report_hygiene(const DataStore* ds);
void report_violations(const DataStore* ds);
void report_bills(const DataStore* ds);

#endif /* UTILS_H */
