#define _CRT_SECURE_NO_WARNINGS
//main.c — 程序入口
//DataStore 在堆上分配，所有模块通过指针共享，无全局变量。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#  include <windows.h>
#endif

#include "structs.h"
#include "file.h"
#include "auth.h"
#include "utils.h"
#include "student.h"
#include "manager.h"
#include "admin.h"

#define MAX_ATTEMPTS 3

 //学生登录
static void login_student(DataStore* ds) {
    char sid[20], pwd[64];
    for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
        print_header("学生登录");
        printf("  学号："); 
        fflush(stdout);
        get_line(sid, sizeof(sid));
        printf("  密码："); 
        fflush(stdout);
        get_line(pwd, sizeof(pwd));

        Student* s = auth_login_student(ds, sid, pwd);
        if (s) {
            student_menu(ds, s);
            return;
        }
        printf("  学号或密码错误（%d/%d）。\n", attempt, MAX_ATTEMPTS);
        pause_screen();
    }
    printf("  登录失败次数过多，已锁定本次会话。\n");
    pause_screen();
}

//宿管登录
static void login_manager(DataStore* ds) {
    char mid[20], pwd[64];
    for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
        print_header("宿管登录");
        printf("  工号："); 
        fflush(stdout);
        get_line(mid, sizeof(mid));
        printf("  密码："); 
        fflush(stdout);
        get_line(pwd, sizeof(pwd));

        DormManager* m = auth_login_manager(ds, mid, pwd);
        if (m) {
            manager_menu(ds, m);
            return;
        }
        printf("  工号或密码错误（%d/%d）。\n", attempt, MAX_ATTEMPTS);
        pause_screen();
    }
    printf("  登录失败次数过多，已锁定本次会话。\n");
    pause_screen();
}

//管理员登录
static void login_admin(DataStore* ds) {
    char uname[20], pwd[64];
    for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
        print_header("管理员登录");
        printf("  账号："); 
        fflush(stdout);
        get_line(uname, sizeof(uname));
        printf("  密码："); 
        fflush(stdout);
        get_line(pwd, sizeof(pwd));

        Admin* a = auth_login_admin(ds, uname, pwd);
        if (a) {
            admin_menu(ds, a);
            return;
        }
        printf("  账号或密码错误（%d/%d）。\n", attempt, MAX_ATTEMPTS);
        pause_screen();
    }
    printf("  登录失败次数过多，已锁定本次会话。\n");
    pause_screen();
}

//主入口
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    //在堆上分配 DataStore，避免栈溢出（约 40+ MB
    DataStore* ds = (DataStore*)calloc(1, sizeof(DataStore));
    if (!ds) {
        fprintf(stderr, "内存分配失败，程序退出。\n");
        return 1;
    }

    //初始化自增 ID 起始值
    ds->ids.next_building_id = 1;
    ds->ids.next_room_id = 1;
    ds->ids.next_repair_id = 1;
    ds->ids.next_visitor_id = 1;
    ds->ids.next_hygiene_id = 1;
    ds->ids.next_violation_id = 1;
    ds->ids.next_bill_id = 1;

    //加载持久化数据
    load_all(ds);

    int choice;
    do {
        print_header("学生宿舍管理系统");
        printf("  请选择登录角色：\n\n");
        printf("  [1]  学生登录\n");
        printf("  [2]  宿管登录\n");
        printf("  [3]  管理员登录\n");
        printf("  [0]  退出系统\n");
        print_line('-', 60);
        choice = get_int("  请选择：");

        switch (choice) {
        case 1: login_student(ds); break;
        case 2: login_manager(ds); break;
        case 3: login_admin(ds);   break;
        case 0: break;
        default:
            printf("  无效选项，请重新选择。\n");
            pause_screen();
        }
    } while (choice != 0);

    //退出前保存所有数据
    save_all(ds);
    free(ds);

    cls();
    printf("  感谢使用学生宿舍管理系统，再见！\n");
    return 0;
}
