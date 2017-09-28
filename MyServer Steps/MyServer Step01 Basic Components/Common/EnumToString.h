/** @file   EnumToString.h
    @desc   macros of EnumToString
    @author jintaeks@kogstudios.com
    @since  2012-6-22, 13:24
*/
#pragma once

///-----------------------------------------------------------------------------
#define STRINGIZE_ENUM(arg)  STRINGIZE_ENUM1(arg)
#define STRINGIZE_ENUM1(arg) STRINGIZE_ENUM2(arg)
#define STRINGIZE_ENUM2(arg) #arg

///-----------------------------------------------------------------------------
#define CONCATENATE_ENUM(arg1, arg2)   CONCATENATE_ENUM1(arg1, arg2)
#define CONCATENATE_ENUM1(arg1, arg2)  CONCATENATE_ENUM2(arg1, arg2)
#define CONCATENATE_ENUM2(arg1, arg2)  arg1##arg2

///-----------------------------------------------------------------------------
#define BEGIN_PACKET_ENUM(type) \
    static const char*  type##ToString( int iEnum_ ) \
    {

///-----------------------------------------------------------------------------
#define END_PACKET_ENUM() ; return ""; }

///-----------------------------------------------------------------------------
#define EXPAND_ENUM( x )     x

///-----------------------------------------------------------------------------
#define PRN_ENUM_TEXT_1( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field);
#define PRN_ENUM_TEXT_2( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_1( __VA_ARGS__) )
#define PRN_ENUM_TEXT_3( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_2( __VA_ARGS__) )
#define PRN_ENUM_TEXT_4( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_3( __VA_ARGS__) )
#define PRN_ENUM_TEXT_5( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_4( __VA_ARGS__) )
#define PRN_ENUM_TEXT_6( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_5( __VA_ARGS__) )
#define PRN_ENUM_TEXT_7( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_6( __VA_ARGS__) )
#define PRN_ENUM_TEXT_8( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_7( __VA_ARGS__) )
#define PRN_ENUM_TEXT_9( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_8( __VA_ARGS__) )
#define PRN_ENUM_TEXT_10( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_9( __VA_ARGS__) )
#define PRN_ENUM_TEXT_11( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_10( __VA_ARGS__) )
#define PRN_ENUM_TEXT_12( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_11( __VA_ARGS__) )
#define PRN_ENUM_TEXT_13( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_12( __VA_ARGS__) )
#define PRN_ENUM_TEXT_14( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_13( __VA_ARGS__) )
#define PRN_ENUM_TEXT_15( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_14( __VA_ARGS__) )
#define PRN_ENUM_TEXT_16( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_15( __VA_ARGS__) )
#define PRN_ENUM_TEXT_17( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_16( __VA_ARGS__) )
#define PRN_ENUM_TEXT_18( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_17( __VA_ARGS__) )
#define PRN_ENUM_TEXT_19( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_18( __VA_ARGS__) )
#define PRN_ENUM_TEXT_20( field, ... ) \
    if( iEnum_ == field ) return STRINGIZE_ENUM(field); \
    EXPAND_ENUM( PRN_ENUM_TEXT_19( __VA_ARGS__) )

///-----------------------------------------------------------------------------
#define PRN_ENUM_TEXT_NARG(...) EXPAND_ENUM( PRN_ENUM_TEXT_NARG_(__VA_ARGS__, PRN_ENUM_TEXT_RSEQ_N()) )
#define PRN_ENUM_TEXT_NARG_(...) EXPAND_ENUM( PRN_ENUM_TEXT_ARG_N(__VA_ARGS__) )
#define PRN_ENUM_TEXT_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, ...) N
#define PRN_ENUM_TEXT_RSEQ_N() 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define PRN_ENUM_TEXT_( N, ... ) CONCATENATE_ENUM(PRN_ENUM_TEXT_, N)( __VA_ARGS__)

#define PRN_ENUM_TEXT( ... ) EXPAND_ENUM( PRN_ENUM_TEXT_( EXPAND_ENUM( PRN_ENUM_TEXT_NARG( __VA_ARGS__) ), __VA_ARGS__ ) )

///-----------------------------------------------------------------------------
#define IMPLEMENT_ENUM( type, ... ) \
    BEGIN_PACKET_ENUM(type) \
        EXPAND_ENUM( PRN_ENUM_TEXT( __VA_ARGS__ ) ) \
    END_PACKET_ENUM()

///-----------------------------------------------------------------------------
#define DECLARE_ENUM( type, ... ) \
    enum type\
    { \
    __VA_ARGS__ \
    }; \
    EXPAND_ENUM( IMPLEMENT_ENUM(type, __VA_ARGS__) );


/** @exam
    @code

    class KTestPacket
    {
    public:
        DECLARE_ENUM
        (
            INVALID,
            SUCCESS,
            ERROR_DB_FAILED,
            ERROR_PARAMETER,
            ERROR_RESULT
        );

    public:
        int                 m_nOk;
        std::string         m_strText;

                            KTestPacket()
                                : m_nOk(INVALID)
                                , m_strText()
                            {
                            }//KTestPacket()
    };//class KTestPacket


    int _tmain( int argc, _TCHAR* argv[] )
    {
        KTestPacket     ack;

        std::cout << KTestPacket::INVALID << std::endl;
        std::cout << KTestPacket::SUCCESS << std::endl;
        std::cout << KTestPacket::ERROR_PARAMETER << std::endl;
        std::cout << KTestPacket::EnumToString( KTestPacket::INVALID ) << std::endl;
        std::cout << KTestPacket::EnumToString( KTestPacket::SUCCESS ) << std::endl;
        std::cout << KTestPacket::EnumToString( KTestPacket::ERROR_RESULT ) << std::endl;

        /// output:
        ///    0
        ///    1
        ///    3
        ///    INVALID
        ///    SUCCESS
        ///    ERROR_RESULT
        ///    계속하려면 아무 키나 누르십시오 . . .
        ///

        return 0;
    }//_tmain()

    @endcode
*/
