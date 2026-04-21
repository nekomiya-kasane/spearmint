#pragma once

#if defined(_WIN32) || defined(_WIN64)
#    ifdef SPEARMINT_BUILD_INTERNAL
#        define SPEARMINT_API __declspec(dllexport)
#    else
#        define SPEARMINT_API __declspec(dllimport)
#    endif
#else
#    ifdef SPEARMINT_BUILD_INTERNAL
#        define SPEARMINT_API __attribute__((visibility("default")))
#    else
#        define SPEARMINT_API
#    endif
#endif
