#define _CRT_SECURE_NO_WARNINGS
//student.c — 学生端所有功能实现
#include "student.h"
#include "file.h"
#include "utils.h"

 //1. 个人宿舍信息
void student_view_dorm(DataStore* ds, Student* s) {
    print_header("个人宿舍信息");

    if (s->room_id == -1) {
        printf("  您尚未分配宿舍，请联系管理员。\n");
        pause_screen();
        return;
    }

    Room* r = find_room(ds, s->room_id);
    if (!r) { 
        printf("  数据异常，请联系管理员。\n"); 
        pause_screen(); 
        return; 
    }

    Building* b = find_building(ds, r->building_id);

    printf("  校  区：%s\n", b ? b->campus : "—");
    printf("  楼  栋：%s\n", b ? b->building_name : "—");
    printf("  房  号：%s\n", r->room_number);
    printf("  容  量：%d 人寝\n", r->capacity);
    printf("  当前人数：%d 人\n", r->current_count);

    //电费实时提醒
    if (r->elec_balance <= 0) {
        printf("\n  [!] 宿舍电费已欠费（%.2f 元），请尽快充值！\n", r->elec_balance);
    }
    else if (r->elec_balance <= r->elec_alert_threshold) {
        printf("\n  [!] 电费余额不足（%.2f 元 <= 阈值 %.2f 元），请及时充值！\n",r->elec_balance, r->elec_alert_threshold);
    }

    //舍友表格
    printf("\n  ┌──────────────────────────────────┐\n");
    printf("  │  床号   学号           姓名      │\n");
    printf("  ├─────────────────────────────────-┤\n");
    int found = 0;
    for (int i = 0; i < ds->student_count; i++) {
        Student* t = &ds->students[i];
        if (t->room_id != s->room_id) continue;
        char mark = (strcmp(t->student_id, s->student_id) == 0) ? '*' : ' ';
        printf("  │  %d号床%c  %-14s  %-8s  │\n",
            t->bed_number, mark, t->student_id, t->name);
        found++;
    }
    if (!found) printf("  │  （暂无舍友）                       │\n");
    printf("  └──────────────────────────────────┘\n");
    printf("  (*本人)\n");
    pause_screen();
}

//2. 提交报修申请
void student_submit_repair(DataStore* ds, Student* s) {
    print_header("提交报修申请");

    if (ds->repair_count >= MAX_REPAIRS) {
        printf("  报修记录已满，请联系管理员。\n");
        pause_screen();
        return;
    }

    char desc[128];
    printf("  请描述故障情况（最多120字）：\n  > "); fflush(stdout);
    get_line(desc, sizeof(desc));
    if (strlen(desc) == 0) {
        printf("  描述不能为空。\n"); 
        pause_screen(); 
        return;
    }

    RepairRequest* rr = &ds->repairs[ds->repair_count];
    memset(rr, 0, sizeof(*rr));
    rr->repair_id = ds->ids.next_repair_id++;
    strncpy(rr->student_id, s->student_id, sizeof(rr->student_id) - 1);
    strncpy(rr->description, desc, sizeof(rr->description) - 1);
    rr->status = REPAIR_PENDING;
    get_current_time(rr->submit_time, sizeof(rr->submit_time));
    strncpy(rr->update_time, rr->submit_time, sizeof(rr->update_time) - 1);
    ds->repair_count++;
    save_repairs(ds);

    printf("\n  提交成功，报修单号：%d\n", rr->repair_id);
    pause_screen();
}

//3. 查看我的报修记录
void student_view_repairs(DataStore* ds, Student* s) {
    print_header("我的报修记录");

    int found = 0;
    for (int i = 0; i < ds->repair_count; i++) {
        RepairRequest* rr = &ds->repairs[i];
        if (strcmp(rr->student_id, s->student_id) != 0) continue;
        if (!found) {
            printf("  %-6s  %-20s  %-8s  %s\n","单号", "提交时间", "状态", "描述（前30字）");
            print_line('-', 65);
        }
        printf("  %-6d  %-20s  %-8s  %.30s\n",rr->repair_id, rr->submit_time,repair_status_str(rr->status), rr->description);
        if (rr->status == REPAIR_DONE || rr->status == REPAIR_REJECTED) {
            printf("         更新时间：%s\n", rr->update_time);
        }
        found++;
    }
    if (!found) printf("  暂无报修记录。\n");
    pause_screen();
}

