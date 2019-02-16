#ifndef CONFIG_H
#define CONFIG_H

#ifdef __linux
    #include "linux_config.h"
#else
    #include "windows_config.h"
#endif


#endif  // CONFIG_H
