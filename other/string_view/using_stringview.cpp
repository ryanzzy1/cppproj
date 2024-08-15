#include <string_view>
#include <iostream>


/* 
template <class CharT,
          class Traits = std::char_traits<CharT>>
class basic_string_view; 
*/

int main()
{
    constexpr std::string_view str_view("abcd");

    constexpr auto begin = str_view.begin();
    constexpr auto cbegin = str_view.cbegin();
    static_assert(
        *begin == 'a' and
        *cbegin == 'a' and
        *begin == *cbegin and
        begin == cbegin and
        // std::same_as<decltype(begin), decltype(cbegin)>
        std::is_same<decltype(begin), decltype(cbegin)>::value == true
        );
    
    constexpr auto end = str_view.end();
    constexpr auto cend = str_view.cend();
    static_assert(
        *std::prev(end) == 'd' && 'd' == *std::prev(cend) and end == cend
    );

    std::cout << "Done.\n";
}