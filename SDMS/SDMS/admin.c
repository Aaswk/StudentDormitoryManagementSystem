#define _CRT_SECURE_NO_WARNINGS
//admin.c — 管理员端所有功能实现
#include "admin.h"
#include "file.h"
#include "utils.h"

//楼栋管理
void admin_manage_buildings(DataStore* ds) {
    int choice;
    do {
        print_header("楼栋管理");
        printf("  [1] 查看所有楼栋（报表）\n");
        printf("  [2] 新增楼栋\n");
        printf("  [3] 修改楼栋信息\n");
        printf("  [4] 删除楼栋\n");
        printf("  [0] 返回\n");
        print_line('-', 40);
        choice = get_int("  请选择：");

        if (choice == 1) {
            print_header("楼栋报表");
            report_buildings(ds);
            pause_screen();

        }
        else if (choice == 2) {
            if (ds->building_count >= MAX_BUILDINGS) {
                printf("  楼栋记录已满。\n"); 
                pause_screen(); 
                continue;
            }
            Building* b = &ds->buildings[ds->building_count];
            memset(b, 0, sizeof(*b));
            printf("  校区名称："); 
            fflush(stdout);
            get_line(b->campus, sizeof(b->campus));
            if (strlen(b->campus) == 0) {
                printf("  校区不能为空。\n"); 
                pause_screen(); 
                continue;
            }
            printf("  楼栋名称："); 
            fflush(stdout);
            get_line(b->building_name, sizeof(b->building_name));
            if (strlen(b->building_name) == 0) {
                printf("  楼栋名称不能为空。\n"); 
                pause_screen(); 
                continue;
            }
            b->building_id = ds->ids.next_building_id++;
            ds->building_count++;
            save_buildings(ds);
            printf("  已添加楼栋，ID=%d\n", b->building_id);
            pause_screen();

        }
        else if (choice == 3) {
            int id = get_int("  输入楼栋 ID：");
            Building* b = find_building(ds, id);
            if (!b) { 
                printf("  未找到该楼栋。\n"); 
                pause_screen(); 
                continue; 
            }
            char tmp[32];
            printf("  新校区名（留空不改）："); 
            fflush(stdout);
            get_line(tmp, sizeof(tmp));
            if (strlen(tmp) > 0) { 
                strncpy(b->campus, tmp, sizeof(b->campus) - 1); 
            }
            printf("  新楼栋名（留空不改）："); 
            fflush(stdout);
            get_line(tmp, sizeof(tmp));
            if (strlen(tmp) > 0) {
                strncpy(b->building_name, tmp, sizeof(b->building_name) - 1);
            }
            save_buildings(ds);
            printf("  修改成功。\n"); 
            pause_screen();

        }
        else if (choice == 4) {
            int id = get_int("  输入要删除的楼栋 ID：");
            int idx = -1;
            for (int i = 0; i < ds->building_count; i++)
                if (ds->buildings[i].building_id == id) { 
                    idx = i; 
                    break; 
                }
            if (idx < 0) { 
                printf("  未找到。\n"); 
                pause_screen(); 
                continue; 
            }
            /* 右移覆盖删除 */
            for (int i = idx; i < ds->building_count - 1; i++) ds->buildings[i] = ds->buildings[i + 1];
            ds->building_count--;
            save_buildings(ds);
            printf("  删除成功。\n"); 
            pause_screen();
        }
    } while (choice != 0);
}

