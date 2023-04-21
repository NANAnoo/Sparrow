//
// Created by duhao on 2023/3/29.
//

#ifndef SPARROW_PHYSICMATERIAL_H
#define SPARROW_PHYSICMATERIAL_H

namespace SPW{
    class PhysicMaterial {

    public:
        PhysicMaterial(float static_friction, float dynamic_friction, float restitution):
                static_friction_(static_friction),
                dynamic_friction_(dynamic_friction),
                restitution_(restitution)
        {
        }

        float static_friction() const{return static_friction_;}
        void set_static_friction(float static_friction){static_friction_ = static_friction;}

        float dynamic_friction() const{return dynamic_friction_;}
        void set_dynamic_friction(float dynamic_friction){dynamic_friction_ = dynamic_friction;}

        float restitution() const{return restitution_;}
        void set_restitution(float restitution){restitution_ = restitution;}

    private:
        float static_friction_;
        float dynamic_friction_;
        float restitution_;
    };
}



#endif //SPARROW_PHYSICMATERIAL_H
