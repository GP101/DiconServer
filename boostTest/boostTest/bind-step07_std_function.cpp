#include <functional>
#include <iostream>

struct Foo {
    Foo( int num ) : num_( num ) {}
    void print_add( int i ) { std::cout << num_ + i << '\n'; }
    int num_;
};

void print_num( int i ) {
    std::cout << i << '\n';
}

struct PrintNum {
    void operator()( int i ) const {
        std::cout << i << '\n';
    }
};

int main() {
    // store a free function
    std::function<void( int )> f_display = print_num;
    f_display( -9 );

    // store a lambda
    std::function<void()> f_display_42 = []() { print_num( 42 ); };
    f_display_42();

    // store a call to a member function
    using namespace std::placeholders;
    const Foo foo( 314159 );
    std::function<void( int )> f_add_display = std::bind( &Foo::print_add, foo, _1 );
    f_add_display( 1 );

    // store a call to a function object
    std::function<void( int )> f_display_obj = PrintNum();
    f_display_obj( 18 );
}
