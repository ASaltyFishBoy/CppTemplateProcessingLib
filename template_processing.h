#ifndef TEMPLATE_PROCESSING_H
#define TEMPLATE_PROCESSING_H

#include <cstddef>
#include <type_traits>

template<typename Type>
struct template_exploser;

template<template<typename...>class Template, typename ...Args>
struct template_exploser<Template<Args...>>
{
    template<typename...NewArgs>
    using type = Template<NewArgs...>;
    template<template<typename...>class NewTemplate>
    using apply = NewTemplate<Args...>;

    static constexpr size_t size = sizeof... (Args);
};

template <typename Type, template <typename ...> class Template>
struct is_instance_of
{
private:
    template<typename ...CheckType>
    static void check (Template<CheckType...>);
    template<typename ...CheckType>
    static bool check (...);
public:
    static constexpr bool value = std::is_void<decltype (check (std::declval<Type>())) >::value;
};

template<size_t i, typename FillType = void>
struct template_filler
{
    template<template<typename...>class Template, typename ...Args>
    using apply = typename template_filler < i - 1 >::template apply<Template, Args..., FillType>;
};

template<>
struct template_filler<0>
{
    template<template<typename...>class Template, typename ...Args>
    using apply = Template<Args...>;
};

template<template<typename...>class Template, size_t i = 0
         , typename = void
         >
struct template_size
{
    static constexpr size_t value = template_size < Template, i + 1 >::value;
    using type = typename template_size < Template, i + 1 >::type;
};
template<template<typename...>class Template, size_t i>
struct template_size<Template, i, std::void_t<typename template_filler<i>::template apply<Template>>>
{
    using type = typename template_filler<i>::template apply<Template>;
    static constexpr size_t value = template_exploser<type>::size;
};

template<template<typename...>class TemplateFirst, template<typename...>class TemplateSecond>
struct is_same_template
{
private:
    template<template<typename...>class TemplateCheck>
    static void check (typename template_size<TemplateCheck>::type*);
    template<template<typename...>class TemplateCheck>
    static bool check (...);
public:
    static constexpr bool value = std::is_void <
                                  decltype (check<TemplateFirst> (static_cast<typename template_size<TemplateSecond>::type*> (nullptr)))
                                  >::value;
};

#endif
