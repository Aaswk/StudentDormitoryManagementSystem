//student.h — 学生端功能接口
#ifndef STUDENT_H
#define STUDENT_H

#include "structs.h"

 /* 学生主菜单入口（登录后调用） */
void student_menu(DataStore* ds, Student* s);

/* 各功能子函数（供 menu 调用） */
void student_view_dorm(DataStore* ds, Student* s);
void student_submit_repair(DataStore* ds, Student* s);
void student_view_repairs(DataStore* ds, Student* s);
void student_view_elec(DataStore* ds, Student* s);
void student_submit_visitor(DataStore* ds, Student* s);
void student_view_visitors(DataStore* ds, Student* s);
void student_view_hygiene(DataStore* ds, Student* s);
void student_view_violations(DataStore* ds, Student* s);
void student_change_password(DataStore* ds, Student* s);

#endif /* STUDENT_H */