//房间管理
void admin_manage_rooms(DataStore* ds) {
    int choice;
    do {
        print_header("房间管理");
        printf("  [1] 查看所有房间（报表）\n");
        printf("  [2] 新增房间\n");
        printf("  [3] 充值电费 / 修改预警阈值\n");
        printf("  [4] 修改房间容量\n");
        printf("  [5] 删除房间\n");
        printf("  [0] 返回\n");
        print_line('-', 40);
        choice = get_int("  请选择：");

        if (choice == 1) {
            print_header("房间报表");
            report_rooms(ds);
            pause_screen();

        }
        else if (choice == 2) {
            if (ds->room_count >= MAX_ROOMS) {
                printf("  房间记录已满。\n"); 
                pause_screen(); 
                continue;
            }
            Room* r = &ds->rooms[ds->room_count];
            memset(r, 0, sizeof(*r));
            r->room_id = ds->ids.next_room_id++;
            r->building_id = get_int("  所属楼栋 ID：");
            if (!find_building(ds, r->building_id)) {
                printf("  警告：楼栋 ID=%d 不存在，仍将创建。\n", r->building_id);
            }
            printf("  房号（如 208）："); 
            fflush(stdout);
            get_line(r->room_number, sizeof(r->room_number));
            r->capacity = get_int("  总床位数：");
            r->elec_alert_threshold = get_float("  电费预警阈值（元）：");
            r->elec_balance = get_float("  初始电费余额（元）：");
            r->current_count = 0;
            ds->room_count++;
            save_rooms(ds);
            printf("  已添加房间，ID=%d\n", r->room_id);
            pause_screen();

        }
        else if (choice == 3) {
            int id = get_int("  输入房间 ID：");
            Room* r = find_room(ds, id);
            if (!r) { 
                printf("  未找到。\n"); 
                pause_screen(); 
                continue; 
            }
            printf("  当前余额：%.2f 元  阈值：%.2f 元\n",r->elec_balance, r->elec_alert_threshold);
            printf("  [1] 充值余额  [2] 修改预警阈值  [0] 取消\n");
            int op = get_int("  选择：");
            if (op == 1) {
                float amt = get_float("  充值金额（元）：");
                if (amt <= 0) { 
                    printf("  金额无效。\n"); 
                    pause_screen(); 
                    continue; 
                }
                r->elec_balance += amt;
                printf("  充值后余额：%.2f 元\n", r->elec_balance);
            }
            else if (op == 2) {
                r->elec_alert_threshold = get_float("  新预警阈值（元）：");
                printf("  阈值已更新为 %.2f 元\n", r->elec_alert_threshold);
            }
            save_rooms(ds);
            pause_screen();

        }
        else if (choice == 4) {
            int id = get_int("  输入房间 ID：");
            Room* r = find_room(ds, id);
            if (!r) { 
                printf("  未找到。\n"); 
                pause_screen(); 
                continue; 
            }
            int cap = get_int("  新容量：");
            if (cap < r->current_count) {
                printf("  容量不能小于已住人数（%d）。\n", r->current_count);
                pause_screen(); 
                continue;
            }
            r->capacity = cap;
            save_rooms(ds);
            printf("  修改成功。\n"); 
            pause_screen();

        }
        else if (choice == 5) {
            int id = get_int("  输入要删除的房间 ID：");
            int idx = -1;
            for (int i = 0; i < ds->room_count; i++) {
                if (ds->rooms[i].room_id == id) { 
                    idx = i; 
                    break; 
                }
            }
            if (idx < 0) { 
                printf("  未找到。\n"); 
                pause_screen(); 
                continue; 
            }
            if (ds->rooms[idx].current_count > 0) {
                printf("  该房间仍有 %d 名学生，无法删除。\n",ds->rooms[idx].current_count);
                pause_screen(); 
                continue;
            }
            for (int i = idx; i < ds->room_count - 1; i++) {
                ds->rooms[i] = ds->rooms[i + 1];
            }
            ds->room_count--;
            save_rooms(ds);
            printf("  删除成功。\n"); 
            pause_screen();
        }
    } while (choice != 0);
}

//学生账号管理
void admin_manage_students(DataStore* ds) {
    int choice;
    do {
        print_header("学生账号管理");
        printf("  [1] 查看所有学生（报表）\n");
        printf("  [2] 新增学生\n");
        printf("  [3] 修改学生信息\n");
        printf("  [4] 重置学生密码\n");
        printf("  [5] 删除学生\n");
        printf("  [0] 返回\n");
        print_line('-', 40);
        choice = get_int("  请选择：");

        if (choice == 1) {
            print_header("学生列表报表");
            report_students(ds);
            pause_screen();

        }
        else if (choice == 2) {
            if (ds->student_count >= MAX_STUDENTS) {
                printf("  学生记录已满。\n"); 
                pause_screen(); 
                continue;
            }
            Student* s = &ds->students[ds->student_count];
            memset(s, 0, sizeof(*s));
            printf("  学号（最多11位）："); 
            fflush(stdout);
            get_line(s->student_id, sizeof(s->student_id));
            if (strlen(s->student_id) == 0) {
                printf("  学号不能为空。\n"); 
                pause_screen(); 
                continue;
            }
            if (find_student(ds, s->student_id)) {
                printf("  学号已存在。\n"); 
                pause_screen(); 
                continue;
            }
            printf("  姓名："); 
            fflush(stdout);
            get_line(s->name, sizeof(s->name));
            char plain[32];
            printf("  密码："); 
            fflush(stdout);
            get_line(plain, sizeof(plain));
            if (strlen(plain) == 0) {
                strncpy(plain, "123456", sizeof(plain) - 1);
            }
            encrypt_password(plain, s->password);
            s->room_id = -1;
            s->bed_number = 0;
            ds->student_count++;
            save_students(ds);
            printf("  学生 %s 已添加（密码已加密存储）。\n", s->student_id);
            pause_screen();

        }
        else if (choice == 3) {
            char sid[12]; 
            printf("  输入学号："); 
            fflush(stdout);
            get_line(sid, sizeof(sid));
            Student* s = find_student(ds, sid);
            if (!s) { 
                printf("  未找到。\n"); 
                pause_screen(); 
                continue; 
            }
            char tmp[32];
            printf("  新姓名（留空不改）："); 
            fflush(stdout);
            get_line(tmp, sizeof(tmp));
            if (strlen(tmp) > 0) {
                strncpy(s->name, tmp, sizeof(s->name) - 1);
            }
            save_students(ds);
            printf("  修改成功。\n"); 
            pause_screen();

        }
        else if (choice == 4) {
            char sid[12]; 
            printf("  输入学号："); 
            fflush(stdout);
            get_line(sid, sizeof(sid));
            Student* s = find_student(ds, sid);
            if (!s) { 
                printf("  未找到。\n"); 
                pause_screen(); 
                continue; 
            }
            char plain[32];
            printf("  新密码："); 
            fflush(stdout);
            get_line(plain, sizeof(plain));
            if (strlen(plain) == 0) strncpy(plain, "123456", sizeof(plain) - 1);
            encrypt_password(plain, s->password);
            save_students(ds);
            printf("  密码已重置。\n"); 
            pause_screen();

        }
        else if (choice == 5) {
            char sid[12]; 
            printf("  输入学号："); 
            fflush(stdout);
            get_line(sid, sizeof(sid));
            int idx = -1;
            for (int i = 0; i < ds->student_count; i++) {
                if (strcmp(ds->students[i].student_id, sid) == 0) { 
                    idx = i; 
                    break; 
                }
            }
            if (idx < 0) { 
                printf("  未找到。\n"); 
                pause_screen(); 
                continue; 
            }
            //退房
            if (ds->students[idx].room_id != -1) {
                Room* r = find_room(ds, ds->students[idx].room_id);
                if (r && r->current_count > 0) r->current_count--;
                save_rooms(ds);
            }
            for (int i = idx; i < ds->student_count - 1; i++) {
                ds->students[i] = ds->students[i + 1];
            }
            ds->student_count--;
            save_students(ds);
            printf("  删除成功。\n"); 
            pause_screen();
        }
    } while (choice != 0);
}

