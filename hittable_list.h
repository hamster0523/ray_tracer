#pragma once

#include "hittable.h"

#include <utility>
#include <memory>
#include <vector>

class hittable_list : public hittable
{
    public:
        std::vector<std::shared_ptr<hittable>> objects;

        hittable_list() {}

        hittable_list(const std::shared_ptr<hittable>& object)
        {
            add(object);
        }

        void clear() {objects.clear();}

        void add(const std::shared_ptr<hittable>& object)
        {
            objects.push_back(object);
            // When object added -> expand the AABB 
            bbox = merge_two_AABB(bbox, object->bounding_box());
        }

        bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& record) const override
        {
            hit_record temp_record;
            bool hit_anything = false;
            auto cloest_so_far = ray_tmax;

            for(const auto& object : objects)
            {
                if(object->hit(r, ray_tmin, cloest_so_far, temp_record))
                {
                    hit_anything = true;
                    cloest_so_far = temp_record.t;
                    // copy -> because record all pod(plain old data) copy is effiency no need to move
                    record = temp_record;
                }
            }

            return hit_anything;
        }

        bool hit(const ray& r, interval inter, hit_record& record) const override
        {
            hit_record temp_record;
            bool hit_anything = false;
            auto cloest_so_far = inter.max;

            for(const auto& object : objects)
            {
                if(object->hit(r, interval(inter.min, cloest_so_far), temp_record))
                {
                    hit_anything = true;
                    cloest_so_far = temp_record.t;
                    record = temp_record;
                }
            }

            return hit_anything;
        }

        AABB bounding_box() const override
        {
            return bbox;
        }

    private:
        AABB bbox;

};