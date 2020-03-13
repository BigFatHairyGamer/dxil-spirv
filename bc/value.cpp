/*
 * Copyright 2019-2020 Hans-Kristian Arntzen for Valve Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "value.hpp"
#include "instruction.hpp"
#include "type.hpp"
#include "context.hpp"
#include "logging.hpp"
#include "cast.hpp"
#include <string.h>

namespace LLVMBC
{
Value::Value(Type *type_, ValueKind kind_)
	: type(type_), kind(kind_)
{
}

void Value::set_tween_id(uint64_t id)
{
	tween_id = id;
}

uint64_t Value::get_tween_id() const
{
	return tween_id;
}

Type *Value::getType() const
{
	return type;
}

ValueKind Value::get_value_kind() const
{
	return kind;
}

bool Constant::is_base_of_value_kind(ValueKind kind)
{
	switch (kind)
	{
	case ValueKind::ConstantFP:
	case ValueKind::ConstantInt:
	case ValueKind::ConstantDataArray:
	case ValueKind::ConstantAggregateZero:
	case ValueKind::ConstantBase:
	case ValueKind::Undef:
		return true;

	default:
		return false;
	}
}

Constant::Constant(Type *type, ValueKind kind)
	: Value(type, kind)
{
}

ConstantInt *ConstantInt::get(Type *type, uint64_t value)
{
	auto &context = type->getContext();
	return context.construct<ConstantInt>(type, value);
}

const APInt &Constant::getUniqueInteger() const
{
	return apint;
}

void Constant::set_integer(const APInt &apint_)
{
	apint = apint_;
}

void Constant::set_float(const APFloat &apfloat_)
{
	apfloat = apfloat_;
}

APInt::APInt(Type *type_, uint64_t value_)
	: type(type_), value(value_)
{
}

APFloat::APFloat(Type *type_, uint64_t value_)
	: type(type_), value(value_)
{
}

int64_t APInt::getSExtValue() const
{
	switch (cast<IntegerType>(type)->getBitWidth())
	{
	case 1: return (value & 1) != 0 ? -1 : 0;
	case 8: return int8_t(value);
	case 16: return int16_t(value);
	case 32: return int32_t(value);
	case 64: return int64_t(value);
	default:
		LOGE("Unrecognized bitwidth.\n");
		return 0;
	}
}

uint64_t APInt::getZExtValue() const
{
	switch (cast<IntegerType>(type)->getBitWidth())
	{
	case 1: return value & 1;
	case 8: return uint8_t(value);
	case 16: return uint16_t(value);
	case 32: return uint32_t(value);
	case 64: return uint64_t(value);
	default:
		LOGE("Unrecognized bitwidth.\n");
		return 0;
	}
}

ConstantFP *ConstantFP::get(Type *type, uint64_t value)
{
	auto &context = type->getContext();
	return context.construct<ConstantFP>(type, value);
}

ConstantInt::ConstantInt(Type *type, uint64_t value)
	: Constant(type, ValueKind::ConstantInt)
{
	set_integer(APInt(type, value));
}

ConstantFP::ConstantFP(Type *type, uint64_t value)
	: Constant(type, ValueKind::ConstantFP)
{
	set_float(APFloat(type, value));
}

const APFloat &Constant::getValueAPF() const
{
	return apfloat;
}

float APFloat::convertToFloat() const
{
	switch (type->getTypeID())
	{
	case Type::TypeID::FloatTyID:
	{
		float f;
		auto u = uint32_t(value);
		static_assert(sizeof(f) == sizeof(u), "Float is not 32-bit.");
		memcpy(&f, &u, sizeof(float));
		return f;
	}
	case Type::TypeID::DoubleTyID:
	{
		double f;
		static_assert(sizeof(f) == sizeof(value), "Double is not 64-bit.");
		memcpy(&f, &value, sizeof(double));
		return float(f);
	}
	default:
		// TODO: FP16 -> FP32 conversion.
		LOGE("Unknown FP type.\n");
		return 0.0f;
	}
}

double APFloat::convertToDouble() const
{
	switch (type->getTypeID())
	{
	case Type::TypeID::FloatTyID:
	{
		float f;
		auto u = uint32_t(value);
		static_assert(sizeof(f) == sizeof(u), "Float is not 32-bit.");
		memcpy(&f, &u, sizeof(float));
		return double(f);
	}
	case Type::TypeID::DoubleTyID:
	{
		double f;
		static_assert(sizeof(f) == sizeof(value), "Double is not 64-bit.");
		memcpy(&f, &value, sizeof(double));
		return f;
	}
	default:
		// TODO: FP16 -> FP32 conversion.
		LOGE("Unknown FP type.\n");
		return 0.0f;
	}
}

UndefValue::UndefValue(Type *type)
	: Constant(type, ValueKind::Undef)
{
}

UndefValue *UndefValue::get(Type *type)
{
	auto &context = type->getContext();
	return context.construct<UndefValue>(type);
}

ConstantAggregateZero::ConstantAggregateZero(Type *type)
	: Constant(type, ValueKind::ConstantAggregateZero)
{
}

ConstantDataArray::ConstantDataArray(Type *type, std::vector<Constant *> elements_)
	: Constant(type, ValueKind::ConstantDataArray), elements(std::move(elements_))
{
}

unsigned ConstantDataArray::getNumElements() const
{
	return elements.size();
}

Constant *ConstantDataArray::getElementAsConstant(unsigned index) const
{
	return elements[index];
}

GlobalVariable::GlobalVariable(Type *type, bool is_const_)
	: Value(type, ValueKind::Global), is_const(is_const_)
{
}

bool GlobalVariable::hasInitializer() const
{
	return initializer != nullptr;
}

Constant *GlobalVariable::getInitializer() const
{
	return initializer;
}

void GlobalVariable::set_initializer(Constant *value)
{
	initializer = value;
}

bool GlobalVariable::isConstant() const
{
	return is_const;
}
}
