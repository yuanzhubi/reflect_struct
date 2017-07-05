#if !defined(DEF_STRUCT_BEGIN)

//Template nesting 168 times may be safe for most compiler. You can increase it if your compiler can afford
#define MAX_MEMBER_COUNT 168

#define REFELECT_STRING2(x) #x
#define REFELECT_STRING(x) REFELECT_STRING2(x)

#define REFELECT_JOIN2(x, y) x##y
#define REFELECT_JOIN(x, y) REFELECT_JOIN2(x, y)

namespace reflect_struct{
    template<unsigned int N>
    struct struct_int : struct_int<N - 1> {unsigned int content;};
    template<>
    struct struct_int<0> {};

    typedef struct_int<2> yes_int;
    typedef struct_int<1> no_int;


    template<typename T> yes_int is_reflect_struct_tester(typename T::this_is_reflect_struct*);
    template<typename T> no_int is_reflect_struct_tester(...);
    template<typename T>
    struct is_reflect_struct{
        static const bool result = (sizeof(is_reflect_struct_tester<T>(0)) == sizeof(yes_int));
    };

    template<typename T, bool result = is_reflect_struct<T>::result>
    struct not_reflect_struct{};

    template<typename T>
    struct not_reflect_struct<T, false>{
        typedef int type;
    };

    struct dummy_type{
        //using ... with C++ reference arguments will lead some compiler warning. So we manually overload every cases;

        template<typename This, typename Func>
        static inline void iterate_all(const This&, Func& ) {
        }
        template<typename This, typename Func>
        static inline void iterate_all_with_name(const This&, Func& ) {
        }
        template<typename This, typename Func>
        static  inline void iterate_all_with_name_depth(const This&, Func&, unsigned int)  {
        }

        template<typename Func>
        inline void iterate_all(Func& ) const{
        }
        template<typename Func>
        inline void iterate_all_with_name(Func& ) const{
        }
        template<typename Func>
        inline void iterate_all_with_name_depth(Func&, unsigned int) const {
        }
    };

    template <typename T, bool is_reflect = is_reflect_struct<T>::result>
    struct reflect_struct_adapter{
        typedef T type;
    };

    template <typename T>
    struct reflect_struct_adapter<T, false>{
        typedef dummy_type type;
    };


}
#define REFELECT_INDEXER(counter) ((sizeof(*counter((reflect_struct::struct_int<MAX_MEMBER_COUNT>*)0)) - sizeof(*counter((void*)0))) / sizeof(char))

#define REFELECT_INCREASER(counter, name)  \
    static const int name = REFELECT_INDEXER(counter); \
    static char (*counter(reflect_struct::struct_int<sizeof(*counter((void*)0) ) / sizeof(char) + name + 1>*))[sizeof(*counter((void*)0)) / sizeof(char) + name + 1];

#define FIELD_INDEX(name) REFELECT_JOIN(reflect_tag_, name)

//DEF_FIELD_BEGIN provides member function interface, so do not put it after "private:"!
#define DEF_FIELD_BEGIN(class_name) \
    typedef class_name this_type; \
    typedef class_name this_is_reflect_struct; \
    static char (*field_counter(...))[1]; \
    template<typename T, int N = -1> \
    struct members_iterator: public reflect_struct::dummy_type{ \
    }; \
    template <typename T, int N > friend struct members_iterator; \
\
    template<typename Func> \
    void iterate_all(Func& func_instance) const{ \
        members_iterator<int, 0 >::iterate_all(*this, func_instance); \
    } \
    template<typename Func> \
    void iterate_all_with_name(Func& func_instance) const{ \
        members_iterator<int, 0 >::iterate_all_with_name(*this, func_instance); \
    } \
    template<typename Func> \
    void iterate_all_with_name_depth(Func& func_instance, int depth = 0) const{ \
        members_iterator<int, 0 >::iterate_all_with_name_depth(*this, func_instance, depth); \
    } \

#define DEF_FIELD(input_type, name) \
    input_type name; \
    REFELECT_INCREASER(field_counter, FIELD_INDEX(name)) \
    template <typename T> \
    struct members_iterator<T, FIELD_INDEX(name) > { \
        typedef members_iterator<T, FIELD_INDEX(name) + 1 > next_type; \
        template<typename Func> \
        static void iterate_all(const this_type& this_instance, Func& func_instance){ \
            func_instance(this_instance.name); \
            if(reflect_struct::is_reflect_struct<input_type>::result){ \
                typedef const reflect_struct::reflect_struct_adapter<input_type>::type* adapter_type; \
                (static_cast<adapter_type>((void*)&this_instance))->iterate_all(func_instance); \
            } \
            next_type::iterate_all(this_instance, func_instance); \
        } \
        template<typename Func> \
        static void iterate_all_with_name(const this_type& this_instance, Func& func_instance){ \
            func_instance( this_instance.name, REFELECT_STRING(name)); \
            if(reflect_struct::is_reflect_struct<input_type>::result){ \
                typedef const reflect_struct::reflect_struct_adapter<input_type>::type* adapter_type; \
                (static_cast<adapter_type>((void*)&this_instance))->iterate_all_with_name(func_instance); \
            } \
            next_type::iterate_all_with_name(this_instance, func_instance); \
        } \
        template<typename Func> \
        static void iterate_all_with_name_depth(const this_type& this_instance, Func& func_instance, int depth){ \
            func_instance(this_instance.name, REFELECT_STRING(name), depth); \
            if(reflect_struct::is_reflect_struct<input_type>::result){ \
                typedef const reflect_struct::reflect_struct_adapter<input_type>::type* adapter_type; \
                (static_cast<adapter_type>((void*)&this_instance))->iterate_all_with_name_depth(func_instance, depth + 1); \
            } \
            next_type::iterate_all_with_name_depth(this_instance, func_instance, depth); \
        } \
    }; \


//This is optional, if you want to get the member count;
#define DEF_FIELD_END \
    static const unsigned int field_count = REFELECT_INDEXER(field_counter); \

#endif
