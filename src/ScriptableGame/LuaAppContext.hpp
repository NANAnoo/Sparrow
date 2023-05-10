#include <sol/sol.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtx/euler_angles.hpp>

extern "C"{
#include <luasocket.h>
}

namespace SPW {
    class LuaAppContext {
    public:
        LuaAppContext() = default;
        ~LuaAppContext() {
            onInit = sol::nil;
            beforeUpdate = sol::nil;
            onUpdate = sol::nil;
            afterUpdate = sol::nil;
            onStop = sol::nil;
            onWindowResize = sol::nil;
        }
        // init lua state and load libraries, debug port is for remote debug
        inline void init(int debug_port) {
            m_state.open_libraries(sol::lib::base,
                                sol::lib::package,
                                sol::lib::coroutine,
                                sol::lib::string,
                                sol::lib::os,
                                sol::lib::math,
                                sol::lib::table,
                                sol::lib::debug,
                                sol::lib::bit32,
                                sol::lib::io,
                                sol::lib::utf8);

            std::string x = m_state["package"]["path"];
            m_state["package"]["path"] = x + ";./LuaLib/?.lua;./resources/scripts/lua/?.lua";

            // glm::vec3
            {
                // bind multiple constructors for vec3
                auto glm_ns_table = m_state["glm"].get_or_create<sol::table>();
                glm_ns_table.new_usertype<glm::vec3>("vec3",
                        sol::call_constructor,sol::constructors<glm::vec3(const float&, const float&, const float&)>(),
                        "x", &glm::vec3::x,
                        "y", &glm::vec3::y,
                        "z", &glm::vec3::z,
                        "r", &glm::vec3::r,
                        "g", &glm::vec3::g,
                        "b", &glm::vec3::b,
                        sol::meta_function::to_string,[] (const glm::vec3* vec) -> std::string {return glm::to_string(*vec);},
                        sol::meta_function::addition,[] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)+(*vec_b);},
                        sol::meta_function::subtraction,[] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)-(*vec_b);},
                        sol::meta_function::multiplication,[] (const glm::vec3* vec,const float a) {return (*vec)*a;},
                        sol::meta_function::division,[] (const glm::vec3* vec,const float a) {return (*vec)/a;},
                        sol::meta_function::unary_minus,[] (const glm::vec3* vec) {return (*vec)*-1.f;},
                        sol::meta_function::equal_to,[] (const glm::vec3* vec_a,const  glm::vec3* vec_b) {return (*vec_a)==(*vec_b);}
                        );
                // bind a function to make vec3 from a vec4
                glm_ns_table.set_function("toVec3", [](const glm::vec4& v) { return glm::vec3(v); });
            }

            // glm::vec4
            {
                auto glm_ns_table = m_state["glm"].get_or_create<sol::table>();
                glm_ns_table.new_usertype<glm::vec4>("vec4",sol::call_constructor,sol::constructors<glm::vec4(const float&, const float&, const float&, const float&)>(),
                        "x", &glm::vec4::x,
                        "y", &glm::vec4::y,
                        "z", &glm::vec4::z,
                        "w", &glm::vec4::w,
                        "r", &glm::vec4::r,
                        "g", &glm::vec4::g,
                        "b", &glm::vec4::b,
                        "a", &glm::vec4::a,
                        sol::meta_function::to_string,[] (const glm::vec4* vec) {return glm::to_string(*vec);},
                        sol::meta_function::addition,[] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)+(*vec_b);},
                        sol::meta_function::subtraction,[] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)-(*vec_b);},
                        sol::meta_function::multiplication,[] (const glm::vec4* vec,const float a) {return (*vec)*a;},
                        sol::meta_function::division,[] (const glm::vec4* vec,const float a) {return (*vec)/a;},
                        sol::meta_function::unary_minus,[] (const glm::vec4* vec) {return (*vec)*-1.f;},
                        sol::meta_function::equal_to,[] (const glm::vec4* vec_a,const  glm::vec4* vec_b) {return (*vec_a)==(*vec_b);}
                        );
            }

            // glm::mat4
            {
                auto glm_ns_table = m_state["glm"].get_or_create<sol::table>();
                glm_ns_table.new_usertype<glm::mat4>("mat4",sol::call_constructor,sol::constructors<glm::mat4(const float&)>(),
                        sol::meta_function::to_string,[] (const glm::mat4* m) {return glm::to_string(*m);},
                        sol::meta_function::addition,[] (const glm::mat4* m_a,const  glm::mat4* m_b) {return (*m_a)+(*m_b);},
                        sol::meta_function::subtraction,[] (const glm::mat4* m_a,const  glm::mat4* m_b) {return (*m_a)-(*m_b);},
                        sol::meta_function::multiplication,[] (const glm::mat4* m,const glm::vec4* v) {return (*m)*(*v);},
                        sol::meta_function::division,[] (const glm::mat4* m,const float a) {return (*m)/a;},
                        sol::meta_function::unary_minus,[] (const glm::mat4* m) {return (*m)*-1.f;},
                        sol::meta_function::equal_to,[] (const glm::mat4* m_a,const  glm::mat4* m_b) {return (*m_a)==(*m_b);}
                );
            }

            // glm functions
            {
                auto glm_ns_table = m_state["glm"].get_or_create<sol::table>();
                glm_ns_table.set_function("rotate",sol::overload([] (const glm::mat4* m,const float f,const glm::vec3* v) {return glm::rotate(*m,f,*v);}));
                glm_ns_table.set_function("radians",sol::overload([] (const float f) {return glm::radians(f);}));
                glm_ns_table.set_function("to_string",sol::overload(
                        [] (const glm::mat4* m) {return glm::to_string((*m));},
                        [] (const glm::vec3* v) {return glm::to_string((*v));}
                        ));
                glm_ns_table.set_function("eulerAngleYXZ",sol::overload(
                    [] (const float y,const float x,const float z) {
                        return glm::eulerAngleYXZ(y,x,z);
                        }));
                glm_ns_table.set_function("cross",sol::overload(
                    [] (const glm::vec3* v_a,const glm::vec3* v_b) {
                        return glm::cross(*v_a,*v_b);
                        }));
            }

            // debug
            if (debug_port > 0) {
                m_state.require("socket.core",luaopen_socket_core,true);
                m_state["debug_port"] = debug_port;
                m_state.script(R"(require("LuaPanda").start("127.0.0.1", debug_port))");
            }
        }     

        // load lua script file
        void loadEntryScript(const std::string &script) {
             try {
                if(m_state.script_file(script).valid()) {
                    auto global_app = m_state["global"]["app"];
                    onInit = global_app["onInit"];
                    beforeUpdate = global_app["beforeUpdate"];
                    onUpdate = global_app["onUpdate"];
                    afterUpdate = global_app["afterUpdate"];
                    onStop = global_app["onStop"];
                    onWindowResize = global_app["onWindowResize"];
                    app_name = global_app["name"];
                    app_width = global_app["width"];
                    app_height = global_app["height"];
                }
            } catch (sol::error &e) {
                std::cout << e.what() << std::endl;
            }
        }

        sol::table getTable(const std::string &name) {
            return m_state[name].get_or_create<sol::table>();;
        }


        std::string app_name;
        int app_width;
        int app_height;
        sol::function onInit = sol::nil;
        sol::function beforeUpdate = sol::nil;
        sol::function onUpdate = sol::nil;
        sol::function afterUpdate = sol::nil;
        sol::function onStop = sol::nil;
        sol::function onWindowResize = sol::nil;
    private:
        sol::state m_state;
    };
}