//宿管账号管理
void admin_manage_managers(DataStore* ds) {
    int choice;
    do {
        print_header("宿管账号管理");
        printf("  [1] 查看所有宿管（报表）\n");
        printf("  [2] 新增宿管\n");
        printf("  [3] 修改宿管信息\n");
        printf("  [4] 重置宿管密码\n");
        printf("  [5] 删除宿管\n");
        printf("  [0] 返回\n");
        print_line('-', 40);
        choice = get_int("  请选择：");

        if (choice == 1) {
            print_header("宿管列表报表");
            report_managers(ds);
            pause_screen();

        }
        else if (choice == 2) {
            if (ds->manager_count >= MAX_MANAGERS) {
                printf("  宿管记录已满。\n"); 
                pause_screen(); 
                continue;
            }
            DormManager* m = &ds->managers[ds->manager_count];
            memset(m, 0, sizeof(*m));
            printf("  工号："); 
            fflush(stdout); 
            get_line(m->manager_id, sizeof(m->manager_id));
            if (strlen(m->manager_id) == 0) { 
                printf("  工号不能为空。\n"); 
                pause_screen(); 
                continue; 
            }
            if (find_manager(ds, m->manager_id)) { 
                printf("  工号已存在。\n"); 
                pause_screen(); 
                continue; 
            }
            printf("  姓名："); 
            fflush(stdout); 
            get_line(m->name, sizeof(m->name));
            char plain[32];
            printf("  密码："); 
            fflush(stdout); 
            get_line(plain, sizeof(plain));
            if (strlen(plain) == 0) strncpy(plain, "123456", sizeof(plain) - 1);
            encrypt_password(plain, m->password);
            m->building_id = get_int("  负责楼栋 ID（无则输入 -1）：");
            ds->manager_count++;
            save_managers(ds);
            printf("  宿管 %s 已添加（密码已加密存储）。\n", m->manager_id);
            pause_screen();

        }
        else if (choice == 3) {
            char mid[12]; 
            printf("  输入工号："); 
            fflush(stdout); 
            get_line(mid, sizeof(mid));
            DormManager* m = find_manager(ds, mid);
            if (!m) { 
                printf("  未找到。\n"); 
                pause_screen(); 
                continue; 
            }
            char tmp[32];
            printf("  新姓名（留空不改）："); 
            fflush(stdout); 
            get_line(tmp, sizeof(tmp));
            if (strlen(tmp) > 0) strncpy(m->name, tmp, sizeof(m->name) - 1);
            printf("  新楼栋 ID（输入 -999 不改）：");
            int bid = get_int("");
            if (bid != -999) m->building_id = bid;
            save_managers(ds);
            printf("  修改成功。\n"); 
            pause_screen();

        }
        else if (choice == 4) {
            char mid[12]; 
            printf("  输入工号："); 
            fflush(stdout); 
            get_line(mid, sizeof(mid));
            DormManager* m = find_manager(ds, mid);
            if (!m) { 
                printf("  未找到。\n"); 
                pause_screen(); 
                continue; 
            }
            char plain[32];
            printf("  新密码："); 
            fflush(stdout); 
            get_line(plain, sizeof(plain));
            if (strlen(plain) == 0) strncpy(plain, "123456", sizeof(plain) - 1);
            encrypt_password(plain, m->password);
            save_managers(ds);
            printf("  密码已重置。\n"); pause_screen();

        }
        else if (choice == 5) {
            char mid[12]; 
            printf("  输入工号："); 
            fflush(stdout); 
            get_line(mid, sizeof(mid));
            int idx = -1;
            for (int i = 0; i < ds->manager_count; i++) {
                if (strcmp(ds->managers[i].manager_id, mid) == 0) { 
                    idx = i; 
                    break; 
                }
            }
            if (idx < 0) { 
                printf("  未找到。\n"); 
                pause_screen(); 
                continue; 
            }
            for (int i = idx; i < ds->manager_count - 1; i++) {
                ds->managers[i] = ds->managers[i + 1];
            }
            ds->manager_count--;
            save_managers(ds);
            printf("  删除成功。\n"); pause_screen();
        }
    } while (choice != 0);
}


