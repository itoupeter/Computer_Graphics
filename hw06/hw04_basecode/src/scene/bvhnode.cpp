
#include "bvhnode.h"

BVHNode::BVHNode():
    pLeft( NULL ),
    pRight( NULL ),
    pBBox( NULL ),
    pGeometry( NULL ){
}

Intersection BVHNode::getIntersection( const Ray &r ){

    //---leaf node---
    if( pLeft == NULL && pRight == NULL ){
        return pGeometry->GetIntersection( r );
    }

    //---no intersection---
    if( !pBBox->getIntersection( r ) ) return Intersection();

    //---may have intersetion---
    Intersection lResult, rResult;

    if( pLeft != NULL ) lResult = pLeft->getIntersection( r );
    if( pRight != NULL ) rResult = pRight->getIntersection( r );

    if( lResult.object_hit == NULL ){
        return rResult;
    }else if( rResult.object_hit == NULL ){
        return lResult;
    }else if( lResult.t < rResult.t ){
        return lResult;
    }else{
        return rResult;
    }

}
