//
//  cl_reg.h
//  cocos2dx
//
//  Created by Saul Howard on 12/2/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "instantiator.h"

#ifndef CL_TYPE
# error "You must define CL_TYPE before including this file."
#endif

#ifndef CL_TYPE_NAME
# error "You must define CL_TYPE_NAME before including this file."
#endif

#ifndef CL_BASE_TYPE
# define CL_BASE_TYPE CL_TYPE
#endif

namespace { // again, important for linking reasons

# ifndef cl_CLLITE_REG_CONTEXT_DEFINED
# define cl_CLLITE_REG_CONTEXT_DEFINED 1

    ///////////////////////////////////////////////////////////////
    // we must not include this bit more than once per compilation
    // unit...
    ///////////////////////////////////////////////////////////////
    // A unique (per Context/per compilation unit) space to assign
    // a bogus value for classloader registration purposes.
    template <typename Context>
    struct cllite_reg_context {
        static bool placeholder;
    };
    template <typename Context> bool
    cllite_reg_context<Context>::placeholder = false;

# endif // !cl_CLLITE_REG_CONTEXT_DEFINED
    
    // The rest of the supermacro may be included multiple times...
    ////////////////////////////////////////////////////////////////////////
    // Register a factory with the classloader during static initialization:
    template<> bool cllite_reg_context<CL_TYPE>::placeholder = (
#ifdef CL_ABSTRACT_BASE
                                                      // register a no-op factory:
                                                      instantiator<CL_TYPE>::register_factory<CL_TYPE>(
                                                                                                CL_TYPE_NAME,
                                                                                                object_factory<CL_TYPE>::no_instance
                                                                                                ),
#else
                                                      // register the default factory:
//                                                      instantiator::register_subtype<CL_BASE_TYPE>(CL_TYPE_NAME),
                                                        instantiator<CL_BASE_TYPE>::Instance().register_subtype<CL_TYPE>(CL_TYPE_NAME),
#endif // CL_ABSTRACT_BASE
                                                      true); // assign a no-meaning value to the placeholder var
} // anon namespace

// By convention, supermacros undefine all of their parameters after
// use so they can be called repeatedly without clients having to
// undef everything:
#undef CL_TYPE
#undef CL_BASE_TYPE
#undef CL_TYPE_NAME

#ifdef CL_ABSTRACT_BASE
#undef CL_ABSTRACT_BASE

#endif