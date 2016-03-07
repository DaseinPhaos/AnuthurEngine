//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
// Matrix3x3f
//
//----------------------------------------------------------------------------------------------------
#ifndef Matrix3x3f_h
#define Matrix3x3f_h
//----------------------------------------------------------------------------------------------------
#include "GVector3f.h"
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
namespace Glyph3
{
	class Matrix3x3f


	{
	public:
		Matrix3x3f( );
		Matrix3x3f( bool bZero );
		Matrix3x3f( const Matrix3x3f& Matrix );
		Matrix3x3f( float fM11, float fM12, float fM13,
				   float fM21, float fM22, float fM23,
				   float fM31, float fM32, float fM33 );
		
		void RotationX( float fRadians );
		void RotationY( float fRadians );
		void RotationZ( float fRadians );
		void Rotation( Vector3f& Rot );
		void RotationZYX( Vector3f& Rot );
		void RotationEuler( Vector3f& Axis, float Angle );
		void Orthonormalize();

		void MakeZero();
		void MakeIdentity();
		void MakeTranspose();

		Matrix3x3f Zero();
		Matrix3x3f Identity();
		Matrix3x3f Transpose();

		// Operators
		Matrix3x3f& operator= ( const Matrix3x3f& Matrix );

		// member access
		float operator() ( int iRow, int iCol ) const;
		float& operator() ( int iRow, int iCol );
		float operator[] ( int iPos ) const;
		float& operator[] ( int iPos );

		void SetRow( int iRow, const Vector3f& Vector );
		Vector3f GetRow( int iRow ) const;
		void SetColumn( int iCol, const Vector3f& Vector );
		Vector3f GetColumn( int iCol ) const;

		// comparison
		bool operator== ( const Matrix3x3f& Matrix ) const;
		bool operator!= ( const Matrix3x3f& Matrix ) const;

		// arithmetic operations
		Matrix3x3f operator+ ( const Matrix3x3f& Matrix ) const;
		Matrix3x3f operator- ( const Matrix3x3f& Matrix ) const;
		Matrix3x3f operator* ( const Matrix3x3f& Matrix ) const;
		Matrix3x3f operator* ( float fScalar) const;
		Matrix3x3f operator/ ( float fScalar) const;
		Matrix3x3f operator- ( ) const;

		// arithmetic updates
		Matrix3x3f& operator+= ( const Matrix3x3f& Matrix );
		Matrix3x3f& operator-= ( const Matrix3x3f& Matrix );
		Matrix3x3f& operator*= ( const Matrix3x3f& Matrix );
		Matrix3x3f& operator*= ( float fScalar );
		Matrix3x3f& operator/= ( float fScalar );

		// matrix - vector operations
		Vector3f operator* ( const Vector3f& rkV ) const;  // M * v
		float Determinant()const;
		Matrix3x3f Inverse()const;

	protected:
		float m_afEntry[3*3];

		static int I(int iRow, int iCol); // iRow*N + iCol
	};
};
//----------------------------------------------------------------------------------------------------
#endif // Matrix3x3f_h
