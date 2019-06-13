/*
* Copyright (c) 2011 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B2_ROPE_H
#define B2_ROPE_H

#include <Box2D/Common/b2Math.h>

class b2Draw;

/// 
struct b2RopeDef
{
	b2RopeDef()
	{
		vertices = NULL;
		count = 0;
		masses = NULL;
		gravity.SetZero();
		damping = 0.1f;
		k2 = 0.9f;
		k3 = 0.1f;
	}

	///
	b2Vec2* vertices;

	///
	b2_int32 count;

	///
	b2_float32* masses;

	///
	b2Vec2 gravity;

	///
	b2_float32 damping;

	/// Stretching stiffness
	b2_float32 k2;

	/// Bending stiffness. Values above 0.5 can make the simulation blow up.
	b2_float32 k3;
};

/// 
class b2Rope
{
public:
	b2Rope();
	~b2Rope();

	///
	void Initialize(const b2RopeDef* def);

	///
	void Step(b2_float32 timeStep, b2_int32 iterations);

	///
	b2_int32 GetVertexCount() const
	{
		return m_count;
	}

	///
	const b2Vec2* GetVertices() const
	{
		return m_ps;
	}

	///
	void Draw(b2Draw* draw) const;

	///
	void SetAngle(b2_float32 angle);

private:

	void SolveC2();
	void SolveC3();

	b2_int32 m_count;
	b2Vec2* m_ps;
	b2Vec2* m_p0s;
	b2Vec2* m_vs;

	b2_float32* m_ims;

	b2_float32* m_Ls;
	b2_float32* m_as;

	b2Vec2 m_gravity;
	b2_float32 m_damping;

	b2_float32 m_k2;
	b2_float32 m_k3;
};

#endif