//4. 电费账单
void student_view_elec(DataStore* ds, Student* s) {
    print_header("电费账单");

    if (s->room_id == -1) { 
        printf("  未分配宿舍。\n"); 
        pause_screen(); 
        return; 
    }

    Room* r = find_room(ds, s->room_id);
    if (!r) { 
        printf("  数据异常。\n"); 
        pause_screen(); 
        return; 
    }

    printf("  当前余额：%.2f 元", r->elec_balance);
    if (r->elec_balance <= 0) {
        printf("  【欠费！】");
    }
    else if (r->elec_balance <= r->elec_alert_threshold) {
        printf("  【余额不足】");
    }
    printf("\n  预警阈值：%.2f 元\n", r->elec_alert_threshold);

    printf("\n  %-6s  %-20s  %-10s  %-10s  %s\n","单号", "抄表时间", "上期(度)", "本期(度)", "用电量");
    print_line('-', 65);

    int found = 0;
    for (int i = 0; i < ds->bill_count; i++) {
        ElecBill* b = &ds->bills[i];
        if (b->room_id != s->room_id) continue;
        printf("  %-6d  %-20s  %-10.1f  %-10.1f  %.1f 度\n",b->bill_id, b->record_time,b->last_reading, b->curr_reading,b->curr_reading - b->last_reading);
        found++;
    }
    if (!found) printf("  暂无抄表记录。\n");
    pause_screen();
}

//5. 提交访客预约
void student_submit_visitor(DataStore* ds, Student* s) {
    print_header("提交访客预约");

    if (ds->visitor_count >= MAX_VISITORS) {
        printf("  访客记录已满。\n");
        pause_screen(); 
        return;
    }

    VisitorRecord* v = &ds->visitors[ds->visitor_count];
    memset(v, 0, sizeof(*v));

    printf("  访客姓名："); 
    fflush(stdout);
    get_line(v->visitor_name, sizeof(v->visitor_name));
    if (strlen(v->visitor_name) == 0) {
        printf("  姓名不能为空。\n"); 
        pause_screen(); 
        return;
    }

    printf("  预计来访时间（如 2026-09-01 14:00）："); 
    fflush(stdout);
    get_line(v->visit_time, sizeof(v->visit_time));
    if (strlen(v->visit_time) == 0) {
        get_current_time(v->visit_time, sizeof(v->visit_time));
    }

    v->visitor_id = ds->ids.next_visitor_id++;
    strncpy(v->student_id, s->student_id, sizeof(v->student_id) - 1);
    strncpy(v->leave_time, "—", sizeof(v->leave_time) - 1);
    ds->visitor_count++;
    save_visitors(ds);

    printf("\n  预约提交成功，访客单号：%d\n", v->visitor_id);
    pause_screen();
}

//6. 查看访客记录 
void student_view_visitors(DataStore* ds, Student* s) {
    print_header("我的访客记录");

    int found = 0;
    for (int i = 0; i < ds->visitor_count; i++) {
        VisitorRecord* v = &ds->visitors[i];
        if (strcmp(v->student_id, s->student_id) != 0) continue;
        if (!found) {
            printf("  %-6s  %-12s  %-20s  %s\n","单号", "访客姓名", "来访时间", "离开时间");
            print_line('-', 60);
        }
        printf("  %-6d  %-12s  %-20s  %s\n",v->visitor_id, v->visitor_name, v->visit_time, v->leave_time);
        found++;
    }
    if (!found) printf("  暂无访客记录。\n");
    pause_screen();
}

// 7. 卫生检查记录 
void student_view_hygiene(DataStore* ds, Student* s) {
    print_header("卫生检查记录");

    if (s->room_id == -1) { 
        printf("  未分配宿舍。\n"); 
        pause_screen(); 
        return; 
    }

    int found = 0;
    for (int i = 0; i < ds->hygiene_count; i++) {
        HygieneRecord* h = &ds->hygiene[i];
        if (h->room_id != s->room_id) continue;
        if (!found) {
            printf("  %-20s  %-6s  %s\n", "检查时间", "评分", "备注");
            print_line('-', 55);
        }
        printf("  %-20s  %-6d  %s\n", h->check_time, h->score, h->remark);
        found++;
    }
    if (!found) printf("  暂无卫生检查记录。\n");
    pause_screen();
}

