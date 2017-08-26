#include "StringFormat.h"

//Source: http://coliru.stacked-crooked.com/a/511842875075aa26
std::string StringFormat(std::string format, ...) {
    va_list args, args_copy;

    va_start(args, format);
    va_copy(args_copy, args) ;

    const auto sz = std::vsnprintf(nullptr, 0, format.c_str(), args ) + 1 ;

    try {
        std::string result( sz, ' ' ) ;
        std::vsnprintf( &result.front(), sz, format.c_str(), args_copy ) ;

        va_end(args_copy) ;
        va_end(args) ;

        // do whatever else with result

        return result ;
    }
    catch(const std::bad_alloc&){
        va_end(args_copy) ;
        va_end(args) ;
        throw ;
    }
}