//宿舍分配
void admin_assign_dorm(DataStore* ds) {
    print_header("宿舍分配");

    char sid[12];
    printf("  输入学号："); 
    fflush(stdout);
    get_line(sid, sizeof(sid));
    Student* s = find_student(ds, sid);
    if (!s) { 
        printf("  未找到该学生。\n"); 
        pause_screen(); 
        return; 
    }
    printf("  学生：%s，当前房间ID：%s\n",s->name, s->room_id == -1 ? "未分配" : "");
    if (s->room_id != -1) printf("  （当前房间ID=%d）\n", s->room_id);

    /* 显示有空余床位的房间 */
    printf("\n  可用房间列表：\n");
    printf("  %-6s  %-8s  %-6s  %-6s  %-6s  %s\n","ID", "房号", "楼栋ID", "容量", "已住", "楼栋名");
    print_line('-', 55);
    int avail = 0;
    for (int i = 0; i < ds->room_count; i++) {
        Room* r = &ds->rooms[i];
        if (r->current_count >= r->capacity) continue;
        Building* b = find_building(ds, r->building_id);
        printf("  %-6d  %-8s  %-6d  %-6d  %-6d  %s\n",r->room_id, r->room_number, r->building_id,r->capacity, r->current_count,b ? b->building_name : "—");
        avail++;
    }
    if (!avail) { 
        printf("  暂无可用房间。\n"); 
        pause_screen(); 
        return; 
    }

    int rid = get_int("\n  输入目标房间 ID（0=取消）：");
    if (rid == 0) return;
    Room* r = find_room(ds, rid);
    if (!r) { 
        printf("  未找到该房间。\n"); 
        pause_screen(); 
        return; 
    }
    if (r->current_count >= r->capacity) {
        printf("  该房间已住满（%d/%d）。\n", r->current_count, r->capacity);
        pause_screen(); 
        return;
    }

    char prompt[64];
    snprintf(prompt, sizeof(prompt), "  分配床号（1~%d）：", r->capacity);
    int bed = get_int(prompt);
    if (bed < 1 || bed > r->capacity) {
        printf("  床号超出范围。\n"); 
        pause_screen(); 
        return;
    }

    //检查床号是否已被占用
    for (int i = 0; i < ds->student_count; i++) {
        if (ds->students[i].room_id == rid &&ds->students[i].bed_number == bed &&strcmp(ds->students[i].student_id, sid) != 0) {
            printf("  该床号已被学生 %s 占用。\n", ds->students[i].name);
            pause_screen(); 
            return;
        }
    }

    //原房间退住
    if (s->room_id != -1) {
        Room* old_r = find_room(ds, s->room_id);
        if (old_r && old_r->current_count > 0) old_r->current_count--;
    }

    s->room_id = rid;
    s->bed_number = bed;
    r->current_count++;
    save_students(ds);
    save_rooms(ds);

    Building* b = find_building(ds, r->building_id);
    printf("\n  分配成功：%s → %s %s %d号床\n",s->name,b ? b->building_name : "(楼栋未知)",r->room_number, bed);
    pause_screen();
}

//全局统计
void admin_statistics(DataStore* ds) {
    print_header("全局统计概览");

    //入住率
    int total_cap = 0, occupied = 0;
    for (int i = 0; i < ds->room_count; i++) {
        total_cap += ds->rooms[i].capacity;
        occupied += ds->rooms[i].current_count;
    }
    printf("  [入住情况]\n");
    printf("    楼栋数：%-4d  房间数：%-4d\n", ds->building_count, ds->room_count);
    printf("    总床位：%-4d  已住：%-4d  入住率：%.1f%%\n",total_cap, occupied,total_cap > 0 ? (float)occupied / total_cap * 100.0f : 0.0f);
    printf("    在册学生：%-4d  宿管：%d\n\n", ds->student_count, ds->manager_count);

    //报修统计
    int rp[4] = { 0, 0, 0, 0 };
    for (int i = 0; i < ds->repair_count; i++) {
        int st = ds->repairs[i].status;
        if (st >= 0 && st <= 3) rp[st]++;
    }
    printf("  [报修情况]  共 %d 条\n", ds->repair_count);
    printf("    待处理：%d  受理中：%d  已完成：%d  驳回：%d\n",rp[0], rp[1], rp[2], rp[3]);
    if (ds->repair_count > 0) printf("    完成率：%.1f%%\n\n", (float)rp[2] / ds->repair_count * 100.0f);
    else printf("\n");

    //卫生评分
    if (ds->hygiene_count > 0) {
        float sum = 0;
        int hi = ds->hygiene[0].score, lo = ds->hygiene[0].score;
        for (int i = 0; i < ds->hygiene_count; i++) {
            sum += ds->hygiene[i].score;
            if (ds->hygiene[i].score > hi) hi = ds->hygiene[i].score;
            if (ds->hygiene[i].score < lo) lo = ds->hygiene[i].score;
        }
        printf("  [卫生检查]  共 %d 次\n", ds->hygiene_count);
        printf("    平均分：%.1f  最高：%d  最低：%d\n\n",sum / ds->hygiene_count, hi, lo);
    }
    else {
        printf("  [卫生检查]  暂无记录\n\n");
    }

    //违规 TOP3 类型
    printf("  [违规记录]  共 %d 条\n", ds->violation_count);
    if (ds->violation_count > 0) {
        /* 统计各类型频次 */
        char  types[256][32];
        int   counts[256];
        int   type_cnt = 0;
        for (int i = 0; i < ds->violation_count; i++) {
            int hit = 0;
            for (int j = 0; j < type_cnt; j++) {
                if (strcmp(types[j], ds->violations[i].type) == 0) {
                    counts[j]++; 
                    hit = 1; 
                    break;
                }
            }
            if (!hit && type_cnt < 256) {
                strncpy(types[type_cnt], ds->violations[i].type, 31);
                types[type_cnt][31] = '\0';
                counts[type_cnt++] = 1;
            }
        }
        //冒泡排序取 TOP3
        for (int i = 0; i < type_cnt - 1; i++)
            for (int j = i + 1; j < type_cnt; j++)
                if (counts[j] > counts[i]) {
                    int  tc = counts[i]; counts[i] = counts[j]; counts[j] = tc;
                    char tb[32];
                    strncpy(tb, types[i], 31); tb[31] = '\0';
                    strncpy(types[i], types[j], 31);
                    strncpy(types[j], tb, 31);
                }
        int show = type_cnt < 3 ? type_cnt : 3;
        for (int i = 0; i < show; i++) {
            printf("    %d  %-20s  %d 次\n", i + 1, types[i], counts[i]);
        }
    }
    printf("\n");

    //电费概况
    int rooms_owe = 0, rooms_warn = 0;
    float total_balance = 0.0f;
    for (int i = 0; i < ds->room_count; i++) {
        total_balance += ds->rooms[i].elec_balance;
        if (ds->rooms[i].elec_balance <= 0) {
            rooms_owe++;
        }
        else if (ds->rooms[i].elec_balance <= ds->rooms[i].elec_alert_threshold) {
            rooms_warn++;
        }
    }
    printf("  [电费概况]\n");
    printf("    欠费房间：%d  预警房间：%d  总余额：%.2f 元  抄表记录：%d 条\n",rooms_owe, rooms_warn, total_balance, ds->bill_count);

    print_line('-', 55);
    pause_screen();
}

