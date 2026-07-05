#define _CRT_SECURE_NO_WARNINGS
//manager.c — 宿管端所有功能实现
#include "manager.h"
#include "file.h"
#include "utils.h"

//1. 报修申请处理
void manager_process_repairs(DataStore* ds, DormManager* m) {
    (void)m; /* 宿管可处理所有报修 */
    print_header("报修申请处理");

    if (ds->repair_count == 0) {
        printf("  暂无报修记录。\n"); 
        pause_screen(); 
        return;
    }

    printf("  %-5s  %-12s  %-8s  %-20s  %s\n","单号", "学号", "状态", "提交时间", "描述（前25字）");
    print_line('-', 75);
    for (int i = 0; i < ds->repair_count; i++) {
        RepairRequest* rr = &ds->repairs[i];
        printf("  %-5d  %-12s  %-8s  %-20s  %.25s\n",rr->repair_id, rr->student_id,repair_status_str(rr->status),rr->submit_time, rr->description);
    }

    int id = get_int("\n  输入要操作的报修单号（0=取消）：");
    if (id == 0) return;

    RepairRequest* target = NULL;
    for (int i = 0; i < ds->repair_count; i++) {
        if (ds->repairs[i].repair_id == id) {
            target = &ds->repairs[i];
            break;
        }
    }
    if (!target) { 
        printf("  未找到该报修单。\n"); 
        pause_screen(); 
        return; 
    }

    printf("\n  当前状态：%s\n", repair_status_str(target->status));
    printf("  [1] 受理中\n  [2] 已完成\n  [3] 驳回\n  [0] 取消\n");
    int op = get_int("  选择新状态：");
    if (op < 1 || op > 3) return;

    target->status = op;
    get_current_time(target->update_time, sizeof(target->update_time));
    save_repairs(ds);
    printf("  状态已更新为：%s\n", repair_status_str(target->status));
    pause_screen();
}

//2. 访客登记录入
void manager_add_visitor(DataStore* ds, DormManager* m) {
    (void)m;
    print_header("访客登记录入");

    if (ds->visitor_count >= MAX_VISITORS) {
        printf("  访客记录已满。\n"); pause_screen(); return;
    }

    VisitorRecord* v = &ds->visitors[ds->visitor_count];
    memset(v, 0, sizeof(*v));

    printf("  被访学生学号："); fflush(stdout);
    get_line(v->student_id, sizeof(v->student_id));
    if (!find_student(ds, v->student_id)) {
        printf("  未找到该学生。\n"); pause_screen(); return;
    }

    printf("  访客姓名："); fflush(stdout);
    get_line(v->visitor_name, sizeof(v->visitor_name));
    if (strlen(v->visitor_name) == 0) {
        printf("  姓名不能为空。\n"); pause_screen(); return;
    }

    printf("  来访时间（留空=当前时间）："); fflush(stdout);
    get_line(v->visit_time, sizeof(v->visit_time));
    if (strlen(v->visit_time) == 0)
        get_current_time(v->visit_time, sizeof(v->visit_time));

    printf("  离开时间（未离开填 —）："); fflush(stdout);
    get_line(v->leave_time, sizeof(v->leave_time));
    if (strlen(v->leave_time) == 0)
        strncpy(v->leave_time, "—", sizeof(v->leave_time) - 1);

    v->visitor_id = ds->ids.next_visitor_id++;
    ds->visitor_count++;
    save_visitors(ds);
    printf("\n  访客记录已录入，编号：%d\n", v->visitor_id);
    pause_screen();
}

