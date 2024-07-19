#pragma once

#include "rt.h"

class hittable;
struct hit_record;

class camera
{
    public:
        // Public Camera Parameters Here
        float aspect_ratio = 16.0f / 9.0f;  // Ratio of (image_width / image_height)
        int    image_width = 400;           // Rendered image width in pixel count
        int   sample_pixel = 300;           // Count of random samples for each pixel
        int  max_recur_depth = 40;          // Maximum number of ray bounces into scene
        // Now we will FOV(field of view) to do decide the viewport height
        double vfov = 90.0f;                // Vertical FOV in degrees

        // Specific Parameters for Camera
        point3 lookfrom = point3(0, 0, 0);
        point3 lookat   = point3(0, 0, -1); // Near Plane
        vec3   vup      = vec3(0, 1, 0);    // Up direction

        // Parameters related to defocus blur
        double defocus_angle = 0.0;         // Variation angle of rays through each pixel
        double focus_dist    = 10 ;         // Distance from camera lookfrom to plane of perfect focus point

        // Render -> Get the Final Image
        void render(const hittable& world_list)
        {
            // initialize(); 

            // Test defocus_blur
            Initialize_with_defocus_blur();
            
            std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

            for(int j = 0; j < image_height; j ++)
            {
                std::clog <<"\rScanlines remaining: " << (image_height - j) << " " << std::flush;
                for(int i = 0; i < image_width; i ++)
                {
                    // auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                    // // ray from pixel_center point tp camera_center
                    // auto ray_direction = (pixel_center - camera_center);
                    // ray_direction = normalize(ray_direction);
                    // ray r(pixel_center, ray_direction);

                    // color pixel_color = ray_color(r);

                    // color pixel_color = ray_color(r, world_list);
                    // MSAA
                    color pixel_color(0.0f);
                    for(int sample = 0; sample < sample_pixel; sample ++)
                    {
                        ray r = get_ray(i, j);
                        pixel_color += ray_color(r, world_list, max_recur_depth);
                    }
                    pixel_color *= pixel_samples_division;
                    write_color(std::cout, pixel_color);
                }
            }

            std::clog << "\rDone.              \n";
            
        }

    private:
        int image_height;               // Rendered image height
        point3 camera_center;           // Camera Center
        point3 pixel00_loc;             // Location of pixel (0, 0) not (0, 0) center
        vec3 pixel_delta_u;             // Offset to pixel to the right
        vec3 pixel_delta_v;             // Offset to pixel to the below
        double pixel_samples_division;  // Color Scale factoe for a sum of piexl samples
        vec3 u, v, w;                   // Camera Frame basis vectors
        vec3 defocus_disk_u;            // Defocus disk horizontal radius
        vec3 defocus_disk_v;            // Defocus disk vertical radius

    private:
        
