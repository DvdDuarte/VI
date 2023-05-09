//
//  primitive.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef primitive_hpp
#define primitive_hpp

#include "Geometry/geometry.hpp"
#include "BRDF/BRDF.hpp"
#include <memory>

struct Primitive {
    Geometry *g;
    std::shared_ptr<BRDF> material;
};

#endif /* primitive_hpp */
