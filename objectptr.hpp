#pragma once
#ifndef OBJECTPTR_HPP_WICLN6JL
#define OBJECTPTR_HPP_WICLN6JL

#include "type.hpp"
#include "object.hpp"
#include "reference_type.hpp"

template <typename T, typename Enable = void>
struct ObjectPtr;

template <typename T>
struct ObjectPtr<T, typename std::enable_if<IsDerivedFromObject<T>::Value>::type> {
	typedef T PointeeType;
	
	ObjectPtr() : ptr_(nullptr) {}
	ObjectPtr(T* ptr) : ptr_(ptr) {}
	template <typename U>
	ObjectPtr(ObjectPtr<U> other) { ptr_ = other.ptr_; }
	ObjectPtr(const ObjectPtr<T>& other) = default;
	ObjectPtr(ObjectPtr<T>&& other) = default;
	template <typename U>
	ObjectPtr<T>& operator=(U* other) { ptr_ = other; return *this; }
	template <typename U>
	ObjectPtr<T>& operator=(ObjectPtr<U> other) { ptr_ = other.ptr_; return *this; }
	template <typename U>
	bool operator==(ObjectPtr<U> other) const { ptr_ == other.ptr_; }
	template <typename U>
	bool operator!=(ObjectPtr<U> other) const { ptr_ != other.ptr_; }
	
	template <typename U>
	ObjectPtr<U> cast() const {
		return aspect_cast<U>(ptr_);
	}
	
	T* get() const { return ptr_; }
	T* operator->() const { return ptr_; }
	T& operator*() const { return *ptr_; }
private:
	T* ptr_;
};

template <typename T>
struct BuildTypeInfo<ObjectPtr<T>> {
	static const ReferenceTypeImpl<ObjectPtr<T>>* build() {
		static const ReferenceTypeImpl<ObjectPtr<T>> type("ObjectPtr");
		return &type;
	}
};

#endif /* end of include guard: OBJECTPTR_HPP_WICLN6JL */