///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a handy indexed triangle class.
 *	\file		IceIndexedTriangle.h
 *	\author		Pierre Terdiman
 *	\date		January, 17, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEINDEXEDTRIANGLE_H__
#define __ICEINDEXEDTRIANGLE_H__

	// Forward declarations
	enum CubeIndex;

	// An indexed triangle class.
	class ICEMATHS_API IndexedTriangle
	{
		public:
		//! Constructor
		inline_					IndexedTriangle()									{}
		//! Constructor
		inline_					IndexedTriangle(udword r0, udword r1, udword r2)	{ mVRef[0]=r0; mVRef[1]=r1; mVRef[2]=r2; }
		//! Copy constructor
		inline_					IndexedTriangle(const IndexedTriangle& triangle)
								{
									mVRef[0] = triangle.mVRef[0];
									mVRef[1] = triangle.mVRef[1];
									mVRef[2] = triangle.mVRef[2];
								}
		//! Destructor
		inline_					~IndexedTriangle()									{}
		//! Vertex-references
				udword			mVRef[3];

		// Methods
				void			Flip();
				float			Area(const OPC_Point* verts)											const;
				float			Perimeter(const OPC_Point* verts)										const;
				float			Compacity(const OPC_Point* verts)										const;
				void			Normal(const OPC_Point* verts, OPC_Point& normal)							const;
				void			DenormalizedNormal(const OPC_Point* verts, OPC_Point& normal)				const;
				void			Center(const OPC_Point* verts, OPC_Point& center)							const;
				void			CenteredNormal(const OPC_Point* verts, OPC_Point& normal)					const;
				void			RandomPoint(const OPC_Point* verts, OPC_Point& random)						const;
				bool			IsVisible(const OPC_Point* verts, const OPC_Point& source)					const;
				bool			BackfaceCulling(const OPC_Point* verts, const OPC_Point& source)			const;
				float			ComputeOcclusionPotential(const OPC_Point* verts, const OPC_Point& view)	const;
				bool			ReplaceVertex(udword oldref, udword newref);
				bool			IsDegenerate()														const;
				bool			HasVertex(udword ref)												const;
				bool			HasVertex(udword ref, udword* index)								const;
				ubyte			FindEdge(udword vref0, udword vref1)								const;
				udword			OppositeVertex(udword vref0, udword vref1)							const;
		inline_	udword			OppositeVertex(ubyte edgenb)										const	{ return mVRef[2-edgenb];	}
				void			GetVRefs(ubyte edgenb, udword& vref0, udword& vref1, udword& vref2)	const;
				float			MinEdgeLength(const OPC_Point* verts)									const;
				float			MaxEdgeLength(const OPC_Point* verts)									const;
				void			ComputePoint(const OPC_Point* verts, float u, float v, OPC_Point& pt, udword* nearvtx=null)	const;
				float			Angle(const IndexedTriangle& tri, const OPC_Point* verts)				const;
		inline_	Plane			PlaneEquation(const OPC_Point* verts)									const	{ return Plane(verts[mVRef[0]], verts[mVRef[1]], verts[mVRef[2]]);	}
				bool			Equal(const IndexedTriangle& tri)									const;
				CubeIndex		ComputeCubeIndex(const OPC_Point* verts)								const;
	};

#endif // __ICEINDEXEDTRIANGLE_H__
