#pragma once

#if defined(GEM_IMPORT)
#define GEM_API __declspec(dllimport)
#elif defined(GEM_EXPORT)
#define GEM_API __declspec(dllexport)
#else
#error Either GEM_IMPORT or GEM_EXPORT must be defined
#endif

#if !defined(GEM_DEBUG) && !defined(GEM_RELEASE)
#error Either GEM_DEBUG or GEM_RELEASE must be defined
#endif
