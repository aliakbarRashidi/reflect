#pragma once
#ifndef STRUCT_TYPE_HPP_PTB31EJN
#define STRUCT_TYPE_HPP_PTB31EJN

#include "type.hpp"
#include <memory>
#include <vector>
#include <new>
#include "attribute.hpp"

struct StructTypeBase : DerivedType {
	const std::string& name() const override { return name_; }
	const std::string& description() const { return description_; }
	Object* cast(const DerivedType* to, Object* o) const override;
	const StructTypeBase* super() const;
	virtual std::vector<const AttributeBase*> attributes() const = 0;
protected:
	StructTypeBase(const StructTypeBase* super, std::string name, std::string description) : super_(super), name_(std::move(name)), description_(std::move(description)) {}
	
	const StructTypeBase* super_;
	std::string name_;
	std::string description_;
};

template <typename T>
struct StructType : StructTypeBase {
	StructType(const StructTypeBase* super, std::string name, std::string description) : StructTypeBase(super, std::move(name), std::move(description)), is_abstract_(false) {}
	
	void construct(byte* place, IUniverse& universe) const override {
		T* p = ::new(place) T;
		p->set_object_type__(this);
		p->set_universe__(&universe);
	}
	void destruct(byte* place, IUniverse&) const override { reinterpret_cast<T*>(place)->~T(); }
	size_t size() const override { return sizeof(T); }
	
	void set_properties(std::vector<AttributeForObject<T>*> properties) {
		properties_ = std::move(properties);
	}
	
	std::vector<const AttributeBase*> attributes() const override {
		std::vector<const AttributeBase*> result;
		result.resize(properties_.size());
		for (auto& it: properties_) {
			result.push_back(dynamic_cast<const AttributeBase*>(it));
		}
		return result;
	}
	
	size_t num_elements() const override { return properties_.size(); }
	const Type* type_of_element(size_t idx) const override { return properties_[idx]->attribute_type(); }
	size_t offset_of_element(size_t idx) const override { return 0; /* TODO */ }
	
	void deserialize(byte* object, const ArchiveNode&) const override;
	void serialize(const byte* object, ArchiveNode&) const override;
	
	void set_abstract(bool b) { is_abstract_ = b; }
	bool is_abstract() const override { return is_abstract_; }
protected:
	std::vector<AttributeForObject<T>*> properties_;
	bool is_abstract_;
};


template <typename T>
void StructType<T>::deserialize(byte* object, const ArchiveNode& node) const {
	auto s = super();
	if (s) s->deserialize(object, node);
	
	T* obj = reinterpret_cast<T*>(object);
	for (auto& property: properties_) {
		property->deserialize_attribute(obj, node[property->attribute_name()]);
	}
}

template <typename T>
void StructType<T>::serialize(const byte* object, ArchiveNode& node) const {
	auto s = super();
	if (s) s->serialize(object, node);
	
	const T* obj = reinterpret_cast<const T*>(object);
	for (auto& property: properties_) {
		property->serialize_attribute(obj, node[property->attribute_name()]);
	}
	node["class"] = name();
}

#endif /* end of include guard: STRUCT_TYPE_HPP_PTB31EJN */
