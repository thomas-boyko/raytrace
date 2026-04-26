#include <iostream>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "RayTrace.h"


using namespace std;
using namespace glm;

Sphere::Sphere(vec3 c, float r, int ID){
	centre = c;
	radius = r;
	id = ID;
}

//------------------------------------------------------------------------------
// This is part 2.1 of your assignment. At the moment, the spheres are not showing
// up. Implement this method to make them show up.
//
// Make sure you set all of the appropriate fields in the Intersection object.
//------------------------------------------------------------------------------
Intersection Sphere::getIntersection(Ray ray){
	// You are required to implement this intersection.
	//
	// NOTE: You _must_ set these values appropriately for each case:
	//
	// No Intersection:
	// i.numberOfIntersections = 0;
	//
	// Intersection:
	// i.normal = **the normal at the point of intersection **
	// i.point = **the point of intersection**
	// i.numberOfIntersections = 1; // for a single intersection
	//
	// If you get fancy and implement things like refraction, you may actually
	// want to track more than one intersection. You'll need to change
	// The intersection struct in that case.
	const float EPSILON = 0.000001;

    glm::vec3 dir = ray.direction;
    glm::vec3 origin = ray.origin;

    //line from origin to centre of circle
    glm::vec3 oc = origin-centre;

    float a,b,c;
    a = dot(dir,dir);
    b = 2.f*glm::dot(oc,dir);
    c = glm::dot(oc,oc)- pow(radius,2);

    //discriminant
    float disc = b*b-4*a*c;

    Intersection p{};
    // if discriminant is nonnegative
    if (disc>0){
        // quadratic formula
        float t1 = (-b + glm::sqrt(pow(b,2)-4.f*a*c))/(2*a);
        float t2 = (-b - glm::sqrt(pow(b,2)-4.f*a*c))/(2*a);

        // TODO handle negative intersection ... for now everything is in front of camera so who cares
        float t = std::min(t1,t2);

		p.point = ray.origin + ray.direction * t;

		p.normal = glm::normalize(p.point-centre);
		p.material = material;
		p.numberOfIntersections = 1;
		p.id = id;
    } 
    return p;
}

//------------------------------------------------------------------------------
// This is part 2.2 of your assignment. At the moment, the cylinders are not showing
// up. Implement this method to make them show up.
//
// Make sure you set all of the appropriate fields in the Intersection object.
//------------------------------------------------------------------------------
Cylinder::Cylinder(vec3 c, float r, float h, int ID)
{
	centre = c;
	radius = r;
    height = h;

	id = ID;
}

Intersection Cylinder::getIntersection(Ray ray){
    glm::vec3 dir = ray.direction;
    glm::vec3 origin = ray.origin;
    glm::vec3 oc = origin - centre;

    Intersection p{};
    float bestT = 99999;

    // quadratic coefficient
    float a = dir.x*dir.x + dir.z*dir.z;
    float b = 2.f * (oc.x*dir.x + oc.z*dir.z);
    float c = oc.x*oc.x + oc.z*oc.z - radius*radius;

    //discriminant
    float disc = b*b - 4.f*a*c;

    if (disc > 0 ){
        //quadratic solutions
        float t1 = (-b - glm::sqrt(disc)) / (2.f*a); 
        float t2 = (-b + glm::sqrt(disc)) / (2.f*a);

        for (float t : {t1, t2}){
            if (t < 0) continue; // must be in front of ray

            glm::vec3 hitPoint = origin + dir * t;
            float localY = hitPoint.y - centre.y;

            if (localY >= -height/2.f && localY <= height/2.f){ //within the y-bounds of the cylinder
                if (t < bestT){
                    bestT = t;
                    p.point  = hitPoint;
                    p.normal = glm::normalize(glm::vec3(
                        hitPoint.x - centre.x,
                        0.f,
                        hitPoint.z - centre.z
                    ));
                    p.material = material;
                    p.numberOfIntersections = 1;
                    p.id = id;
                }
                break; 
            }
        }
    }

    // End cap, just the top cause the cameras are above the cylinders.
    float capY =  centre.y + height/2.f;

    if (glm::abs(dir.y) < 0) {
        return p;// ray parallel to cap plane
    } 

    float t = (capY - origin.y) / dir.y;

    if (t < 0) { // behind 
        return p;
    }

    glm::vec3 hitPoint = origin + dir * t;
    float dx = hitPoint.x - centre.x;
    float dz = hitPoint.z - centre.z;

    //are  we in the right xz region
    if (dx*dx + dz*dz <= radius*radius && t < bestT){
        bestT = t;
        p.point  = hitPoint;
        p.normal = glm::vec3(0.f, 1.f, 0.f);
        p.material = material;
        p.numberOfIntersections = 1;
        p.id = id;
    }
    return p;
}

