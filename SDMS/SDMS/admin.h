//admin.h — 管理员端功能接口
#ifndef ADMIN_H
#define ADMIN_H

#include "structs.h"

 //管理员主菜单入口
void admin_menu(DataStore* ds, Admin* a);

//各管理子模块
void admin_manage_buildings(DataStore* ds);
void admin_manage_rooms(DataStore* ds);
void admin_manage_students(DataStore* ds);
void admin_manage_managers(DataStore* ds);
void admin_assign_dorm(DataStore* ds);
void admin_statistics(DataStore* ds);

//组合查询
void admin_query_students(DataStore* ds);    /* 按姓名+楼栋*/
void admin_query_repairs(DataStore* ds);     /* 按学号+状态*/
void admin_query_hygiene(DataStore* ds);     /* 按房间+评分区间*/
void admin_query_violations(DataStore* ds);  /* 按学号+违规类型*/

//排序报表
void admin_sort_and_report(DataStore* ds);

//管理员账号管理
void admin_manage_admins(DataStore* ds, Admin* current);

#endif /* ADMIN_H */
