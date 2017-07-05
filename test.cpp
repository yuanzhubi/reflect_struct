
#include "reflect_struct.h"
#include <iostream>

#include <string>

struct test_type0{
DEF_FIELD_BEGIN(test_type0)
    DEF_FIELD(int, x)
    DEF_FIELD(std::string, y)
DEF_FIELD_END
};

struct test_type1{
DEF_FIELD_BEGIN(test_type1)
    DEF_FIELD(test_type0, z)
    DEF_FIELD(std::string, w)
DEF_FIELD_END
};

struct iterate_output{
    template<typename T>
    void operator()(const T& arg, typename reflect_struct::not_reflect_struct<T>::type *p = 0) const{(void)p;
        std::cout << arg << std::endl;
    }
    template<typename T>
    void operator()(const T&, typename T::this_is_reflect_struct* p = 0 ) const{(void)p;
        std::cout << std::endl;
    }
};

struct iterate_output_with_name{
    template<typename T>
    void operator()(const T& arg, const char* name, typename reflect_struct::not_reflect_struct<T>::type *p = 0) const{(void)p;
        std::cout << name <<" : " << arg << std::endl;
    }

    template<typename T>
    void operator()(const T&, const char* name, typename T::this_is_reflect_struct* p = 0 ) const{(void)p;
        std::cout << "reflect_struct : " << name << std::endl;
    }
};

struct iterate_output_with_name_depth{
    int current_depth;
    iterate_output_with_name_depth():current_depth(-1){
    }
    void prefix_init(int depth){
        if(this->current_depth > depth){
             std::cout << "}, " ;
             this->current_depth = depth;
        }
        else if(this->current_depth == depth){
             std::cout << ", " ;
        }
        else{
            this->current_depth = depth;
        }
    }

    template<typename T>
    void operator()(const T& arg, const char* name, int depth, typename reflect_struct::not_reflect_struct<T>::type *p = 0) {(void)p;
        prefix_init(depth);
        std::cout <<'"'<< name <<"\" : " << arg ;
    }

    void operator()(const std::string& arg, const char* name, int depth) {
        prefix_init(depth);
        std::cout <<'"'<< name <<"\" : \"" << arg <<'"'   ;
    }

    template<typename T>
    void operator()(const T&, const char* name, int depth, typename T::this_is_reflect_struct* p = 0) {(void)p;
        prefix_init(depth);
        std::cout <<'"'<< name <<"\" : {" ;
    }
};


int main(){
    iterate_output i1;
    iterate_output_with_name i2;
    iterate_output_with_name_depth i3;
    test_type1 instance;
    instance.z.x = 1;
    instance.z.y = "2";
    instance.w = "3";

    std::cout << "show all the elements" << std::endl;
    instance.iterate_all(i1);
    std::cout << std::endl;

    std::cout << "show all the elements with name" << std::endl;
    instance.iterate_all_with_name(i2);
    std::cout << std::endl;

    std::cout << "show in json type" << std::endl;
    std::cout << '{';
    instance.iterate_all_with_name_depth(i3);
    std::cout << '}';
    std::cout << std::endl;
    return 0;
}
