/* ----------------------------------------------------------------------------
** Copyright (c) 2016 Austin Brunkhorst, All Rights Reserved.
**
** ReflectionDatabase.cpp
** --------------------------------------------------------------------------*/

#include "Precompiled.h"

#include "ReflectionDatabase.h"

#include "Type.h"

#define REGISTER_NATIVE_TYPE(type)                    \
    {                                                 \
        auto id = AllocateType( #type );              \
        auto &handle = types[ id ];                   \
                                                      \
        TypeInfo<type>::Register( id, handle, true ); \
    }                                                 \

#define REGISTER_NATIVE_TYPE_VARIANTS(type) \
    REGISTER_NATIVE_TYPE( type )            \
    REGISTER_NATIVE_TYPE( type* )           \
    REGISTER_NATIVE_TYPE( const type* )     \

#define REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(type)           \
    REGISTER_NATIVE_TYPE_VARIANTS( type )                     \
    types[ TypeInfo<type>::ID ].SetArrayConstructor<type>( ); \

namespace ursine
{
    namespace meta
    {
        ReflectionDatabase::ReflectionDatabase(void)
            : types( 1 )
            , m_nextID( 1 )
        {
            types[ Type::Invalid ].name = "UNKNOWN";

            // register all of the native type variants explicity, before
            // anything else
            REGISTER_NATIVE_TYPE_VARIANTS( void );
            REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY( int );
            REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY( unsigned int );
            REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY( bool );
            REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY( float );
            REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY( double );
            REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY( std::string );

            auto &stringType = types[ TypeInfo<std::string>::ID ];

            // explicitly add default constructors for string

            stringType.AddConstructor<std::string, false, false>( { } );
            stringType.AddConstructor<std::string, false, true>( { } );
        }

        ///////////////////////////////////////////////////////////////////////

        ReflectionDatabase::~ReflectionDatabase(void) { }

        ///////////////////////////////////////////////////////////////////////

        ReflectionDatabase &ReflectionDatabase::Instance(void)
        {
            static ReflectionDatabase instance;

            return instance;
        }

        ///////////////////////////////////////////////////////////////////////

        TypeID ReflectionDatabase::AllocateType(const std::string &name)
        {
            auto search = ids.find( name );

            // already defined
            if (search != ids.end( ))
                return Type::Invalid;

            types.emplace_back( name );

            auto id = m_nextID++;

            ids[ name ] = id;

            return id;
        }

        ///////////////////////////////////////////////////////////////////////

        const Function &ReflectionDatabase::GetGlobalFunction(
            const std::string &name
        )
        {
            auto &base = globalFunctions[ name ];

            if (!base.size( ))
                return Function::Invalid( );

            return base.begin( )->second;
        }

        ///////////////////////////////////////////////////////////////////////

        const Function &ReflectionDatabase::GetGlobalFunction(
            const std::string &name, 
            const InvokableSignature &signature
        )
        {
            auto &base = globalFunctions[ name ];

            auto search = base.find( signature );

            if (search == base.end( ))
                return Function::Invalid( );

            return search->second;
        }
    }
}
