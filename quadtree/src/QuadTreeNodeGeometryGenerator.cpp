//
// Created by moose on 05/04/19.
//

#include "quadtree/QuadTreeNodeGeometryGenerator.h"
#include "quadtree/QuadTreeNodeGeometry.h"

#include "util/LogUtils.h"
#define TAG "QuadTreeNodeGeometryGenerator"

bool QuadTreeNodeGeometryGenerator::operator== ( const Qt3DRender::QBufferDataGenerator& other ) const
{
    const auto *otherFunctor = functor_cast<QuadTreeNodeGeometryGenerator> ( &other );
    if ( otherFunctor != nullptr ) {
        bool match = * ( otherFunctor->mGeometry ) == *mGeometry;

        log_debug ( "match=%s", match );

        return match;
    }

    return false;
}


QByteArray QuadTreeNodeGeometryGenerator::operator() ()
{
    log_trace_enter();
    const QByteArray &ba = mGeometry->generateVertexData();
    log_trace_exit();

    return ba;
}

