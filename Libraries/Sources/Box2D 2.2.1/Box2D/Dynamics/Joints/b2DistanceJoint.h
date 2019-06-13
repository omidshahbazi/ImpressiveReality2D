/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
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

#ifndef B2_DISTANCE_JOINT_H
#define B2_DISTANCE_JOINT_H

#include <Box2D/Dynamics/Joints/b2Joint.h>

/// Distance joint definition. This requires defining an
/// anchor point on both bodies and the non-zero length of the
/// distance joint. The definition uses local anchor points
/// so that the initial configuration can violate the constraint
/// slightly. This helps when saving and loading a game.
/// @warning Do not use a zero or short length.
struct b2DistanceJointDef : public b2JointDef
{
	b2DistanceJointDef()
	{
		type = e_distanceJoint;
		localAnchorA.Set(0.0f, 0.0f);
		localAnchorB.Set(0.0f, 0.0f);
		length = 1.0f;
		frequencyHz = 0.0f;
		dampingRatio = 0.0f;
	}

	/// Initialize the bodies, anchors, and length using the world
	/// anchors.
	void Initialize(b2Body* bodyA, b2Body* bodyB,
					const b2Vec2& anchorA, const b2Vec2& anchorB);

	/// The local anchor point relative to bodyA's origin.
	b2Vec2 localAnchorA;

	/// The local anchor point relative to bodyB's origin.
	b2Vec2 localAnchorB;

	/// The natural length between the anchor points.
	b2_float32 length;

	/// The mass-spring-damper frequency in Hertz. A value of 0
	/// disables softness.
	b2_float32 frequencyHz;

	/// The damping ratio. 0 = no damping, 1 = critical damping.
	b2_float32 dampingRatio;
};

/// A distance joint constrains two points on two bodies
/// to remain at a fixed distance from each other. You can view
/// this as a massless, rigid rod.
class b2DistanceJoint : public b2Joint
{
public:

	b2Vec2 GetAnchorA() const;
	b2Vec2 GetAnchorB() const;

	/// Get the reaction force given the inverse time step.
	/// Unit is N.
	b2Vec2 GetReactionForce(b2_float32 inv_dt) const;

	/// Get the reaction torque given the inverse time step.
	/// Unit is N*m. This is always zero for a distance joint.
	b2_float32 GetReactionTorque(b2_float32 inv_dt) const;

	/// The local anchor point relative to bodyA's origin.
	const b2Vec2& GetLocalAnchorA() const { return m_localAnchorA; }

	/// The local anchor point relative to bodyB's origin.
	const b2Vec2& GetLocalAnchorB() const  { return m_localAnchorB; }

	/// Set/get the natural length.
	/// Manipulating the length can lead to non-physical behavior when the frequency is zero.
	void SetLength(b2_float32 length);
	b2_float32 GetLength() const;

	/// Set/get frequency in Hz.
	void SetFrequency(b2_float32 hz);
	b2_float32 GetFrequency() const;

	/// Set/get damping ratio.
	void SetDampingRatio(b2_float32 ratio);
	b2_float32 GetDampingRatio() const;

	/// Dump joint to dmLog
	void Dump();

protected:

	friend class b2Joint;
	b2DistanceJoint(const b2DistanceJointDef* data);

	void InitVelocityConstraints(const b2SolverData& data);
	void SolveVelocityConstraints(const b2SolverData& data);
	bool SolvePositionConstraints(const b2SolverData& data);

	b2_float32 m_frequencyHz;
	b2_float32 m_dampingRatio;
	b2_float32 m_bias;

	// Solver shared
	b2Vec2 m_localAnchorA;
	b2Vec2 m_localAnchorB;
	b2_float32 m_gamma;
	b2_float32 m_impulse;
	b2_float32 m_length;

	// Solver temp
	b2_int32 m_indexA;
	b2_int32 m_indexB;
	b2Vec2 m_u;
	b2Vec2 m_rA;
	b2Vec2 m_rB;
	b2Vec2 m_localCenterA;
	b2Vec2 m_localCenterB;
	b2_float32 m_invMassA;
	b2_float32 m_invMassB;
	b2_float32 m_invIA;
	b2_float32 m_invIB;
	b2_float32 m_mass;
};

inline void b2DistanceJoint::SetLength(b2_float32 length)
{
	m_length = length;
}

inline b2_float32 b2DistanceJoint::GetLength() const
{
	return m_length;
}

inline void b2DistanceJoint::SetFrequency(b2_float32 hz)
{
	m_frequencyHz = hz;
}

inline b2_float32 b2DistanceJoint::GetFrequency() const
{
	return m_frequencyHz;
}

inline void b2DistanceJoint::SetDampingRatio(b2_float32 ratio)
{
	m_dampingRatio = ratio;
}

inline b2_float32 b2DistanceJoint::GetDampingRatio() const
{
	return m_dampingRatio;
}

#endif
