///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for segments.
 *	\file		IceSegment.h
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICESEGMENT_H__
#define __ICESEGMENT_H__

	class ICEMATHS_API Segment
	{
		public:
		//! Constructor
		inline_					Segment()															{}
		//! Constructor
		inline_					Segment(const OPC_Point& p0, const OPC_Point& p1) : mP0(p0), mP1(p1)		{}
		//! Copy constructor
		inline_					Segment(const Segment& seg) : mP0(seg.mP0), mP1(seg.mP1)			{}
		//! Destructor
		inline_					~Segment()															{}

		inline_	const	OPC_Point&	GetOrigin()						const	{ return mP0;						}
		inline_			OPC_Point	ComputeDirection()				const	{ return mP1 - mP0;					}
		inline_			void	ComputeDirection(OPC_Point& dir)	const	{ dir = mP1 - mP0;					}
		inline_			float	ComputeLength()					const	{ return mP1.Distance(mP0);			}
		inline_			float	ComputeSquareLength()			const	{ return mP1.SquareDistance(mP0);	}

		inline_			void	SetOriginDirection(const OPC_Point& origin, const OPC_Point& direction)
								{
									mP0 = mP1 = origin;
									mP1 += direction;
								}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes a OPC_Point on the segment
		 *	\param		pt	[out] OPC_Point on segment
		 *	\param		t	[in] OPC_Point's parameter [t=0 => pt = mP0, t=1 => pt = mP1]
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_			void	ComputePoint(OPC_Point& pt, float t)	const	{	pt = mP0 + t * (mP1 - mP0);		}

						float	SquareDistance(const OPC_Point& OPC_Point, float* t=null)	const;
		inline_			float	Distance(const OPC_Point& point, float* t=null)			const			{ return sqrtf(SquareDistance(point, t));	}

						OPC_Point	mP0;		//!< Start of segment
						OPC_Point	mP1;		//!< End of segment
	};

#endif // __ICESEGMENT_H__