//组合查询 — 学生（姓名+楼栋）
void admin_query_students(DataStore* ds) {
    print_header("学生组合查询（姓名+楼栋）");

    char kw_name[16], kw_bname[32];
    printf("  姓名关键字（留空=不限）："); 
    fflush(stdout);
    get_line(kw_name, sizeof(kw_name));
    printf("  楼栋名关键字（留空=不限）："); 
    fflush(stdout);
    get_line(kw_bname, sizeof(kw_bname));

    if (strlen(kw_name) == 0 && strlen(kw_bname) == 0) {
        printf("  至少输入一个条件。\n"); 
        pause_screen(); 
        return;
    }

    printf("\n  %-12s  %-10s  %-8s  %-4s  %s\n","学号", "姓名", "房间ID", "床号", "楼栋");
    print_line('-', 55);

    int found = 0;
    for (int i = 0; i < ds->student_count; i++) {
        Student* s = &ds->students[i];

        //条件1：姓名包含关键字
        if (strlen(kw_name) > 0 && strstr(s->name, kw_name) == NULL)
            continue;

        //件2：所在楼栋名包含关键字
        if (strlen(kw_bname) > 0) {
            int match = 0;
            if (s->room_id != -1) {
                Room* r = find_room(ds, s->room_id);
                if (r) {
                    Building* b = find_building(ds, r->building_id);
                    if (b && strstr(b->building_name, kw_bname) != NULL)
                        match = 1;
                }
            }
            if (!match) continue;
        }

        //获取楼栋名用于显示
        const char* bname = "—";
        if (s->room_id != -1) {
            Room* r = find_room(ds, s->room_id);
            if (r) {
                Building* b = find_building(ds, r->building_id);
                if (b) bname = b->building_name;
            }
        }
        printf("  %-12s  %-10s  %-8d  %-4d  %s\n",s->student_id, s->name, s->room_id, s->bed_number, bname);
        found++;
    }
    if (!found) printf("  未找到符合条件的记录。\n");
    else printf("\n  共 %d 条结果。\n", found);
    pause_screen();
}

   //组合查询 — 报修（学号 AND 状态）
void admin_query_repairs(DataStore* ds) {
    print_header("报修组合查询（学号+状态）");

    char kw_sid[12];
    printf("  学号（留空=不限）："); fflush(stdout);
    get_line(kw_sid, sizeof(kw_sid));

    printf("  状态（-1=不限 / 0待处理 1受理中 2已完成 3驳回）：");
    int status_filter = get_int("");

    if (strlen(kw_sid) == 0 && status_filter == -1) {
        printf("  至少输入一个条件。\n"); 
        pause_screen(); 
        return;
    }

    printf("\n  %-5s  %-12s  %-8s  %-20s  %s\n",
        "单号", "学号", "状态", "提交时间", "描述（前25字）");
    print_line('-', 72);

    int found = 0;
    for (int i = 0; i < ds->repair_count; i++) {
        RepairRequest* rr = &ds->repairs[i];

        //条件1：学号匹配
        if (strlen(kw_sid) > 0 && strcmp(rr->student_id, kw_sid) != 0)
            continue;

        //条件2：状态匹配
        if (status_filter != -1 && rr->status != status_filter)
            continue;

        printf("  %-5d  %-12s  %-8s  %-20s  %.25s\n",
            rr->repair_id, rr->student_id,
            repair_status_str(rr->status),
            rr->submit_time, rr->description);
        found++;
    }
    if (!found) printf("  未找到符合条件的记录。\n");
    else printf("\n  共 %d 条结果。\n", found);
    pause_screen();
}

