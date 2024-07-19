#pragma once

#include "rt.h"

class bvh_node : public hittable
{
    private:
        std::shared_ptr<hittable> left;     // Left  Child
        std::shared_ptr<hittable> right;    // Right Child
        AABB bbox;                          // Node  AABB

        // Strategy Pattern
        // Depend on specific axis to sort the hittable object
        static bool box_compare(const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b,
                                std::size_t axis_index)
        {
            auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
            auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
            // In axis_index : small to big
            return a_axis_interval.min < b_axis_interval.min;
        }

        static bool box_compare_x(const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b)
        {
            return box_compare(a, b, 0);
        }

        static bool box_compare_y(const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b)
        {
            return box_compare(a, b, 1);
        }

        static bool box_compare_z(const std::shared_ptr<hittable>& a, const std::shared_ptr<hittable>& b)
        {
            return box_compare(a, b, 2);
        }

        int Get_Max_Axis(const std::vector<std::shared_ptr<hittable>>& cur_obj_vec)
        {
            auto interval_axis_x_merged = cur_obj_vec[0]->bounding_box().x_interval;
            auto interval_axis_y_merged = cur_obj_vec[0]->bounding_box().y_interval;
            auto interval_axis_z_merged = cur_obj_vec[0]->bounding_box().z_interval;

            for(const auto& obj : cur_obj_vec)
            {
                interval_axis_x_merged = two_interval_merge(interval_axis_x_merged, obj->bounding_box().x_interval);
                interval_axis_y_merged = two_interval_merge(interval_axis_y_merged, obj->bounding_box().y_interval);
                interval_axis_z_merged = two_interval_merge(interval_axis_z_merged, obj->bounding_box().z_interval);
            }

            auto x_axis_len = interval_axis_x_merged.size();
            auto y_aixs_len = interval_axis_y_merged.size();
            auto z_axis_len = interval_axis_z_merged.size();

            if(x_axis_len > y_aixs_len && x_axis_len > z_axis_len)
                return 0;
            if(y_aixs_len > x_axis_len && y_aixs_len > z_axis_len)
                return 1;
            if(z_axis_len > x_axis_len && z_axis_len > y_aixs_len)
                return 2;
            // None reached
            __builtin_unreachable();
        }

        int Max_Axis(const std::vector<std::shared_ptr<hittable>>& objects, std::size_t start_index, std::size_t end_index)
        {
            auto bbox = AABB::empty;
            for(std::size_t object_index = start_index; object_index < end_index; object_index ++)
            {
                bbox = merge_two_AABB(bbox, objects[object_index]->bounding_box());
            }

            return bbox.longest_axis();
        }

