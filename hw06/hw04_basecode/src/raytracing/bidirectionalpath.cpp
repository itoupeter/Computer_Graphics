
#include <raytracing/bidirectionalpath.h>

BidirectionalPath::BidirectionalPath(){

    vertices_eye.clear();
    vertices_light.clear();

    pdf_eye.clear();
    pdf_light.clear();

    wiW_eye.clear();
    woW_eye.clear();
    wiW_light.clear();
    woW_light.clear();

    bxdf_eye.clear();
    bxdf_light.clear();
}