Plane::Plane(vec3 p, vec3 n, int ID){
	point = p;
	normal = n;
	id = ID;
}


float dot_normalized(vec3 v1, vec3 v2){
	return glm::dot(glm::normalize(v1), glm::normalize(v2));
}

void debug(char* str, vec3 a){
	cout << "debug:" << str << ": " << a.x <<", " << a.y <<", " << a.z << endl;
}
// --------------------------------------------------------------------------
void Triangles::initTriangles(int num, vec3 * t, int ID){
	id = ID;
	for(int i = 0; i< num; i++){
		triangles.push_back(Triangle(*t, *(t+1), *(t+2)));
		t+=3;
	}
}

Intersection Triangles::intersectTriangle(Ray ray, Triangle triangle){
	// From https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
	const float EPSILON = 0.000001;
	auto vertex0 = triangle.p1;
	auto vertex1 = triangle.p2;
	auto vertex2 = triangle.p3;

	glm::vec3 edge1, edge2, h, s, q;
	float a,f,u,v;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;
	h = glm::cross(ray.direction, edge2);
	a = glm::dot(edge1, h);
	if (a > -EPSILON && a < EPSILON) {
		return Intersection{}; // no intersection
	}
	f = 1.0/a;
	s = ray.origin - vertex0;
	u = f * glm::dot(s, h);
	if (u < 0.0 || u > 1.0) {
		return Intersection{}; // no intersection
	}
	q = glm::cross(s, edge1);
	v = f * glm::dot(ray.direction, q);
	if (v < 0.0 || u + v > 1.0) {
		return Intersection{}; // no intersection
	}
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * glm::dot(edge2, q);
	// ray intersection
	if (t > EPSILON) {
		Intersection p;
		p.point = ray.origin + ray.direction * t;
		p.normal = glm::normalize(glm::cross(edge1, edge2));
		p.material = material;
		p.numberOfIntersections = 1;
		p.id = id;
		return p;
	} else {
		// This means that there is a line intersection but not a ray intersection.
		return Intersection{}; // no intersection
	}
}


Intersection Triangles::getIntersection(Ray ray){
	Intersection result{};
	result.material = material;
	result.id = id;
	float min = 9999;
	result = intersectTriangle(ray, triangles.at(0));
	if(result.numberOfIntersections!=0)min = glm::distance(result.point, ray.origin);
	for(int i = 1; i<triangles.size() ;i++){
		Intersection p = intersectTriangle(ray, triangles.at(i));
		if(p.numberOfIntersections !=0 && glm::distance(p.point, ray.origin) < min){
			min = glm::distance(p.point, ray.origin);
			result = p;
		}
	}

	result.material = material;
	result.id = id;
	return result;
}

Intersection Plane::getIntersection(Ray ray){
	Intersection result;
	result.material = material;
	result.id = id;
	result.normal = normal;
	if(dot(normal, ray.direction)>=0)return result;
	float s = dot(point - ray.origin, normal)/dot(ray.direction, normal);
	//if(s<0.00001)return result;
	result.numberOfIntersections = 1;
	result.point = ray.origin + s*ray.direction;
	return result;
}
