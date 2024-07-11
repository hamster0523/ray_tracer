#include <iostream>
#include "rt.h"
#include "sphere.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"

color ray_color(const ray& r)
{
    // sphere sphe(0.5, point3(0, 0, 1));
    // if(sphe.hit_sphere(r))
    //     return color(1, 0, 0);
    // // return color(0, 0, 0);
    // // transform from [-1, 1] to [0, 1]
    // double t = (r.direction().y() + 1.0f) / 2;
    // vec3 white(1.0f);
    // vec3 blue(0.5, 0.7, 1.0);
    // return (1.0 - t) * white + t * blue;

    sphere sphe(0.5, point3(0, 0, -1));
    auto t = sphe.hit_sphere_return_hit_point(r);
    // printf("t is : %f\n", t);
    std::clog << "now the t is : " << t << std::endl << std::flush;
    if(t != -100.0)
    {
        vec3 N = normalize(r.at(t) - sphe.center());
        return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
    }

    vec3 unit_r_direction = normalize(r.direction());
    t = (unit_r_direction.y() + 1.0f) * 0.5f;
    vec3 white(1.0f);
    vec3 blue(0.5, 0.7, 1.0);
    return (1.0f - t) * white + t * blue;
}

color ray_color(const ray& r, const hittable& world)
{
    hit_record record;
    if(world.hit(r, interval(0, infinity), record))
    {
        return 0.5 * (record.normal + vec3(1.0f, 1.0f, 1.0f));
    }

    vec3 unit_vector = normalize(r.direction());
    auto t = (unit_vector.y() + 1.0f) * 0.5f;
    vec3 white(1.0f);
    vec3 blue(0.5, 0.7, 1.0);
    return (1.0f - t) * white + t * blue;
}

bool hit_sphere(const sphere& sphe, const ray& r)
{
    auto center = sphe.center();
    auto radius = sphe.radius();
    vec3 oc = center - r.origin();
    auto a  = dot(r.direction(), r.direction());
    auto b  = -2 * dot(r.direction(), oc);
    auto c  = dot(oc, oc) - radius * radius;
    auto discrim = b * b - 4 * a * c;
    return discrim >= 0;
}

double hit_sphere_return_hit_point(const sphere& sphere, const ray& r)
{
    auto center = sphere.center();
    auto radius = sphere.radius();

    vec3 oc = center - r.origin();
    auto a  = dot(r.direction(), r.direction());
    auto b  = -2 * dot(r.direction(), oc);
    auto h  = dot(r.direction(), oc);
    auto c  = dot(oc, oc) - radius * radius;
    // auto discrim = b * b - 4 * a * c;
    auto discrim = h * h - a * c;

    if(discrim < 0)
    {
        // the hit point is behind the camera
        return -100.0f;
    }
    else 
    {
        // else there is a hit point - return the nearest hit point
        // return (- b - std::sqrt(discrim)) / (2.0f * a);
        return (h - std::sqrt(discrim)) / a;
    }
}

int main()
{
    // // int image_width = 256;
    // // int image_height = 256;

    // // std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    // // for(int j = 0; j < image_height; j++)
    // // {
    // //     std::clog << "\rScanlines remaining: " << (image_height - j) << " " << std::flush;
    // //     for(int i = 0; i < image_width; i ++)
    // //     {
    // //         auto r = double(i) / (image_width - 1);
    // //         auto g = double(j) / (image_height - 1);
    // //         auto b = 0.0;

    // //         auto pixel_color = color(r, g, b);

    // //         write_color(std::cout, pixel_color);
    // //     }
    // // }
    // // std::clog << "\rDone.            \n";

    // auto aspect_ratio = 16.0f / 9.0f;
    // int image_width = 400;

    // // Calculate the image height and ensure it's at least is 1
    // int image_height = int(image_width / aspect_ratio);
    // image_height = (image_height < 1) ? 1 : image_height;

    // // Add object in world hittable_list
    // hittable_list world_list;
    // world_list.add(std::make_shared<sphere>(0.6f, point3(0, 0, -1)));
    // world_list.add(std::make_shared<sphere>(100, point3(0, -100.999, -1)));

    // // Camera Parameters
    // auto focal_length = 1.0f;
    // auto viewport_height = 2.0f;
    // auto viewport_width = viewport_height * (static_cast<double>(image_width) / static_cast<double>(image_height));
    // auto camera_center = point3(0, 0, 0);

    // // Calculate the vectors across the horizontal and down the vertical viewport edges
    // auto viewport_u = vec3(viewport_width, 0, 0);
    // auto viewport_v = vec3(0, -viewport_height, 0);

    // // Calculate the horizontal and vertical delta vectors from pixel to pixel
    // auto pixel_delta_u = viewport_u / image_width;
    // auto pixel_delta_v = viewport_v / image_height;

    // // Calculate the location of the upper left pixel
    // /*
    // the upper_left_location calculate : 
    // - first calculate the vector_1 : camera_center - vec3(0, 0, focal_length) 
    //   - the vector start from viewport_plane center to the camera_pos
    // - second calculate the vector_2 : viewport_u / 2 + viewport_v / 2
    //   - the vector start from plane upper_left(not coord) to plane center
    // - AB - BC = AB + (-BC) = AB + CB = AC
    // - first_vec - second_vec = pixel00_loc really coord - pixel 00 's center
    // */
    // auto viewport_upper_left = camera_center - 
    //     vec3(0, 0, focal_length) - (viewport_u / 2 + viewport_v / 2);
    // auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // // render

    // std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    // for(int j = 0; j < image_height; j ++)
    // {
    //     std::clog <<"\rScanlines remaining: " << (image_height - j) << " " << std::flush;
    //     for(int i = 0; i < image_width; i ++)
    //     {
    //         auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
    //         // ray from pixel_center point tp camera_center
    //         auto ray_direction = (pixel_center - camera_center);
    //         ray_direction = normalize(ray_direction);
    //         ray r(pixel_center, ray_direction);

    //         // color pixel_color = ray_color(r);

    //         color pixel_color = ray_color(r, world_list);
    //         write_color(std::cout, pixel_color);
    //     }

    // std::clog << "\rDone.              \n";

    // // -----------------------------------------------------
    // All Wrap in Camera Class
    hittable_list world;

    std::vector<point3> center = {
        point3( 0.0, -100.999, -1.0),
        point3( 0.0,      0.0, -1.2),
        point3(-1.0,      0.0, -1.0),
        point3( 1.0,      0.0, -1.0),
    };

    std::vector<double> r = {
        100.0f,
        0.5f,
        0.5f,
        0.5f
    };

    std::vector<std::shared_ptr<material>> mat = {
        std::make_shared<lambertian>(albedo(0.8, 0.8, 0.0)),
        std::make_shared<lambertian>(albedo(0.1, 0.2, 0.5)),
        std::make_shared<metal>(albedo(0.8, 0.8, 0.8), 1.0),
        std::make_shared<metal>(albedo(0.8, 0.6, 0.2), 0.4),
    };

    auto sphere_num = center.size();

    for(int i = 0; i < sphere_num; i ++)
    {
        world.add(std::make_shared<sphere>(r[i], center[i], mat[i]));
    }

    camera cam;
    cam.aspect_ratio = 16.0f / 9.0f;
    cam.image_width = 400;
    cam.max_recur_depth = 50;

    cam.render(world);

    return 0;
}