        // Initialize the Camera
        // Which Version No defocus Blur
        void initialize()
        {
            image_height = static_cast<int>(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            pixel_samples_division = 1.0 / sample_pixel;

            // --------------------------
            // Now the lookfrom is the center of camera
            // camera_center = point3(0, 0, 0);
            // --------------------------
            camera_center = lookfrom;

            // --------------------------
            // Detemine viewport dimensions
            // auto focal_length = 1.0f;
            // --------------------------     
            auto focal_length = (lookfrom - lookat).length();      
            
            // --------------------------
            // Now we will FOV(field of view) to do decide the viewport height
            // auto viewport_height = 2.0f;
            // --------------------------

            // Calculate the camera frame basis vectors
            w = normalize(lookfrom - lookat);
            u = normalize(cross(vup, w));
            v = cross(w, u);

            auto theta = degree_to_radians(vfov);
            // tan(theta / 2) = h / focal_length
            // h = tan(theta / 2) * focal_length
            // viewport_height = 2.0f * tan(theta / 2) * focal_length
            auto h = std::tan(theta / 2.0f);
            auto viewport_height = 2.0f * h * focal_length;
            auto viewport_width = viewport_height * (static_cast<double>(image_width) / static_cast<double>(image_height));

            // --------------------------
            // // Calculate the vectos across the horizontal and down the vertical viewport
            // auto viewport_u = vec3(viewport_width, 0, 0);
            // auto viewport_v = vec3(0, - viewport_height, 0);
            // --------------------------
            // After we have the camera frame basis vectors, we can calculate viewport_u, viewport_v
            auto viewport_u = viewport_width * u;
            auto viewport_v = - viewport_height * v;


            // Calculate the delta vector across the horizontal and down vertical
            // Here is member variable, No longer is temporary variable
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // --------------------------
            // // // Calculate the pixel00_loc coord
            // // auto viewport_upper_left = 
            // //     (camera_center - vec3(0, 0, focal_length)) - (viewport_u  + viewport_v) / 2;
            // --------------------------
            // After we have the camera frame basis vectors(exclusive the w vector)
            auto viewport_upper_left = (camera_center - focal_length * w) - (viewport_u + viewport_v) / 2;

            // (0, 0) pixel center location coord
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
        }

        void Initialize_with_defocus_blur() noexcept
        {
            image_height = static_cast<int>(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            pixel_samples_division = 1.0 / sample_pixel;

            camera_center = lookfrom;

            // Determine viewport dimensions
            auto theta = degree_to_radians(vfov);
            auto h     = tan(theta / 2);
            auto viewport_height = 2 * h * focus_dist;
            auto viewport_width  = viewport_height * (static_cast<double>(image_width) / static_cast<double>(image_height));

            // Calculate the u,v,w unit bias vectors for the camera coordinate frame
            w = normalize(lookfrom - lookat);
            u = cross(vup, w);
            v = cross(w, u);

            // Calculate the vectors across the horizontal and down the viewport viewport edges
            vec3 viewport_u =   viewport_width * u;
            vec3 viewport_v = - viewport_height * v;

            // Calculate the horizontal and vertical delta vectors to the next pixel
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // Calculate the localtion of upper left pixel
            auto viewport_upper_left = (camera_center - (focus_dist * w)) - (viewport_u + viewport_v) / 2;
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            // Calculate the camera defocus disk basis vectors
            auto defocus_radius = focus_dist * std::tan(degree_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;

        }

        // Get the single ray color
        color ray_color(const ray& r, const hittable& world_list) const
        {
            hit_record record;

            if(world_list.hit(r, interval(0.000001, infinity), record))
            {
               // return 0.5 * (record.normal + color(1.0, 1.0, 1.0));
               // get a same face (normal and ramdom vector)
               vec3 direction = random_on_hemisphere(record.normal);
               // Assume the hit point abosord 50% radiance
               color recur_color = ray_color(ray(record.p, direction), world_list);
               return 0.5 * recur_color;
            }

            // BackGround Color
            vec3 unit_direction = normalize(r.direction());
            auto a = 0.5f * (unit_direction.y() + 1.0f);
            color white(1.0f, 1.0f, 1.0f);
            color blue(0.5f, 0.7f, 1.0f);
            return lerp(blue, white, a);
        }

        color ray_color(const ray& r, const hittable& world_list, int cur_depth) const
        {
            if(cur_depth < 0) return color(0.0f, 0.0f, 0.0f);

            hit_record record;
            if(world_list.hit(r, interval(0.001, infinity), record))
            {
                // // vec3 direction = random_on_hemisphere(record.normal);
                // // Is Lambertian ?
                // vec3 direction = normalize(record.normal) + record.p + random_unit_vector();
                // color recur_color = ray_color(ray(record.p, direction), world_list, cur_depth - 1);
                // ------------------
                // depend on the hit point's material to get the ray color
                ray scattered; // Scattered ray
                vec3 attenuation; // out light radiance / in light radiance
                if(record.mat->scatter(r, record, attenuation, scattered))
                {
                    return attenuation * ray_color(scattered, world_list, cur_depth - 1);
                }
                return color(0, 0, 0);
            }

            // BackGround Color
            vec3 unit_direction = normalize(r.direction());
            auto a = 0.5f * (unit_direction.y() + 1.0f);
            color white(1.0f);
            color blue(0.5f, 0.7f, 1.0f);
            return lerp(blue, white, a);
        }

        ray get_ray(int i, int j) const noexcept
        {
            // auto offset = sample_square();
            // auto center_pixel_loc = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            // auto sample_pixel = center_pixel_loc + offset.x() * pixel_delta_u + offset.y() * pixel_delta_v;

            // auto ray_origin = camera_center;
            // auto ray_direction = sample_pixel - ray_origin;

            // return ray(ray_origin, ray_direction);

                    // Construct a camera ray originating from the origin and directed at randomly sampled
        // point around the pixel location i, j.

            auto offset = sample_square();
            auto pixel_sample = pixel00_loc
                            + ((i + offset.x()) * pixel_delta_u)
                            + ((j + offset.y()) * pixel_delta_v);

            // auto ray_origin = camera_center;
            auto ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;

            // Motion-Blur -> Get a ray random time in the range [0, 1]
            auto ray_time = random_double();

            return ray(ray_origin, ray_direction, ray_time);
        }

        vec3 sample_square() const noexcept
        {
            // Return the vector to a random point center in [-0.5, 0.5] - [-0.5, 0.5] square
            return vec3(random_double() - 0.5f, random_double() - 0.5f, 0.0f);
        }

        point3 defocus_disk_sample() const
        {
            auto p = random_in_unit_disk();
            return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }



};