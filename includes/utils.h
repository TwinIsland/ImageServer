#pragma once

#include <unistd.h>

#define is_file_exist(name) (access(name, F_OK) == 0)