//3. 卫生检查录入
void manager_add_hygiene(DataStore* ds, DormManager* m) {
    print_header("卫生检查录入");

    if (ds->hygiene_count >= MAX_HYGIENE) {
        printf("  卫生记录已满。\n"); pause_screen(); return;
    }

    /* 显示本楼栋房间 */
    printf("  本楼栋（ID=%d）房间列表：\n", m->building_id);
    int found = 0;
    for (int i = 0; i < ds->room_count; i++) {
        Room* r = &ds->rooms[i];
        if (r->building_id != m->building_id) continue;
        printf("    房间 ID %-4d  房号 %-8s  当前人数 %d\n",
            r->room_id, r->room_number, r->current_count);
        found++;
    }
    if (!found) { printf("  本楼栋暂无房间。\n"); pause_screen(); return; }

    int rid = get_int("\n  输入房间 ID：");
    Room* r = find_room(ds, rid);
    if (!r || r->building_id != m->building_id) {
        printf("  无效房间 ID。\n"); pause_screen(); return;
    }

    HygieneRecord* h = &ds->hygiene[ds->hygiene_count];
    memset(h, 0, sizeof(*h));
    h->hygiene_id = ds->ids.next_hygiene_id++;
    h->room_id = rid;
    h->score = get_int("  评分（0-100）：");
    if (h->score < 0)   h->score = 0;
    if (h->score > 100) h->score = 100;

    printf("  备注（可留空）："); fflush(stdout);
    get_line(h->remark, sizeof(h->remark));
    get_current_time(h->check_time, sizeof(h->check_time));

    ds->hygiene_count++;
    save_hygiene(ds);
    printf("\n  卫生记录已录入，房号 %s，评分 %d\n", r->room_number, h->score);
    pause_screen();
}

//4. 违规信息录入
void manager_add_violation(DataStore* ds, DormManager* m) {
    (void)m;
    print_header("违规信息录入");

    if (ds->violation_count >= MAX_VIOLATIONS) {
        printf("  违规记录已满。\n"); pause_screen(); return;
    }

    ViolationRecord* v = &ds->violations[ds->violation_count];
    memset(v, 0, sizeof(*v));

    printf("  学生学号："); fflush(stdout);
    get_line(v->student_id, sizeof(v->student_id));
    Student* s = find_student(ds, v->student_id);
    if (!s) { printf("  未找到该学生。\n"); pause_screen(); return; }
    printf("  学生姓名：%s\n", s->name);

    printf("  违规类型（如：夜不归宿/私接电器/打架斗殴）："); fflush(stdout);
    get_line(v->type, sizeof(v->type));
    if (strlen(v->type) == 0) {
        printf("  类型不能为空。\n"); pause_screen(); return;
    }

    printf("  详细描述："); fflush(stdout);
    get_line(v->description, sizeof(v->description));

    v->violation_id = ds->ids.next_violation_id++;
    get_current_time(v->record_time, sizeof(v->record_time));
    ds->violation_count++;
    save_violations(ds);
    printf("\n  违规记录已录入，编号：%d\n", v->violation_id);
    pause_screen();
}

//5. 电费抄表录入
void manager_add_elec_reading(DataStore* ds, DormManager* m) {
    print_header("电费抄表录入");

    /* 显示本楼栋房间电费状态 */
    printf("  %-6s  %-8s  %-10s  %-10s  %s\n",
        "房间ID", "房号", "余额(元)", "上期(度)", "状态");
    print_line('-', 55);
    int found = 0;
    for (int i = 0; i < ds->room_count; i++) {
        Room* r = &ds->rooms[i];
        if (r->building_id != m->building_id) continue;
        float last = 0.0f;
        for (int j = ds->bill_count - 1; j >= 0; j--) {
            if (ds->bills[j].room_id == r->room_id) {
                last = ds->bills[j].curr_reading; break;
            }
        }
        const char* flag = "";
        if (r->elec_balance <= 0)
            flag = "[欠费]";
        else if (r->elec_balance <= r->elec_alert_threshold)
            flag = "[预警]";
        printf("  %-6d  %-8s  %-10.2f  %-10.1f  %s\n",
            r->room_id, r->room_number, r->elec_balance, last, flag);
        found++;
    }
    if (!found) { printf("  本楼栋暂无房间。\n"); pause_screen(); return; }

    int rid = get_int("\n  输入房间 ID：");
    Room* r = find_room(ds, rid);
    if (!r || r->building_id != m->building_id) {
        printf("  无效房间 ID。\n"); pause_screen(); return;
    }

    /* 找上期读数 */
    float last_reading = 0.0f;
    for (int j = ds->bill_count - 1; j >= 0; j--) {
        if (ds->bills[j].room_id == rid) {
            last_reading = ds->bills[j].curr_reading; break;
        }
    }
    printf("  上期读数：%.1f 度\n", last_reading);

    float curr = get_float("  本期读数（度）：");
    if (curr < last_reading) {
        printf("  本期读数不能小于上期（%.1f 度）。\n", last_reading);
        pause_screen(); return;
    }

    float price = get_float("  电价（元/度，如 0.55）：");
    if (price <= 0.0f) { printf("  电价无效。\n"); pause_screen(); return; }

    float usage = curr - last_reading;
    float cost = usage * price;
    r->elec_balance -= cost;
    save_rooms(ds);

    /* 写账单 */
    if (ds->bill_count < MAX_BILLS) {
        ElecBill* b = &ds->bills[ds->bill_count];
        memset(b, 0, sizeof(*b));
        b->bill_id = ds->ids.next_bill_id++;
        b->room_id = rid;
        b->last_reading = last_reading;
        b->curr_reading = curr;
        get_current_time(b->record_time, sizeof(b->record_time));
        ds->bill_count++;
        save_bills(ds);
    }

    printf("\n  用电量：%.1f 度  费用：%.2f 元  剩余余额：%.2f 元\n",
        usage, cost, r->elec_balance);
    if (r->elec_balance <= 0)
        printf("  [!] 该房间已欠费！请通知学生尽快充值。\n");
    else if (r->elec_balance <= r->elec_alert_threshold)
        printf("  [!] 该房间余额低于预警阈值（%.2f 元），请提醒学生充值。\n",
            r->elec_alert_threshold);
    pause_screen();
}

