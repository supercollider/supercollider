/*
 *    SuperCollider real time audio synthesis system
 *    Copyright (c) 2002 James McCartney. All rights reserved.
 *    Copyright (c) 2011 Tim Blechmann
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef SC_PLUGIN_HPP
#define SC_PLUGIN_HPP

#include "SC_PlugIn.h"

// c++ wrapper for Unit struct
class SCUnit:
	public Unit
{
public:
	const float * in(int index) const
	{
		const Unit * unit = this;
		return IN(index);
	}

	const float * zin(int index) const
	{
		const Unit * unit = this;
		return ZIN(index);
	}

	float const & in0(int index) const
	{
		const Unit * unit = this;
		return IN0(index);
	}

	float * out(int index) const
	{
		const Unit * unit = this;
		return OUT(index);
	}

	float * zout(int index) const
	{
		const Unit * unit = this;
		return ZOUT(index);
	}

	float & out0(int index) const
	{
		const Unit * unit = this;
		return OUT0(index);
	}

	int inRate(int index) const
	{
		const Unit * unit = this;
		return INRATE(index);
	}

	int inBufferSize(int index) const
	{
		const Unit * unit = this;
		return INBUFLENGTH(index);
	}

	/// @{
	double sampleRate() const
	{
		const Unit * unit = this;
		return SAMPLERATE;
	}

	double sampleDur() const
	{
		const Unit * unit = this;
		return SAMPLEDUR;
	}

	int bufferSize() const
	{
		return mBufLength;
	}

	double controlRate() const
	{
		const Unit * unit = this;
		return BUFRATE;
	}

	double controlDur() const
	{
		const Unit * unit = this;
		return BUFDUR;
	}

	double fullSampleRate() const
	{
		const Unit * unit = this;
		return FULLRATE;
	}

	int fullBufferSize() const
	{
		const Unit * unit = this;
		return FULLBUFLENGTH;
	}
	/// @}

	template <typename FloatType>
	FloatType calcSlope(FloatType next, FloatType prev) const
	{
		const Unit * unit = this;
		return CALCSLOPE(next, prev);
	}

	template <typename UnitType, void (UnitType::*PointerToMember)(int)>
	static UnitCalcFunc make_calc_function(void)
	{
		return &run_member_function<UnitType, PointerToMember>;
	}

	/// @{
	/// set calc function & compute initial sample
	template <typename UnitType, void (UnitType::*PointerToMember)(int)>
	void set_calc_function(void)
	{
		mCalcFunc = make_calc_function<UnitType, PointerToMember>();
		(mCalcFunc)(this, 1);
	}

	template <typename UnitType, void (UnitType::*VectorCalcFunc)(int), void (UnitType::*ScalarCalcFunc)(int)>
	void set_vector_calc_function(void)
	{
		mCalcFunc = make_calc_function<UnitType, VectorCalcFunc>();
		make_calc_function<UnitType, ScalarCalcFunc>()(this, 1);
	}
	/// @}

private:
	template <typename UnitType, void (UnitType::*PointerToMember)(int)>
	static void run_member_function(struct Unit * unit, int inNumSamples)
	{
		UnitType * realUnit = static_cast<UnitType*>(unit);
		((realUnit)->*(PointerToMember))(inNumSamples);
	}
};

// define Ctor/Dtor functions
#define DEFINE_XTORS(CLASSNAME)         \
void CLASSNAME##_Ctor(CLASSNAME * unit) \
{                                       \
	new(unit) CLASSNAME();              \
}                                       \
										\
void CLASSNAME##_Dtor(CLASSNAME * unit) \
{                                       \
	unit->~CLASSNAME();                 \
}

#endif /* SC_PLUGIN_HPP */
