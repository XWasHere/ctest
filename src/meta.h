#ifndef CTEST_META_H
#define CTEST_META_H

namespace ctest {
    template<template<class ...> class TEMPLATE, class ...TYPE> struct is_instantiation_of                              : std::false_type {};
    template<template<class ...> class TEMPLATE, class ...TYPE> struct is_instantiation_of<TEMPLATE, TEMPLATE<TYPE...>> : std::true_type  {};
};

#endif