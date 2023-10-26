#pragma once

#if defined(GEM_RUNTIME_IMPORT)
#define GEM_RUNTIME_API __declspec(dllimport)
#elif defined(GEM_RUNTIME_EXPORT)
#define GEM_RUNTIME_API __declspec(dllexport)
#else
#error Either GEM_RUNTIME_IMPORT or GEM_RUNTIME_EXPORT must be defined
#endif

#if !defined(GEM_DEBUG) && !defined(GEM_RELEASE)
#error Either GEM_DEBUG or GEM_RELEASE must be defined
#endif
