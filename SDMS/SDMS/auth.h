//auth.h — 登录认证接口
#ifndef AUTH_H
#define AUTH_H

#include "structs.h"

//三端登录函数
//返回值：找到且密码正确时返回非 NULL 指针（学生/宿管/管理员），否则 NULL
Student* auth_login_student(DataStore* ds, const char* id, const char* plain_pwd);
DormManager* auth_login_manager(DataStore* ds, const char* id, const char* plain_pwd);
Admin* auth_login_admin(DataStore* ds, const char* username, const char* plain_pwd);

#endif /* AUTH_H */