//8. 违规记录
void student_view_violations(DataStore* ds, Student* s) {
    print_header("我的违规记录");

    int found = 0;
    for (int i = 0; i < ds->violation_count; i++) {
        ViolationRecord* v = &ds->violations[i];
        if (strcmp(v->student_id, s->student_id) != 0) continue;
        if (!found) {
            printf("  %-20s  %-16s  %s\n", "时间", "类型", "描述（前30字）");
            print_line('-', 65);
        }
        printf("  %-20s  %-16s  %.30s\n",v->record_time, v->type, v->description);
        found++;
    }
    if (!found) printf("  暂无违规记录。\n");
    pause_screen();
}

//9. 修改密码
void student_change_password(DataStore* ds, Student* s) {
    print_header("修改密码");

    char old_pwd[32], new_pwd[32], confirm[32];

    printf("  当前密码："); fflush(stdout);
    get_line(old_pwd, sizeof(old_pwd));
    if (!check_password(old_pwd, s->password)) {
        printf("  原密码错误。\n"); 
        pause_screen(); 
        return;
    }

    printf("  新密码（至少6位）："); fflush(stdout);
    get_line(new_pwd, sizeof(new_pwd));
    if (strlen(new_pwd) < 6) {
        printf("  密码至少6位。\n"); 
        pause_screen(); 
        return;
    }

    printf("  确认新密码："); 
    fflush(stdout);
    get_line(confirm, sizeof(confirm));
    if (strcmp(new_pwd, confirm) != 0) {
        printf("  两次输入不一致。\n"); 
        pause_screen(); 
        return;
    }

    encrypt_password(new_pwd, s->password);
    save_students(ds);
    printf("  密码修改成功。\n");
    pause_screen();
}

//学生主菜单 
void student_menu(DataStore* ds, Student* s) {
    //登录后先检查电费
    if (s->room_id != -1) {
        Room* r = find_room(ds, s->room_id);
        if (r && r->elec_balance <= 0) {
            cls();
            printf("\n  ╔══════════════════════════════════════╗\n");
            printf("  ║  [!] 您的宿舍电费已欠费！            ║\n");
            printf("  ║      请尽快充值，以免影响正常使用。  ║\n");
            printf("  ╚══════════════════════════════════════╝\n");
            pause_screen();
        }
        else if (r && r->elec_balance <= r->elec_alert_threshold) {
            cls();
            printf("\n  [!] 电费余额不足（%.2f 元），请及时充值！\n",
                r->elec_balance);
            pause_screen();
        }
    }

    int choice;
    do {
        print_header("学生端 — 功能菜单");
        printf("  欢迎，%s（学号：%s）\n\n", s->name, s->student_id);
        printf("  [1]  个人宿舍信息\n");
        printf("  [2]  提交报修申请\n");
        printf("  [3]  我的报修记录\n");
        printf("  [4]  电费账单\n");
        printf("  [5]  提交访客预约\n");
        printf("  [6]  我的访客记录\n");
        printf("  [7]  卫生检查记录\n");
        printf("  [8]  违规记录\n");
        printf("  [9]  修改密码\n");
        printf("  [0]  退出登录\n");
        print_line('-', 60);
        choice = get_int("  请选择：");
        switch (choice) {
        case 1: student_view_dorm(ds, s);       break;
        case 2: student_submit_repair(ds, s);   break;
        case 3: student_view_repairs(ds, s);    break;
        case 4: student_view_elec(ds, s);       break;
        case 5: student_submit_visitor(ds, s);  break;
        case 6: student_view_visitors(ds, s);   break;
        case 7: student_view_hygiene(ds, s);    break;
        case 8: student_view_violations(ds, s); break;
        case 9: student_change_password(ds, s); break;
        case 0: break;
        default: printf("  无效选项。\n");
        }
    } while (choice != 0);
}
