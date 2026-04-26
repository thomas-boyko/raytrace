#define _USE_MATH_DEFINES
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <glm/gtx/vector_query.hpp>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "imagebuffer.h"
#include "RayTrace.h"
#include "Scene.h"
#include "Lighting.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

const float EPSILON = 0.00001f;

int hasIntersection(Scene const &scene, Ray ray, int skipID){
	for (auto &shape : scene.shapesInScene) {
		Intersection tmp = shape->getIntersection(ray);
		if(
			shape->id != skipID
			&& tmp.numberOfIntersections!=0
			&& glm::distance(tmp.point, ray.origin) > 0.00001
			&& glm::distance(tmp.point, ray.origin) < glm::distance(ray.origin, scene.lightPosition) - 0.01
		){
			return tmp.id;
		}
	}
	return -1;
}

Intersection getClosestIntersection(Scene const &scene, Ray ray, int skipID){ //get the nearest
	Intersection closestIntersection;
	float min = std::numeric_limits<float>::max();
	for(auto &shape : scene.shapesInScene) {
		if(skipID == shape->id) {
			// Sometimes you need to skip certain shapes. Useful to
			// avoid self-intersection. ;)
			continue;
		}
		Intersection p = shape->getIntersection(ray);

		if(p.numberOfIntersections !=0){
            float distance = glm::distance(p.point, ray.origin);
            if(distance<min){
                min = distance;
                closestIntersection = p;
            }
		}
	}
	return closestIntersection;
}


glm::vec3 raytraceSingleRay(Scene const &scene, Ray const &ray, int level, int source_id) {
	// TODO: Part 3: Somewhere in this function you will need to add the code to determine
	//               if a given point is in shadow or not. Think carefully about what parts
	//               of the lighting equation should be used when a point is in shadow.
	// TODO: Part 4: Somewhere in this function you will need to add the code that does reflections and refractions.
	//               NOTE: The ObjectMaterial class already has a parameter to store the object's
	//               reflective properties. Use this parameter + the color coming back from the
	//               reflected array and the color from the phong shading equation.
	Intersection result = getClosestIntersection(scene, ray, source_id); //find intersection

	PhongReflection phong;
	phong.ray = ray;
	phong.scene = scene;
	phong.material = result.material;
	phong.intersection = result;

    Ray lightRay;
    lightRay.origin = result.point+ EPSILON * result.normal;
    lightRay.direction = scene.lightPosition - result.point;
    
    Intersection lightInt = getClosestIntersection(scene,lightRay,result.id);

    float lightDist = glm::length(scene.lightPosition - result.point);
    float hitDist = glm::length(lightInt.point - result.point);

    if (lightInt.numberOfIntersections > 0 && hitDist < lightDist-EPSILON && hitDist >EPSILON) { // shadow detection
        return phong.Is();
    }

	if (level < 1 || result.numberOfIntersections==0) {
		phong.material.reflectionStrength = glm::vec3(0); // black if no bounces specified
	} else {
        glm::vec3 q = glm::normalize(ray.direction); // point of intersection
        glm::vec3 n = glm::normalize(result.normal); // normal at intersection

        glm::vec3 r = q-2.f* (glm::dot(q,n))*n ; //direction of reflection

        Ray reflect;
        reflect.direction =r;
        reflect.origin = result.point;

        // transmitted ray for translucence?
        Ray transmit;
        PhongReflection phongTrans;

        return phong.I() + phong.Ks()*raytraceSingleRay(scene, reflect ,level-1, result.id);
        //phong.Ks() * phongRefl.I();
    }

	return phong.I();
}

struct RayAndPixel {
	Ray ray;
	int x;
	int y;
};

std::vector<RayAndPixel> getRaysForViewpoint(Scene const &scene, ImageBuffer &image, glm::vec3 viewPoint) {
	// This is the function you must implement for part 1
	//
	// This function is responsible for creating the rays that go
	// from the viewpoint out into the scene with the appropriate direction
	// and angles to produce a perspective image.
	int x = 0;
	int y = 0;
	std::vector<RayAndPixel> rays;

	// TODO: Part 1: Currently this is casting rays in an orthographic style.
	//               You need to change this code to project them in a pinhole camera style.
	for (float i = -1; x < image.Width(); x++) {
		y = 0;
		for (float j = -1; y < image.Height(); y++) {
			glm::vec3 direction(
                    i,
                    j,
                    -2.f
                );
			Ray r = Ray(viewPoint, direction);
			rays.push_back({r, x, y});
			j += 2.f / image.Height();
		}
		i += 2.f / image.Width();
	}
	return rays;
}

void raytraceImage(Scene const &scene, ImageBuffer &image, glm::vec3 viewPoint) {
	// Reset the image to the current size of the screen.
	image.Initialize();

	// Get the set of rays to cast for this given image / viewpoint
	std::vector<RayAndPixel> rays = getRaysForViewpoint(scene, image, viewPoint);


	// This loops processes each ray and stores the resulting pixel in the image.
	// final color into the image at the appropriate location.
	//
	// I've written it this way, because if you're keen on this, you can
	// try and parallelize this loop to ensure that your ray tracer makes use
	// of all of your CPU cores
	//
	// Note, if you do this, you will need to be careful about how you render
	// things below too
	// std::for_each(std::begin(rays), std::end(rays), [&] (auto const &r) {
	for (auto const & r : rays) {
		glm::vec3 color = raytraceSingleRay(scene, r.ray, 5, -1);
		image.SetPixel(r.x, r.y, color);
	}
}

// EXAMPLE CALLBACKS
class Assignment5 : public CallbackInterface {

public:
	Assignment5() {
		viewPoint = glm::vec3(0, 0, 0);
		scene = initScene1(0.f);
		raytraceImage(scene, outputImage, viewPoint);
	}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
			shouldQuit = true;
		}
	}

	bool shouldQuit = false;

	ImageBuffer outputImage;
	Scene scene;
	glm::vec3 viewPoint;

};
// END EXAMPLES


int main() {
	Log::debug("Starting main");
	glfwInit();
	int width = 1200;
	int height = 1200;
	Window window(width, height, "CPSC 453");
	GLDebug::enable();
	std::shared_ptr<Assignment5> a5 = std::make_shared<Assignment5>();
	window.setCallbacks(a5);

	float t = 0.0f;
    int i = 0;

	while (!window.shouldClose() && !a5->shouldQuit) {
		glfwPollEvents();
		glEnable(GL_FRAMEBUFFER_SRGB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		a5->scene = initScene1(t);  
        
        raytraceImage(a5->scene, a5->outputImage, a5->viewPoint);

		a5->outputImage.Render();
        a5->outputImage.SaveToFile("out/"+to_string(i)+".png");

        i++;
        t = (2.0f * M_PI * i) / 120.0f;  // 60 frames, should loop

		window.swapBuffers();
        if(i>241){a5->shouldQuit=true;}
	}
	glfwTerminate();
	return 0;
}
