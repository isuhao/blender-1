/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "MinkowskiPenetrationDepthSolver.h"
#include "CollisionShapes/MinkowskiSumShape.h"
#include "NarrowPhaseCollision/SubSimplexConvexCast.h"
#include "NarrowPhaseCollision/VoronoiSimplexSolver.h"
#include "NarrowPhaseCollision/GjkPairDetector.h"


struct MyResult : public DiscreteCollisionDetectorInterface::Result
{

	MyResult():m_hasResult(false)
	{
	}
	
	SimdVector3 m_normalOnBInWorld;
	SimdVector3 m_pointInWorld;
	float m_depth;
	bool	m_hasResult;

	void AddContactPoint(const SimdVector3& normalOnBInWorld,const SimdVector3& pointInWorld,float depth)
	{
		m_normalOnBInWorld = normalOnBInWorld;
		m_pointInWorld = pointInWorld;
		m_depth = depth;
		m_hasResult = true;
	}
};



bool MinkowskiPenetrationDepthSolver::CalcPenDepth(SimplexSolverInterface& simplexSolver,
												   ConvexShape* convexA,ConvexShape* convexB,
												   const SimdTransform& transA,const SimdTransform& transB,
												   SimdVector3& v, SimdPoint3& pa, SimdPoint3& pb)
{


	//just take fixed number of orientation, and sample the penetration depth in that direction

	int N = 3;
	float minProj = 1e30f;
	SimdVector3 minNorm;
	SimdVector3 minVertex;
	SimdVector3 minA,minB;

	//not so good, lots of directions overlap, better to use gauss map
	for (int i=-N;i<N;i++)
	{
		for (int j = -N;j<N;j++)
		{
			for (int k=-N;k<N;k++)
			{
				if (i | j | k)
				{
					SimdVector3 norm(i,j,k);
					norm.normalize();

					{
						SimdVector3 seperatingAxisInA = (-norm)* transA.getBasis();
						SimdVector3 seperatingAxisInB = norm* transB.getBasis();

						SimdVector3 pInA = convexA->LocalGetSupportingVertex(seperatingAxisInA);
						SimdVector3 qInB = convexB->LocalGetSupportingVertex(seperatingAxisInB);
						SimdPoint3  pWorld = transA(pInA);	
						SimdPoint3  qWorld = transB(qInB);

						SimdVector3 w	= qWorld - pWorld;
						float delta = norm.dot(w);
						//find smallest delta

						if (delta < minProj)
						{
							minProj = delta;
							minNorm = norm;
							minA = pWorld;
							minB = qWorld;
						}
					}

					{
						SimdVector3 seperatingAxisInA = (norm)* transA.getBasis();
						SimdVector3 seperatingAxisInB = -norm* transB.getBasis();

						SimdVector3 pInA = convexA->LocalGetSupportingVertex(seperatingAxisInA);
						SimdVector3 qInB = convexB->LocalGetSupportingVertex(seperatingAxisInB);
						SimdPoint3  pWorld = transA(pInA);	
						SimdPoint3  qWorld = transB(qInB);

						SimdVector3 w	= qWorld - pWorld;
						float delta = (-norm).dot(w);
						//find smallest delta

						if (delta < minProj)
						{
							minProj = delta ;
							minNorm = -norm;
							minA = pWorld;
							minB = qWorld;
						}
					}



				}
			}
		}
	}

	SimdTransform ident;
	ident.setIdentity();

	GjkPairDetector gjkdet(convexA,convexB,&simplexSolver,0);


	v = minNorm * minProj;


	GjkPairDetector::ClosestPointInput input;
		
	SimdVector3 newOrg = transA.getOrigin() + v + v;

	SimdTransform displacedTrans = transA;
	displacedTrans.setOrigin(newOrg);

	input.m_transformA = displacedTrans;
	input.m_transformB = transB;
	input.m_maximumDistanceSquared = 1e30f;
	
	MyResult res;
	gjkdet.GetClosestPoints(input,res);

	if (res.m_hasResult)
	{
		pa = res.m_pointInWorld - res.m_normalOnBInWorld*0.1f*res.m_depth;
		pb = res.m_pointInWorld;
	}
	return res.m_hasResult;
}
