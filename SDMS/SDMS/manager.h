//manager.h — 宿管端功能接口
#ifndef MANAGER_H
#define MANAGER_H

#include "structs.h"

void manager_menu(DataStore* ds, DormManager* m);

void manager_process_repairs(DataStore* ds, DormManager* m);
void manager_add_visitor(DataStore* ds, DormManager* m);
void manager_add_hygiene(DataStore* ds, DormManager* m);
void manager_add_violation(DataStore* ds, DormManager* m);
void manager_add_elec_reading(DataStore* ds, DormManager* m);
void manager_view_elec_alerts(DataStore* ds, DormManager* m);
void manager_change_password(DataStore* ds, DormManager* m);

#endif /* MANAGER_H */