//6. 电费预警列表
void manager_view_elec_alerts(DataStore* ds, DormManager* m) {
    print_header("电费预警列表");

    int found = 0;
    for (int i = 0; i < ds->room_count; i++) {
        Room* r = &ds->rooms[i];
        if (r->building_id != m->building_id) continue;
        if (r->elec_balance > r->elec_alert_threshold) continue;

        if (!found) {
            printf("  %-8s  %-10s  %-10s  %s\n",
                "房号", "余额(元)", "阈值(元)", "状态");
            print_line('-', 45);
        }
        const char* st = (r->elec_balance <= 0) ? "【欠费】" : "【余额不足】";
        printf("  %-8s  %-10.2f  %-10.2f  %s\n",
            r->room_number, r->elec_balance, r->elec_alert_threshold, st);
        found++;
    }
    if (!found) printf("  当前无预警房间。\n");
    pause_screen();
}

//7. 修改密码
void manager_change_password(DataStore* ds, DormManager* m) {
    print_header("修改密码");

    char old_pwd[32], new_pwd[32], confirm[32];

    printf("  当前密码："); fflush(stdout);
    get_line(old_pwd, sizeof(old_pwd));
    if (!check_password(old_pwd, m->password)) {
        printf("  原密码错误。\n"); pause_screen(); return;
    }

    printf("  新密码（至少6位）："); fflush(stdout);
    get_line(new_pwd, sizeof(new_pwd));
    if ((int)strlen(new_pwd) < 6) {
        printf("  密码至少6位。\n"); pause_screen(); return;
    }

    printf("  确认新密码："); fflush(stdout);
    get_line(confirm, sizeof(confirm));
    if (strcmp(new_pwd, confirm) != 0) {
        printf("  两次输入不一致。\n"); pause_screen(); return;
    }

    encrypt_password(new_pwd, m->password);
    save_managers(ds);
    printf("  密码修改成功。\n");
    pause_screen();
}

//宿管主菜单
void manager_menu(DataStore* ds, DormManager* m) {
    int choice;
    do {
        print_header("宿管端 — 功能菜单");
        Building* b = find_building(ds, m->building_id);
        printf("  欢迎，%s  |  负责楼栋：%s\n\n",m->name, b ? b->building_name : "未分配");
        printf("  [1]  报修申请处理\n");
        printf("  [2]  访客登记录入\n");
        printf("  [3]  卫生检查录入\n");
        printf("  [4]  违规信息录入\n");
        printf("  [5]  电费抄表录入\n");
        printf("  [6]  电费预警列表\n");
        printf("  [7]  修改密码\n");
        printf("  [0]  退出登录\n");
        print_line('-', 60);
        choice = get_int("  请选择：");
        switch (choice) {
        case 1: manager_process_repairs(ds, m);   break;
        case 2: manager_add_visitor(ds, m);       break;
        case 3: manager_add_hygiene(ds, m);       break;
        case 4: manager_add_violation(ds, m);     break;
        case 5: manager_add_elec_reading(ds, m);  break;
        case 6: manager_view_elec_alerts(ds, m);  break;
        case 7: manager_change_password(ds, m);   break;
        case 0: break;
        default: printf("  无效选项。\n");
        }
    } while (choice != 0);
}