//组合查询 — 卫生（房间ID+评分区间）
void admin_query_hygiene(DataStore* ds) {
    print_header("卫生组合查询（房间ID+评分区间）");

    printf("  房间 ID（0=不限）：");
    int rid_filter = get_int("");

    printf("  最低评分（0=不限）：");
    int score_lo = get_int("");
    printf("  最高评分（100=不限）：");
    int score_hi = get_int("");
    if (score_hi <= 0) score_hi = 100;

    if (rid_filter == 0 && score_lo == 0 && score_hi == 100) {
        printf("  至少输入一个约束条件。\n"); 
        pause_screen(); 
        return;
    }

    printf("\n  %-6s  %-8s  %-6s  %-20s  %s\n",
        "记录ID", "房间ID", "评分", "检查时间", "备注");
    print_line('-', 60);

    int found = 0;
    for (int i = 0; i < ds->hygiene_count; i++) {
        HygieneRecord* h = &ds->hygiene[i];

        //条件1：房间 ID
        if (rid_filter != 0 && h->room_id != rid_filter)
            continue;

        //条件2：评分区间
        if (h->score < score_lo || h->score > score_hi)
            continue;

        printf("  %-6d  %-8d  %-6d  %-20s  %s\n",
            h->hygiene_id, h->room_id, h->score,
            h->check_time, h->remark);
        found++;
    }
    if (!found) printf("  未找到符合条件的记录。\n");
    else printf("\n  共 %d 条结果。\n", found);
    pause_screen();
}


   //组合查询 — 违规（学号+违规类型关键字）
void admin_query_violations(DataStore* ds) {
    print_header("违规组合查询（学号+类型关键字）");

    char kw_sid[12], kw_type[32];
    printf("  学号（留空=不限）："); 
    fflush(stdout);
    get_line(kw_sid, sizeof(kw_sid));
    printf("  违规类型关键字（留空=不限）："); 
    fflush(stdout);
    get_line(kw_type, sizeof(kw_type));

    if (strlen(kw_sid) == 0 && strlen(kw_type) == 0) {
        printf("  至少输入一个条件。\n"); 
        pause_screen(); 
        return;
    }

    printf("\n  %-6s  %-12s  %-16s  %-20s  %s\n",
        "编号", "学号", "类型", "时间", "描述（前25字）");
    print_line('-', 75);

    int found = 0;
    for (int i = 0; i < ds->violation_count; i++) {
        ViolationRecord* v = &ds->violations[i];

        /* 条件1：学号 */
        if (strlen(kw_sid) > 0 && strcmp(v->student_id, kw_sid) != 0)
            continue;

        /* 条件2：类型关键字 */
        if (strlen(kw_type) > 0 && strstr(v->type, kw_type) == NULL)
            continue;

        printf("  %-6d  %-12s  %-16s  %-20s  %.25s\n",
            v->violation_id, v->student_id,
            v->type, v->record_time, v->description);
        found++;
    }
    if (!found) printf("  未找到符合条件的记录。\n");
    else printf("\n  共 %d 条结果。\n", found);
    pause_screen();
}


