#ifndef BIDIRECTIONALPATH_H
#define BIDIRECTIONALPATH_H

#include <vector>
using std::vector;

#include <raytracing/intersection.h>

class BidirectionalPath
{
public:
    BidirectionalPath();

    //---vertices along path---
    vector< Intersection > vertices_eye;
    vector< Intersection > vertices_light;

    //---pdfs of vertices---
    vector< float > pdf_eye;
    vector< float > pdf_light;

    //--directions of vertices---
    vector< glm::vec3 > wiW_eye;
    vector< glm::vec3 > woW_eye;
    vector< glm::vec3 > wiW_light;
    vector< glm::vec3 > woW_light;

    //---BxDFs of vertices---
    vector< glm::vec3 > bxdf_eye;
    vector< glm::vec3 > bxdf_light;

    //---sample on light---
    Intersection isx_light;
};

#endif // BIDIRECTIONALPATH_H
