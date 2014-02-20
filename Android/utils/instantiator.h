//
//  instantiator.h
//  cocos2dx
//
//  Created by Saul Howard on 12/2/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_instantiator_h
#define cocos2dx_instantiator_h

#include "AndroidMacros.h"

#include "cocos2d.h"

#include <vector>
#include <string>
#include <map>
#include <memory>

using namespace std;

template <typename BaseT, typename SubT = BaseT> // SubT must be-a BaseT.
struct object_factory {
    static shared_ptr<BaseT> new_instance(androidcpp::Context *context, androidcpp::AttributeSet *attrs) { return make_shared<SubT>(context, attrs); }
    static shared_ptr<BaseT> no_instance() { return 0; } // explained below
};

template <typename BaseType>
class instantiator {
    
private:
    
    typedef shared_ptr<BaseType> (*factory_type) (androidcpp::Context *context, androidcpp::AttributeSet *attrs); // factory function signature
    typedef map<string, factory_type> object_factory_map; // key-to-factory map
    
    class ObjectFactory {
    public:
        object_factory_map map;
        
        ObjectFactory() {
            map = object_factory_map();
        }
    };
    
    ObjectFactory s_objectFactory;
    
public:
    
    static instantiator<BaseType> &Instance() {
        static instantiator<BaseType> singleton;
        return singleton;
    }
    
    // Uses a mapped factory to return a new value_type object
    // polymorphically:
    shared_ptr<BaseType> instantiate(const string &key, androidcpp::Context *context, androidcpp::AttributeSet *attrs) {
        
        typename object_factory_map::const_iterator it = s_objectFactory.map.find(key);
        
        if (it != s_objectFactory.map.end()) {
            return (it->second)(context, attrs); // run our factory.
        }
        
        return NULL; // no factory found :(
    }
    
    // Maps the given BaseType factory with the given key.
    // You may pass a factory which returns a subtype, but the
    // default factory will always return an actual value_type object
    // (assuming value_type is not abstract, in which case it returns
    // 0).
    // Note that by providing a default factory here we make a trade-off:
    // this code will not compile when value_type is abstract. If we
    // force the user to specify a factory we can support abstract types,
    // but we almost double the amount of code needed to register types,
    // as demonstrated later. Later in this paper we will mention a cleaner
    // solution which allows us to keep the default argument here and support
    // abstract types.
    void register_factory(const string &key, factory_type fp = 0) {
        
        if (!fp) fp = object_factory<BaseType>::new_instance;
        
        s_objectFactory.map.insert(make_pair(key, fp));
    }
    
    // Registers a factory for a subtype of base_type.
    // This is the same as calling register_factory( key, mySubTypeFactory ),
    // except that it installs a default factory returning a SubOfBaseType
    // if the client does not provide one. SubOfBaseType must be a publically
    // accessible ancestor of base_type, or must be base_type.
    // Please see the notes in register_factory() regarding the second
    // parameter: the same notes apply here.
    template <typename SubOfBaseType>
    void register_subtype(const string &key, factory_type fp = 0) {
        
        if (!fp) fp = object_factory<BaseType, SubOfBaseType>::new_instance;
        
        register_factory(key, fp);
    }
    
    // Tells us if a given key is registered with a base_type
    // factory. Only rarely useful, but here it is...
    bool is_registered(const string &key) {
        return s_objectFactory.map.end() != s_objectFactory.map.find(key);
    }
};

#endif
