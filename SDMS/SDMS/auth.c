#define _CRT_SECURE_NO_WARNINGS
//auth.c — 登录认证实现（Caesar +3 密码校验）
#include "auth.h"
#include "file.h"

Student* auth_login_student(DataStore* ds, const char* id, const char* plain_pwd) {
    Student* s = find_student(ds, id);
    if (!s) return NULL;
    return check_password(plain_pwd, s->password) ? s : NULL;
}

DormManager* auth_login_manager(DataStore* ds, const char* id, const char* plain_pwd) {
    DormManager* m = find_manager(ds, id);
    if (!m) return NULL;
    return check_password(plain_pwd, m->password) ? m : NULL;
}

Admin* auth_login_admin(DataStore* ds, const char* username, const char* plain_pwd) {
    for (int i = 0; i < ds->admin_count; i++) {
        Admin* a = &ds->admins[i];
        if (strcmp(a->username, username) == 0 &&
            check_password(plain_pwd, a->password))
            return a;
    }
    return NULL;
}