    public:
        bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size())
        {
            // Note: objects No reference
            //       Just Copy the objects
            // The lifetime of the copied list only extends until this construct complete
        }    

        bvh_node(std::vector<std::shared_ptr<hittable>>& objects, std::size_t start_index, std::size_t end_index)
        {
            // // Random Axis Split
            // // int axis = random_int(0, 2);
            // auto axis = Get_Max_Axis(std::vector<std::shared_ptr<hittable>>{objects[start_index], objects[end_index - 1]});
            
            // std::function<bool(
            //     const std::shared_ptr<hittable>&, const std::shared_ptr<hittable>&
            // )> comparator  =  (axis == 0) ? box_compare_x :
            //                   (axis == 1) ? box_compare_y :
            //                                 box_compare_z ;

            // std::size_t object_span = end_index - start_index;

            // if(object_span == 1)
            // {
            //     left = right = objects[start_index];
            // }else if(object_span == 2)
            // {
            //     left = objects[start_index];
            //     right = objects[start_index + 1];
            // }else
            // {
            //     std::sort(objects.begin(), objects.end(), comparator);

            //     // Balance Split
            //     auto mid = start_index + object_span / 2;

            //     left = std::make_shared<bvh_node>(objects, start_index, mid);
            //     right = std::make_shared<bvh_node>(objects, mid, end_index);
            // }

            // bbox = merge_two_AABB(left->bounding_box(), right->bounding_box());

            SAH_Build(objects, start_index, end_index);
        }

        void SAH_Build(std::vector<std::shared_ptr<hittable>>& objects, std::size_t start_index, std::size_t end_index) 
        {
            auto max_axis = Get_Max_Axis(std::vector<std::shared_ptr<hittable>>{objects[start_index], objects[end_index - 1]});

            std::function<bool(
                const std::shared_ptr<hittable>&, const std::shared_ptr<hittable>&
            )> comparator = (max_axis == 0) ? box_compare_x :
                            (max_axis == 1) ? box_compare_y :
                                              box_compare_z ;

            int object_span = end_index - start_index;

            if(object_span == 1)
            {
                left  = objects[start_index];
                right = objects[start_index];
            }else if(object_span == 2)
            {
                left = objects[start_index];
                right = objects[start_index + 1];
            }else
            {
                // Use SAH To Build BVH
                interval axis_extend = interval::empty;
                double sum_area = 0.0f;
                for(std::size_t index = start_index; index < end_index; index ++)
                {
                    axis_extend = two_interval_merge(axis_extend, objects[index]->bounding_box().axis_interval(max_axis));
                    sum_area += objects[index]->bounding_box().area();
                }

                auto avg_split_len = axis_extend.size() / object_span;
                std::vector<std::pair<std::shared_ptr<hittable>, int>> bucket;

                for(std::size_t index = start_index; index < end_index; index ++)
                {
                    auto this_aabb_interval = objects[index]->bounding_box().axis_interval(max_axis);
                    int a = static_cast<int>((this_aabb_interval.interval_center() - axis_extend.min) / axis_extend.size() * object_span);
                    // Locate in (0, object_span - 1) some sub-bucket
                    int locate = clamp(a, 0, object_span - 1);
                    bucket.emplace_back(std::pair<std::shared_ptr<hittable>, int>(objects[index], locate));
                }

                std::size_t best_split = start_index + 1;
                auto cost = std::numeric_limits<double>::infinity();
                for(std::size_t mid = start_index + 1; mid < end_index - 1; mid ++)
                {
                    double left_area = 0.0f;
                    double right_area = 0.0f;
                    int left_count = 0;
                    int right_count = 0;
                    for(const auto& item : bucket)
                    {
                        // bucket[index].second \in {0, obejct_span - 1}
                        if(item.second * avg_split_len  + start_index < mid)
                        {
                            left_area += item.first->bounding_box().area();
                            left_count ++;
                        }
                        else
                        {
                            right_area += item.first->bounding_box().area();
                            right_count ++;
                        }
                    }

                    auto this_cost = left_count * (left_area / sum_area) + right_count * (right_area / sum_area);
                    if(this_cost < cost)
                    {
                        best_split = mid;
                        cost = this_cost;
                    }
                }
                // Good ! Save 37% time    
                // std::clog << "\n" << cost << " " << start_index << " " << end_index << " " << "this best split is :" << best_split << "\n";
                left = std::make_shared<bvh_node>(objects, start_index, best_split);
                right = std::make_shared<bvh_node>(objects, best_split, end_index);
            }

            bbox = merge_two_AABB(left->bounding_box(), right->bounding_box());
        }

        void SAH_Build_2(std::vector<std::shared_ptr<hittable>>& objects, std::size_t start_index, std::size_t end_index) 
        {
            auto max_axis = Get_Max_Axis(std::vector<std::shared_ptr<hittable>>{objects[start_index], objects[end_index - 1]});

            std::function<bool(
                const std::shared_ptr<hittable>&, const std::shared_ptr<hittable>&
            )> comparator = (max_axis == 0) ? box_compare_x :
                            (max_axis == 1) ? box_compare_y :
                                            box_compare_z ;

            int object_span = end_index - start_index;

            if(object_span == 1)
            {
                left  = objects[start_index];
                right = objects[start_index];
            }
            else if(object_span == 2)
            {
                left = objects[start_index];
                right = objects[start_index + 1];
            }
            else
            {
                // Use SAH To Build BVH
                interval axis_extend = interval::empty;
                double sum_area = 0.0;
                for(std::size_t index = start_index; index < end_index; index ++)
                {
                    axis_extend = two_interval_merge(axis_extend, objects[index]->bounding_box().axis_interval(max_axis));
                    sum_area += objects[index]->bounding_box().area();
                }

                auto avg_split_len = axis_extend.size() / object_span;
                std::vector<std::pair<std::shared_ptr<hittable>, int>> bucket;

                for(std::size_t index = start_index; index < end_index; index ++)
                {
                    auto this_aabb_interval = objects[index]->bounding_box().axis_interval(max_axis);
                    int a = static_cast<int>((this_aabb_interval.interval_center() - axis_extend.min) / axis_extend.size() * object_span);
                    // Locate in (0, object_span - 1) some sub-bucket
                    int locate = clamp(a, 0, object_span - 1);
                    bucket.emplace_back(std::pair<std::shared_ptr<hittable>, int>(objects[index], locate));
                }

                std::size_t best_split = start_index + 1;
                auto cost = std::numeric_limits<double>::infinity();
                for(std::size_t mid = start_index + 1; mid < end_index; mid ++)
                {
                    double left_area = 0.0;
                    double right_area = 0.0;
                    int left_count = 0;
                    int right_count = 0;
                    for(const auto& item : bucket)
                    {
                        if(item.second * avg_split_len < mid - start_index)
                        {
                            left_area += item.first->bounding_box().area();
                            left_count ++;
                        }
                        else
                        {
                            right_area += item.first->bounding_box().area();
                            right_count ++;
                        }
                    }

                    auto this_cost = (left_count * (left_area / sum_area)) + (right_count * (right_area / sum_area));
                    if(this_cost < cost)
                    {
                        best_split = mid;
                        cost = this_cost;
                    }
                }

                left  = std::make_shared<bvh_node>(objects, start_index, best_split);
                right = std::make_shared<bvh_node>(objects, best_split, end_index);
            }

            bbox = merge_two_AABB(left->bounding_box(), right->bounding_box());
        }
        
        AABB bounding_box() const override
        {
            return bbox;
        }

        bool hit(const ray& r_in , double ray_tmin, double ray_tmax, hit_record& record) const override
        {
            // Pass
            // No Use
            __builtin_unreachable();
        }

        bool hit(const ray& r_in, interval inter, hit_record& record) const override
        {
            if(this->bbox.hit(r_in, inter) == false)
            {
                // No Hit with the Node AABB -> No Possible hit with left and right child
                return false;
            }

            // Hit !
            bool hit_left = left->hit(r_in, inter, record);
            bool hit_right = right->hit(r_in, interval(inter.min, hit_left ? record.t : inter.max) , record);

            return hit_left || hit_right;
        }

};