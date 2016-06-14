// Copyright (C) 2009, 2010 by Florent Lamiraux, Thomas Moulard, JRL.
//
// This file is part of the hpp-corbaserver.
//
// This software is provided "as is" without warranty of any kind,
// either expressed or implied, including but not limited to the
// implied warranties of fitness for a particular purpose.
//
// See the COPYING file for more information.

#include <hpp/fcl/math/transform.h>
#include "tools.hh"

using hpp::corbaServer::size_type;
using CORBA::ULong;

void
hppTransformToTransform3f
(const CORBA::Double* inConfig, hpp::corbaServer::Transform3f& transform)
{
  fcl::Quaternion3f Q (inConfig [3], inConfig [4],
		       inConfig [5], inConfig [6]);
  fcl::Vec3f T (inConfig [0], inConfig [1], inConfig [2]);
  transform.setTransform (Q, T);
}

void
Transform3fTohppTransform (const hpp::corbaServer::Transform3f& transform,
			   CORBA::Double* config)
{
  fcl::Quaternion3f Q = transform.getQuatRotation ();
  fcl::Vec3f T = transform.getTranslation ();
  /*
  for(int i=0; i<3; i++) {
    for(int j=0; j<3; j++)
      config.rot[i*3+j] = R (i,j);
  }
  */
  for(int i=0; i<3; i++)
    config [i] = T [i];
  config[3] = Q.w();
  config[4] = Q.x();
  config[5] = Q.y();
  config[6] = Q.z();
}

hpp::floatSeq* vectorToFloatseq (const hpp::core::vector_t& input)
{
  CORBA::ULong size = (CORBA::ULong) input.size ();
  hpp::floatSeq* q_ptr = new hpp::floatSeq ();
  q_ptr->length (size);

  for (std::size_t i=0; i<size; ++i) {
    (*q_ptr) [(CORBA::ULong)i] = input [i];
  }
  return q_ptr;
}

hpp::floatSeqSeq* matrixToFloatSeqSeq (const hpp::core::matrix_t& input)
{
  hpp::floatSeqSeq* res = new hpp::floatSeqSeq;
  res->length ((ULong) input.rows ());
  for (size_type i=0; i<input.rows (); ++i) {
    hpp::floatSeq row; row.length ((ULong) input.cols ());
    for (size_type j=0; j<input.cols (); ++j) {
      row [(ULong) j] = input (i, j);
    }
    (*res) [(ULong) i] = row;
  }
  return res;
}