//排序报表（升序 / 降序切换）
void admin_sort_and_report(DataStore* ds) {
    int choice;
    do {
        print_header("排序报表");
        printf("  [1] 学生列表（按学号 / 姓名排序）\n");
        printf("  [2] 房间列表（按电费余额排序）\n");
        printf("  [3] 卫生记录（按评分排序）\n");
        printf("  [4] 报修记录（按提交时间排序）\n");
        printf("  [5] 电费账单（按单号排序）\n");
        printf("  [0] 返回\n");
        print_line('-', 40);
        choice = get_int("  请选择：");
        if (choice == 0) break;
        if (choice < 1 || choice > 5) { printf("  无效选项。\n"); continue; }

        printf("  排序方向：[1] 升序  [2] 降序\n");
        int dir_sel = get_int("  请选择：");
        int dir = (dir_sel == 2) ? -1 : 1;
        const char* dir_str = (dir == 1) ? "升序" : "降序";

        if (choice == 1) {
            /* 临时拷贝，不破坏原数组 */
            Student* tmp = (Student*)malloc((size_t)ds->student_count * sizeof(Student));
            if (!tmp) { printf("  内存不足。\n"); pause_screen(); continue; }
            int n = ds->student_count;
            memcpy(tmp, ds->students, (size_t)n * sizeof(Student));

            printf("  子选项：[1] 按学号  [2] 按姓名\n");
            int sub = get_int("  请选择：");
            if (sub == 2) {
                sort_students_by_name(tmp, n, dir);
                print_header("学生列表（按姓名排序）");
            }
            else {
                sort_students_by_id(tmp, n, dir);
                print_header("学生列表（按学号排序）");
            }
            printf("  排序方向：%s\n\n", dir_str);
            printf("  %-12s  %-10s  %-8s  %s\n", "学号", "姓名", "房间ID", "床号");
            print_line('-', 45);
            for (int i = 0; i < n; i++) {
                Student* s = &tmp[i];
                if (s->room_id == -1)
                    printf("  %-12s  %-10s  %-8s  —\n",
                        s->student_id, s->name, "未分配");
                else
                    printf("  %-12s  %-10s  %-8d  %d\n",
                        s->student_id, s->name, s->room_id, s->bed_number);
            }
            if (n == 0) printf("  （暂无数据）\n");
            print_line('-', 45);
            printf("  共 %d 条  [%s]\n", n, dir_str);
            free(tmp);

        }
        else if (choice == 2) {
            Room* tmp = (Room*)malloc((size_t)ds->room_count * sizeof(Room));
            if (!tmp) { printf("  内存不足。\n"); pause_screen(); continue; }
            int n = ds->room_count;
            memcpy(tmp, ds->rooms, (size_t)n * sizeof(Room));
            sort_rooms_by_balance(tmp, n, dir);
            print_header("房间列表（按电费余额排序）");
            printf("  排序方向：%s\n\n", dir_str);
            printf("  %-6s  %-8s  %-10s  %-10s  %s\n",
                "房间ID", "房号", "余额(元)", "阈值(元)", "状态");
            print_line('-', 55);
            for (int i = 0; i < n; i++) {
                Room* r = &tmp[i];
                const char* flag = "";
                if (r->elec_balance <= 0) flag = "[欠费]";
                else if (r->elec_balance <= r->elec_alert_threshold) flag = "[预警]";
                printf("  %-6d  %-8s  %-10.2f  %-10.2f  %s\n",
                    r->room_id, r->room_number,
                    r->elec_balance, r->elec_alert_threshold, flag);
            }
            if (n == 0) printf("  （暂无数据）\n");
            print_line('-', 55);
            printf("  共 %d 条  [%s]\n", n, dir_str);
            free(tmp);

        }
        else if (choice == 3) {
            HygieneRecord* tmp = (HygieneRecord*)malloc((size_t)ds->hygiene_count * sizeof(HygieneRecord));
            if (!tmp) { 
                printf("  内存不足。\n"); 
                pause_screen(); 
                continue; 
            }
            int n = ds->hygiene_count;
            memcpy(tmp, ds->hygiene, (size_t)n * sizeof(HygieneRecord));
            sort_hygiene_by_score(tmp, n, dir);
            print_header("卫生记录（按评分排序）");
            printf("  排序方向：%s\n\n", dir_str);
            printf("  %-6s  %-8s  %-6s  %-20s  %s\n",
                "记录ID", "房间ID", "评分", "检查时间", "备注");
            print_line('-', 60);
            for (int i = 0; i < n; i++) {
                HygieneRecord* h = &tmp[i];
                printf("  %-6d  %-8d  %-6d  %-20s  %s\n",
                    h->hygiene_id, h->room_id, h->score,
                    h->check_time, h->remark);
            }
            if (n == 0) printf("  （暂无数据）\n");
            print_line('-', 60);
            printf("  共 %d 条  [%s]\n", n, dir_str);
            free(tmp);

        }
        else if (choice == 4) {
            RepairRequest* tmp = (RepairRequest*)malloc((size_t)ds->repair_count * sizeof(RepairRequest));
            if (!tmp) { 
                printf("  内存不足。\n"); 
                pause_screen(); 
                continue; 
            }
            int n = ds->repair_count;
            memcpy(tmp, ds->repairs, (size_t)n * sizeof(RepairRequest));
            sort_repairs_by_time(tmp, n, dir);
            print_header("报修记录（按提交时间排序）");
            printf("  排序方向：%s\n\n", dir_str);
            printf("  %-5s  %-12s  %-8s  %-20s  %s\n",
                "单号", "学号", "状态", "提交时间", "描述（前20字）");
            print_line('-', 70);
            for (int i = 0; i < n; i++) {
                RepairRequest* rr = &tmp[i];
                printf("  %-5d  %-12s  %-8s  %-20s  %.20s\n",
                    rr->repair_id, rr->student_id,
                    repair_status_str(rr->status),
                    rr->submit_time, rr->description);
            }
            if (n == 0) printf("  （暂无数据）\n");
            print_line('-', 70);
            printf("  共 %d 条  [%s]\n", n, dir_str);
            free(tmp);

        }
        else { /* choice == 5 */
            ElecBill* tmp = (ElecBill*)malloc((size_t)ds->bill_count * sizeof(ElecBill));
            if (!tmp) { printf("  内存不足。\n"); pause_screen(); continue; }
            int n = ds->bill_count;
            memcpy(tmp, ds->bills, (size_t)n * sizeof(ElecBill));
            sort_bills_by_id(tmp, n, dir);
            print_header("电费账单（按单号排序）");
            printf("  排序方向：%s\n\n", dir_str);
            printf("  %-6s  %-8s  %-10s  %-10s  %-10s  %s\n",
                "单号", "房间ID", "上期(度)", "本期(度)", "用电量", "抄表时间");
            print_line('-', 65);
            for (int i = 0; i < n; i++) {
                ElecBill* b = &tmp[i];
                printf("  %-6d  %-8d  %-10.1f  %-10.1f  %-10.1f  %s\n",
                    b->bill_id, b->room_id,
                    b->last_reading, b->curr_reading,
                    b->curr_reading - b->last_reading,
                    b->record_time);
            }
            if (n == 0) printf("  （暂无数据）\n");
            print_line('-', 65);
            printf("  共 %d 条  [%s]\n", n, dir_str);
            free(tmp);
        }
        pause_screen();
    } while (choice != 0);
}



