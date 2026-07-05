//file.h — 文件读写接口声明
#ifndef FILE_IO_H
#define FILE_IO_H

#include "structs.h"

 /* 一次性加载所有数据，初始化 DataStore */
void load_all(DataStore* ds);

/* 一次性保存所有数据 */
void save_all(const DataStore* ds);

/* 单表保存（操作后即时刷盘） */
void save_buildings(const DataStore* ds);
void save_rooms(const DataStore* ds);
void save_students(const DataStore* ds);
void save_managers(const DataStore* ds);
void save_repairs(const DataStore* ds);
void save_visitors(const DataStore* ds);
void save_hygiene(const DataStore* ds);
void save_violations(const DataStore* ds);
void save_bills(const DataStore* ds);
void save_admins(const DataStore* ds);

/* 查找工具函数 */
Building* find_building(DataStore* ds, int id);
Room* find_room(DataStore* ds, int id);
Student* find_student(DataStore* ds, const char* sid);
DormManager* find_manager(DataStore* ds, const char* mid);

#endif /* FILE_IO_H */