//管理员账号管理
void admin_manage_admins(DataStore* ds, Admin* current) {
    int choice;
    do {
        print_header("管理员账号管理");
        printf("  [1] 查看所有管理员\n");
        printf("  [2] 新增管理员\n");
        printf("  [3] 修改当前账号密码\n");
        printf("  [4] 删除管理员\n");
        printf("  [0] 返回\n");
        print_line('-', 40);
        choice = get_int("  请选择：");

        if (choice == 1) {
            print_header("管理员列表");
            printf("  %-12s  %s\n", "账号", "姓名");
            print_line('-', 35);
            for (int i = 0; i < ds->admin_count; i++)
                printf("  %-12s  %s\n",
                    ds->admins[i].username, ds->admins[i].name);
            if (ds->admin_count == 0) printf("  （暂无数据）\n");
            pause_screen();

        }
        else if (choice == 2) {
            if (ds->admin_count >= MAX_ADMINS) {
                printf("  管理员已满。\n"); pause_screen(); continue;
            }
            Admin* a = &ds->admins[ds->admin_count];
            memset(a, 0, sizeof(*a));
            printf("  账号："); 
            fflush(stdout);
            get_line(a->username, sizeof(a->username));
            if (strlen(a->username) == 0) { 
                printf("  账号不能为空。\n"); 
                pause_screen(); 
                continue; 
            }
            /* 检查重名 */
            int dup = 0;
            for (int i = 0; i < ds->admin_count; i++)
                if (strcmp(ds->admins[i].username, a->username) == 0) { dup = 1; break; }
            if (dup) { 
                printf("  账号已存在。\n"); 
                pause_screen(); 
                continue; 
            }
            printf("  姓名："); 
            fflush(stdout); 
            get_line(a->name, sizeof(a->name));
            char plain[32];
            printf("  密码："); 
            fflush(stdout); 
            get_line(plain, sizeof(plain));
            if (strlen(plain) == 0) strncpy(plain, "admin123", sizeof(plain) - 1);
            encrypt_password(plain, a->password);
            ds->admin_count++;
            save_admins(ds);
            printf("  管理员 %s 已添加。\n", a->username);
            pause_screen();

        }
        else if (choice == 3) {
            char old_p[32], new_p[32], cfm[32];
            printf("  当前密码："); 
            fflush(stdout); 
            get_line(old_p, sizeof(old_p));
            if (!check_password(old_p, current->password)) {
                printf("  原密码错误。\n"); 
                pause_screen(); 
                continue;
            }
            printf("  新密码（至少6位）："); 
            fflush(stdout); get_line(new_p, sizeof(new_p));
            if ((int)strlen(new_p) < 6) { 
                printf("  密码太短。\n"); 
                pause_screen(); 
                continue; 
            }
            printf("  确认新密码："); 
            fflush(stdout); 
            get_line(cfm, sizeof(cfm));
            if (strcmp(new_p, cfm) != 0) { 
                printf("  两次输入不一致。\n"); 
                pause_screen(); 
                continue; }
            encrypt_password(new_p, current->password);
            save_admins(ds);
            printf("  密码已修改。\n"); 
            pause_screen();

        }
        else if (choice == 4) {
            if (ds->admin_count <= 1) {
                printf("  至少保留一个管理员账号。\n"); 
                pause_screen(); 
                continue;
            }
            char uname[20]; 
            printf("  输入要删除的账号："); 
            fflush(stdout);
            get_line(uname, sizeof(uname));
            if (strcmp(uname, current->username) == 0) {
                printf("  不能删除当前登录账号。\n"); 
                pause_screen(); 
                continue;
            }
            int idx = -1;
            for (int i = 0; i < ds->admin_count; i++)
                if (strcmp(ds->admins[i].username, uname) == 0) { 
                    idx = i; 
                    break; 
                }
            if (idx < 0) { 
                printf("  未找到。\n"); 
                pause_screen(); 
                continue; 
            }
            for (int i = idx; i < ds->admin_count - 1; i++)
                ds->admins[i] = ds->admins[i + 1];
            ds->admin_count--;
            save_admins(ds);
            printf("  删除成功。\n"); 
            pause_screen();
        }
    } while (choice != 0);
}

//管理员主菜单
void admin_menu(DataStore* ds, Admin* a) {
    int choice;
    do {
        print_header("管理员端 — 功能菜单");
        printf("  欢迎，%s（账号：%s）\n\n\n", a->name, a->username);
        printf("  ── 基础管理 ──────────────────────\n");
        printf("  [1]  学生账号管理\n");
        printf("  [2]  宿管账号管理\n");
        printf("  [3]  楼栋管理\n");
        printf("  [4]  房间管理\n");
        printf("  [5]  宿舍分配\n");
        printf("  [6]  全局统计\n\n");
        printf("  ── 查询与报表 ────────────────────\n");
        printf("  [7]  组合查询 — 学生（姓名+楼栋）\n");
        printf("  [8]  组合查询 — 报修（学号+状态）\n");
        printf("  [9]  组合查询 — 卫生（房间+评分）\n");
        printf("  [10] 组合查询 — 违规（学号+类型）\n");
        printf("  [11] 排序报表（升序/降序切换）\n\n");
        printf("  ── 系统管理 ──────────────────────\n");
        printf("  [12] 管理员账号管理\n");
        printf("  [0]  退出登录\n");
        print_line('-', 45);
        choice = get_int("  请选择：");
        switch (choice) {
        case  1: admin_manage_students(ds);         break;
        case  2: admin_manage_managers(ds);         break;
        case  3: admin_manage_buildings(ds);        break;
        case  4: admin_manage_rooms(ds);            break;
        case  5: admin_assign_dorm(ds);             break;
        case  6: admin_statistics(ds);              break;
        case  7: admin_query_students(ds);          break;
        case  8: admin_query_repairs(ds);           break;
        case  9: admin_query_hygiene(ds);           break;
        case 10: admin_query_violations(ds);        break;
        case 11: admin_sort_and_report(ds);         break;
        case 12: admin_manage_admins(ds, a);        break;
        case  0: break;
        default: printf("  无效选项。\n");
        }
    } while (choice != 0);
}
